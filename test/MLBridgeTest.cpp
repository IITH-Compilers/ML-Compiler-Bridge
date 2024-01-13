#include "MLModelRunner/MLModelRunner.h"
#include "MLModelRunner/ONNXModelRunner/ONNXModelRunner.h"
#include "MLModelRunner/PipeModelRunner.h"
#include "MLModelRunner/TFModelRunner.h"
#include "MLModelRunner/Utils/MLConfig.h"
#include "MLModelRunner/gRPCModelRunner.h"
// #include "grpc/helloMLBridgeTest/helloMLBridgeTest.grpc.pb.h"
// #include "grpc/helloMLBridgeTest/helloMLBridgeTest.pb.h"
#include "grpcpp/impl/codegen/status.h"
#include "inference/HelloMLBridge_Env.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <fstream>
#include <google/protobuf/text_format.h>
#include <iostream>
#include <iterator>
#include <memory>
#include <random>
#include <string>

static cl::opt<std::string>
    cl_server_address("test-server-address", cl::Hidden,
                      cl::desc("Server address, format <ip>:<port>"),
                      cl::init("localhost:5050"));

static cl::opt<std::string> cl_pipe_name("test-pipe-name", cl::Hidden,
                                         cl::init("hellopipe"),
                                         cl::desc("Name for pipe file"));

static cl::opt<std::string>
    cl_test_config("test-config", cl::Hidden, cl::init("pipe-bytes"),
                   cl::desc("Method for communication with python model"));

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
  std::cout << label << " reply: " << out << "\n";
  assert(std::abs(out - expected) < 10e-6 && "Mismatch in values");
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
  std::cout << label << " reply: ";
  int i = 0;
  for (auto x : reply) {
    std::cout << x << " ";
    assert(std::abs(x - expected[i++]) < 10e-6 && "Mismatch in values");
  }
  std::cout << "\n";
}

void runTests() {
  if (data_format == "json")
    testPrimitive<int, long>("int", 11, 12);
  else
    testPrimitive<int, int>("int", 11, 12);

  testPrimitive<long, long>("long", 1234567890, 1234567891);

  if (data_format == "json")
    testPrimitive<float, double>("float", 3.14, 4.14);
  else
    testPrimitive<float, float>("float", 3.14, 4.14);

  testPrimitive<double, double>("double", 0.123456789123456789,
                                1.123456789123456789);
  testPrimitive<char, char>("char", 'a', 'b');
  testPrimitive<bool, bool>("bool", true, false);

  if (data_format == "json")
    testVector<int, long>("vec_int", {11, 22, 33}, {12, 23, 34});
  else
    testVector<int, int>("vec_int", {11, 22, 33}, {12, 23, 34});

  testVector<long, long>("vec_long", {123456780, 222, 333},
                         {123456780, 123456781, 123456782});

  if (data_format == "json")
    testVector<float, double>("vec_float", {11.1, 22.2, 33.3},
                              {1.11, 2.22, -3.33, 0});
  else
    testVector<float, float>("vec_float", {11.1, 22.2, 33.3},
                             {1.11, 2.22, -3.33, 0});

  testVector<double, double>("vec_double",
                             {-1.1111111111, -2.2222222222, -3.3333333333},
                             {1.12345678912345670, -1.12345678912345671});
}

int testPipes() {
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

int testGRPC() { return 0; }

int testONNX() { return 0; }

} // namespace

int main(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv);
  test_config = cl_test_config.getValue();
  pipe_name = cl_pipe_name.getValue();
  server_address = cl_server_address.getValue();

  if (test_config == "pipe-bytes") {
    data_format = "bytes";
    testPipes();
  } else if (test_config == "pipe-json") {
    data_format = "json";
    testPipes();
  } else if (test_config == "grpc")
    testGRPC();
  else if (test_config == "onnx")
    testONNX();
  else
    std::cout
        << "test-config must belong to [pipe-bytes, pipe-json, grpc, onnx]\n";
  return 0;
}