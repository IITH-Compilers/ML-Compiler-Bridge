//===----------------------------------------------------------------------===//
//
// Part of the MLCompilerBridge Project, under the Apache License v2.0 with LLVM
// Exceptions. See the LICENSE file for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MLModelRunner/MLModelRunner.h"
#include "MLModelRunner/ONNXModelRunner/ONNXModelRunner.h"
#include "MLModelRunner/PipeModelRunner.h"
#include "MLModelRunner/TFModelRunner.h"
#include "MLModelRunner/Utils/DataTypes.h"
#include "MLModelRunner/Utils/MLConfig.h"
#include "MLModelRunner/gRPCModelRunner.h"
// #include "grpc/helloMLBridgeTest/helloMLBridgeTest.grpc.pb.h"
// #include "grpc/helloMLBridgeTest/helloMLBridgeTest.pb.h"
#include "grpcpp/impl/codegen/status.h"
#include "inference/HelloMLBridge_Env.h"
#include "llvm/Support/CommandLine.h"
// #include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <fstream>
#include <google/protobuf/text_format.h>
#include <iostream>
#include <iterator>
#include <memory>
#include <random>
#include <string>

#define debug_out                                                              \
  if (!silent)                                                                 \
  std::cout

static llvm::cl::opt<std::string>
    cl_server_address("test-server-address", llvm::cl::Hidden,
                      llvm::cl::desc("Server address, format <ip>:<port>"),
                      llvm::cl::init(""));

static llvm::cl::opt<std::string>
    cl_pipe_name("test-pipe-name", llvm::cl::Hidden, llvm::cl::init(""),
                 llvm::cl::desc("Name for pipe file"));

static llvm::cl::opt<std::string> cl_test_config(
    "test-config", llvm::cl::Hidden,
    llvm::cl::desc("Method for communication with python model"));

static llvm::cl::opt<bool>
    silent("silent", llvm::cl::Hidden, llvm::cl::init(false),
           llvm::cl::desc("Only print errors when if set to true"));

namespace {
std::unique_ptr<MLModelRunner> MLRunner;
std::string basename;
BaseSerDes::Kind SerDesType;

std::string test_config;
std::string data_format;
std::string pipe_name;
std::string server_address;

// send value of type T1. Test received value of type T2 against expected value
template <typename T1, typename T2>
void testPrimitive(std::string label, T1 value, T2 expected) {
  std::pair<std::string, T1> p("request_" + label, value);
  MLRunner->populateFeatures(p);
  T2 out = MLRunner->evaluate<T2>();
  debug_out << "  " << label << " reply: " << out << "\n";
  if (std::abs(out - expected) > 10e-6) {
    std::cerr << "Error: Expected " << label << " reply: " << expected
              << ", Received: " << out << "\n";
    exit(1);
  }
}

template <typename T1, typename T2>
void testVector(std::string label, std::vector<T1> value,
                std::vector<T2> expected) {
  std::pair<std::string, std::vector<T1>> p("request_" + label, value);
  MLRunner->populateFeatures(p);
  T2 *out;
  size_t size;
  MLRunner->evaluate<T2 *>(out, size);
  std::vector<T2> reply(out, out + size);
  debug_out << "  " << label << " reply: ";
  int i = 0;
  for (auto x : reply) {
    debug_out << x << " ";
    if (std::abs(x - expected[i]) > 10e-6) {
      std::cerr << "Error: Expected " << label << " reply: " << expected[i]
                << ", Received: " << x << "\n";
      exit(1);
    }
    i++;
  }
  debug_out << "\n";
}

void runTests() {
  if (data_format != "json") {
    testPrimitive<int, int>("int", 11, 12);
    testPrimitive<long, long>("long", 1234567890, 1234567891);
    testPrimitive<float, float>("float", 3.14, 4.14);
    testPrimitive<double, double>("double", 0.123456789123456789,
                                  1.123456789123456789);
    testPrimitive<char, char>("char", 'a', 'b');
    testPrimitive<bool, bool>("bool", true, false);
    testVector<int, int>("vec_int", {11, 22, 33}, {12, 23, 34});
    testVector<long, long>("vec_long", {123456780, 222, 333},
                           {123456780, 123456781, 123456782});
    testVector<float, float>("vec_float", {11.1, 22.2, 33.3},
                             {1.11, 2.22, -3.33, 0});
    testVector<double, double>("vec_double",
                               {-1.1111111111, -2.2222222222, -3.3333333333},
                               {1.12345678912345670, -1.12345678912345671});
  } else if (data_format == "json") {
    testPrimitive<int, IntegerType>("int", 11, 12);
    testPrimitive<long, IntegerType>("long", 1234567890, 12345);
    testPrimitive<float, RealType>("float", 3.14, 4.14);
    testPrimitive<double, RealType>("double", 0.123456789123456789,
                                    1.123456789123456789);
    testPrimitive<char, char>("char", 'a', 'b');
    testPrimitive<bool, bool>("bool", true, false);
    testVector<int, IntegerType>("vec_int", {11, 22, 33}, {12, 23, 34});
    testVector<long, IntegerType>("vec_long", {123456780, 222, 333},
                                  {6780, 6781, 6782});
    testVector<float, RealType>("vec_float", {11.1, 22.2, 33.3},
                                {1.11, 2.22, -3.33, 0});
    testVector<double, RealType>("vec_double",
                                 {-1.1111111111, -2.2222222222, -3.3333333333},
                                 {1.12345678912345670, -1.12345678912345671});
  }
}

int testPipes() {
  if (pipe_name == "") {
    std::cerr
        << "Pipe name must be specified via --test-pipe-name=<filename>\n";
    exit(1);
  }
  basename = "/tmp/" + pipe_name;
  if (data_format == "json")
    SerDesType = BaseSerDes::Kind::Json;
  else if (data_format == "protobuf")
    SerDesType = BaseSerDes::Kind::Protobuf;
  else if (data_format == "bytes")
    SerDesType = BaseSerDes::Kind::Bitstream;
  else {
    std::cout << "Invalid data format\n";
    exit(1);
  }

  MLRunner = std::make_unique<PipeModelRunner>(
      basename + ".out", basename + ".in", SerDesType, nullptr);

  runTests();
  return 0;
}

int testGRPC() {
  if (server_address == "") {
    std::cerr << "Server Address must be specified via "
                 "--test-server-address=<ip>:<port>\n";
    exit(1);
  }
  return 0;
}

int testONNX() { return 0; }

} // namespace

int main(int argc, char **argv) {
  llvm::cl::ParseCommandLineOptions(argc, argv);
  test_config = cl_test_config.getValue();

  if (test_config == "pipe-bytes") {
    pipe_name = cl_pipe_name.getValue();
    data_format = "bytes";
    testPipes();
  } else if (test_config == "pipe-json") {
    pipe_name = cl_pipe_name.getValue();
    data_format = "json";
    testPipes();
  } else if (test_config == "grpc") {
    server_address = cl_server_address.getValue();
    testGRPC();
  } else if (test_config == "onnx")
    testONNX();
  else
    std::cerr << "--test-config must be provided from [pipe-bytes, pipe-json, "
                 "grpc, onnx]\n";
  return 0;
}
