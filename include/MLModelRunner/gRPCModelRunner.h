//=== MLModelRunner/gRPCModelRunner.h -MLConfig class definition - C++ -*--===//
//
// Part of the MLCompilerBridge Project, under the Apache 2.0 License.
// See the LICENSE file under home directory for license and copyright
// information.
//
//===---------------------------------------------------------------------===//
//
// gRPCModelRunner class supporting communication via gRPC. This class is used
// to communicate with the gRPC server and send/receive data to/from the model.
// Supports interleaved communication with the model.
//
// There are two ways to use this class:
// 1. Training mode - gRPC Server: In this mode, the gRPCModelRunner object is
// created with the server address and the service object. The service object is
// used to create the server and the server waits for the client to connect.
// Once the client connects, the server waits for the client to send the request
// and then responds with the result.
// In Training mode, GrpcCompilerInterface class from GrpcCompilerInterface.py
// acts as the client and sends the request to the server.
//
// 2. Inference mode - gRPC Client: In this mode, the gRPCModelRunner object is
// created with the server address, request and response objects. The request
// object is used to send the features to the model and the response object is
// used to receive the result from the model.
//
// In Inference mode, the compiler pass using this class acts as the client and
// sends the request to the server implemented by using
// GrpcCompilerInterface.py.
//
//===---------------------------------------------------------------------===//
//
// Interfacing with the model using protobuf (.proto) files:
// Users should define the service, stub, request and response by writing a
// .proto file. The service and stub objects are generated using the protoc
// compiler. The request and response objects are generated using the protoc
// compiler or created by the user. The build process will automatically compile
// .proto files and generates the relevant stubs and request/response objects in
// both CPP and Python which will be used by gRPCModelRunner and
// GrpcCompilerInterface respectively..
//
// In Inference mode, users should override `getAdvice()` RPC method in the
// Python model code to process the request and send the response back to the
// client. This method is called by the gRPC server in the evaluate_untyped()
// method of gRPCModelRunner class to get the result from the model after
// populating the features in the request object.
//
// In Training mode, users should override the RPC function/service that they
// declare in the .proto file in the Compiler pass which is using the
// gRPCModelRunner. This function is called by the gRPC client (Python model)
// that is using the GrpcCompilerInterface class to send the request to the
// server.
//
// Usage:
// 1. Create a .proto file with the service and message definitions
// 2. Generate the stubs using protoc
// 3. Create a gRPCModelRunner object with the server address, stub, request and
// response
// 4. Populate the features to be sent to the model
// 5. Call evaluate() to get the result back from the model
//
// ===----------------------------------------------------------------------===//

#ifndef GRPC_MODELRUNNER_H
#define GRPC_MODELRUNNER_H

#include "MLModelRunner/MLModelRunner.h"

#include <future>
#include <google/protobuf/text_format.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <memory>

namespace MLBridge {
/// grpc model runner requires service, stub, request and response
template <class Client, class Stub, class Request, class Response>
class gRPCModelRunner : public MLModelRunner {
public:
  /// For server mode
  gRPCModelRunner(std::string server_address, grpc::Service *s,
                  llvm::LLVMContext *Ctx = nullptr)
      : MLModelRunner(MLModelRunner::Kind::gRPC, BaseSerDes::Kind::Protobuf,
                      Ctx),
        server_address(server_address), request(nullptr), response(nullptr),
        server_mode(true) {
    RunService(s);
  }

  /// For client mode
  gRPCModelRunner(std::string server_address, Request *request,
                  Response *response, llvm::LLVMContext *Ctx = nullptr)
      : MLModelRunner(MLModelRunner::Kind::gRPC, BaseSerDes::Kind::Protobuf,
                      Ctx),
        server_address(server_address), request(request), response(response),
        server_mode(false) {
    SetStub();
  }

  void requestExit() override {
    std::cout << "Exit from grpc\n";
    exit_requested->set_value();
  }

  std::promise<void> *exit_requested;

  void *evaluateUntyped() override {
    assert(!server_mode &&
           "evaluateUntyped not implemented for gRPCModelRunner; "
           "Override gRPC method instead");
    assert(request != nullptr && "Request cannot be null");
    grpc::ClientContext grpcCtx;
    request = getRequest();
    auto status = stub_->getAdvice(&grpcCtx, *request, response);
    if (!status.ok())
      if (Ctx)
        Ctx->emitError("gRPC failed: " + status.error_message());
      else
        std::cerr << "gRPC failed: " << status.error_message() << std::endl;
    return SerDes->deserializeUntyped(response);
  }

private:
  Stub *stub_;
  std::string server_address;
  Request *request;
  Response *response;
  bool server_mode;

  int RunService(grpc::Service *s) {
    exit_requested = new std::promise<void>();
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(s);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server Listening on " << server_address << std::endl;
    auto serveFn = [&]() { server->Wait(); };
    std::thread serving_thread(serveFn);
    auto f = exit_requested->get_future();
    f.wait();
    server->Shutdown();
    serving_thread.join();
    return 0;
  }

  int SetStub() {
    std::shared_ptr<grpc::Channel> channel =
        grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    auto Stub_temp = Client::NewStub(channel);
    stub_ = Stub_temp.release();
    return 0;
  }

  Request *getRequest() { return (Request *)SerDes->getRequest(); }

  Response *getResponse() { return (Response *)SerDes->getResponse(); }

  void printMessage(const google::protobuf::Message *message) {
    std::string s;
    if (google::protobuf::TextFormat::PrintToString(*message, &s)) {
      std::cout << "Your message: " << s << std::endl;
    } else {
      std::cerr << "Message not valid (partial content: "
                << request->ShortDebugString() << ")\n";
    }
  }
};
} // namespace MLBridge

#endif // GRPC_MODELRUNNER_H
