//===----------------------------------------------------------------------===//
//
// Part of the ml-llvm-tools Project, under the BSD 4-Clause License.
// See the LICENSE.txt file under ml-llvm-tools directory for license
// information.
//
// ONNXModelRunner class supporting communication via ONNX Runtime
//
// How to use?
// 1. Create agent objects with the path to the ONNX model
// 2. Create an environment object inheriting from MLBridge::Environment
// 3. Create an ONNXModelRunner object with the environment and the agents
// 4. Populate the features to be sent to the model
// 5. Call evaluate() to get the result back from the model
//
//===----------------------------------------------------------------------===//

#ifndef ONNX_MODELRUNNER_H
#define ONNX_MODELRUNNER_H

#include "MLModelRunner/MLModelRunner.h"
#include "MLModelRunner/ONNXModelRunner/agent.h"
#include "MLModelRunner/ONNXModelRunner/environment.h"

namespace MLBridge {
class ONNXModelRunner : public MLModelRunner {
public:
  ONNXModelRunner(MLBridge::Environment *env,
                  std::map<std::string, Agent *> agents,
                  llvm::LLVMContext *Ctx = nullptr);
  void setEnvironment(MLBridge::Environment *_env) { env = _env; }
  MLBridge::Environment *getEnvironment() { return env; }
  void addAgent(Agent *agent, std::string name);
  void computeAction(Observation &obs);

  void requestExit() override {}

  void *evaluateUntyped() override;

private:
  MLBridge::Environment *env;
  std::map<std::string, Agent *> agents;
};
} // namespace MLBridge
#endif // ONNX_MODELRUNNER_H
