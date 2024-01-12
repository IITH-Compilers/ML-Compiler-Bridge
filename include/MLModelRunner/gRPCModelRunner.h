//===----------------------------------------------------------------------===//
//
// Part of the ml-llvm-tools Project, under the BSD 4-Clause License.
// See the LICENSE.txt file under ml-llvm-tools directory for license
// information.
//
//===----------------------------------------------------------------------===//

#ifndef GRPC_MODELRUNNER_H
#define GRPC_MODELRUNNER_H

#include "MLModelRunner/MLModelRunner.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"
#include <future>
#include <google/protobuf/text_format.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <memory>

// grpc model runner requires service, stub, request and response
namespace MLBridge {
template <class Client, class Stub, class Request, class Response>
class gRPCModelRunner : public MLModelRunner {
public:
  gRPCModelRunner(std::string server_address, grpc::Service *s,
                  LLVMContext *Ctx = nullptr) // For server mode
      : MLModelRunner(MLModelRunner::Kind::gRPC, BaseSerDes::Kind::Protobuf,
                      Ctx),
        server_address(server_address), request(nullptr), response(nullptr),
        server_mode(true) {
    RunService(s);
  }

  gRPCModelRunner(std::string server_address, Request *request,
                  Response *response,
                  LLVMContext *Ctx = nullptr) // For client mode
      : MLModelRunner(MLModelRunner::Kind::gRPC, BaseSerDes::Kind::Protobuf,
                      Ctx),
        server_address(server_address), request(request), response(response),
        server_mode(false) {
    SetStub();
  }

  // void *getStub() { return stub_; }
  void requestExit() override {
    errs() << "Exit from grpc\n";
    exit_requested->set_value();
  }

  std::promise<void> *exit_requested;

  void *evaluateUntyped() override {
    if (server_mode)
      llvm_unreachable("evaluateUntyped not implemented for gRPCModelRunner; "
                       "Override gRPC method instead");
    assert(request != nullptr && "Request cannot be null");
    grpc::ClientContext grpcCtx;
    request = getRequest();
    auto status = stub_->getAdvice(&grpcCtx, *request, response);
    if (!status.ok())
      if (Ctx)
        Ctx->emitError("gRPC failed: " + status.error_message());
      else
        llvm_unreachable(("gRPC failed: " + status.error_message()).c_str());
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
    errs() << "In gRPCModelRunner printMessage...\n";
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
