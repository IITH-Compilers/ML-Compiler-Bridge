//===----------------------------------------------------------------------===//
//
// Part of the ml-llvm-tools Project, under the BSD 4-Clause License.
// See the LICENSE.txt file under ml-llvm-tools directory for license
// information.
//
// gRPCModelRunner class supporting communication via gRPC
//
// How to use?
// 1. Create a .proto file with the service and message definitions
// 2. Generate the stubs using protoc
// 3. Create a gRPCModelRunner object with the server address, stub, request and
// response
// 4. Populate the features to be sent to the model
// 5. Call evaluate() to get the result back from the model
// ===----------------------------------------------------------------------===//

#ifndef GRPC_MODELRUNNER_H
#define GRPC_MODELRUNNER_H

#include "MLModelRunner/MLModelRunner.h"

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
                  llvm::LLVMContext *Ctx = nullptr) // For server mode
      : MLModelRunner(MLModelRunner::Kind::gRPC, BaseSerDes::Kind::Protobuf,
                      Ctx),
        server_address(server_address), request(nullptr), response(nullptr),
        server_mode(true) {
    RunService(s);
  }

  gRPCModelRunner(std::string server_address, Request *request,
                  Response *response,
                  llvm::LLVMContext *Ctx = nullptr) // For client mode
      : MLModelRunner(MLModelRunner::Kind::gRPC, BaseSerDes::Kind::Protobuf,
                      Ctx),
        server_address(server_address), request(request), response(response),
        server_mode(false) {
    SetStub();
  }

  void requestExit() override {
    sigset_t wset;
    sigemptyset(&wset);
    sigaddset(&wset, SIGKILL);
    sigaddset(&wset, SIGTERM);
    sigaddset(&wset, SIGQUIT);
    int sig;
    if (sigwait(&wset, &sig) != -1) {
      this->exit_requested->set_value();
    } else {
      std::cout << "Problem while closing server\n";
    }
  }

  // checks whether a port number is available or not
  bool isPortAvailable(std::string addr) {
    int max_retries = 30, attempts = 0;
    double wait_seconds = 0.2, backoff_exp = 1.2;

    int idx = addr.find(":");
    int port = stoi(addr.substr(idx + 1, addr.size() - idx - 1));

    while (attempts < max_retries) {
      std::string command = "lsof -i :" + std::to_string(port);
      FILE* pipe = popen(command.c_str(), "r");
      if (!pipe) {
          std::cerr << "Error executing command: " << std::strerror(errno) << std::endl;
          return false;
      }

      char buffer[256];
      std::string result = "";
      while (!feof(pipe)) {
          if (fgets(buffer, 256, pipe) != nullptr)
              result += buffer;
      }
      pclose(pipe);

      if (result.empty()) {
          return true;
      }
      attempts++;
      std::cout << "Port is unavailable retrying! attempt: " << attempts << std::endl;
      std::this_thread::sleep_for(std::chrono::duration<double>(wait_seconds));
      wait_seconds *= backoff_exp;
    }

    std::cout << "Port is unavailable now!" << std::endl;
    return false;
  }


  std::promise<void> *exit_requested;

  void *evaluateUntyped() override {
    assert(!server_mode &&
           "evaluateUntyped not implemented for gRPCModelRunner; "
           "Override gRPC method instead");
    assert(request != nullptr && "Request cannot be null");
    
    int max_retries = 30, attempts = 0;
    double retries_wait_secs = 0.2;
    int deadline_time = 1000;
    int deadline_max_retries = 30, deadline_attpts = 0;
    double retry_wait_backoff_exponent = 1.5;

    // setting a deadline
    auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(deadline_time);

    while (attempts < max_retries && deadline_attpts < deadline_max_retries) {
      grpc::ClientContext grpcCtx;
      request = getRequest();
      grpc::Status status;
      grpcCtx.set_deadline(deadline);
      
      status = stub_->getAdvice(&grpcCtx, *request, response);

      if (status.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
        deadline_attpts++;
        int ext_deadline = 2 * deadline_time;
        deadline_time = ext_deadline;
        std::cout << "Deadline Exceeded for Request! sending the message again with extended deadline : " << deadline_time << "\n";
        deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(deadline_time);
      }
      else if (status.error_code() == grpc::StatusCode::UNAVAILABLE) {
        attempts++;
        std::cout << "Server is unavailable retrying! attempt: " << attempts << "\n";
        std::this_thread::sleep_for(std::chrono::duration<double>(retries_wait_secs));
        retries_wait_secs *= retry_wait_backoff_exponent;
      }
      else {
        request->Clear();
        if (!status.ok())
          Ctx->emitError("gRPC failed: " + status.error_message());
        // auto *action = new int(); // Hard wired for PosetRL case, should be fixed
        // *action = response->action();
        // return action;
        return SerDes->deserializeUntyped(response);
      }
    }

    std::cout << "Server is unavailable now!!!\n";
    return new int(-1);
  }

private:
  Stub *stub_;
  std::string server_address;
  Request *request;
  Response *response;
  bool server_mode;
  // std::mutex lock_server;

  int RunService(grpc::Service *s) {
    exit_requested = new std::promise<void>();
    grpc::ServerBuilder builder;
    // lock_server.lock();
    // if (!this->isPortAvailable(server_address)) return -1;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(s);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server Listening on " << server_address << std::endl;
    auto serveFn = [&]() { server->Wait(); };
    std::thread serving_thread(serveFn);
    auto f = exit_requested->get_future();
    this->requestExit();
    f.wait();
    server->Shutdown();
    serving_thread.join();
    // lock_server.unlock();
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
