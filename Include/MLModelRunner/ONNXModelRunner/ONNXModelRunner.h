//===----------------------------------------------------------------------===//
//
// Part of the ml-llvm-tools Project, under the BSD 4-Clause License.
// See the LICENSE.txt file under ml-llvm-tools directory for license
// information.
//
//===----------------------------------------------------------------------===//

#ifndef ONNX_MODELRUNNER_H
#define ONNX_MODELRUNNER_H

#include <iostream>
#include <map>
#include <string>

#include "MLModelRunner/MLModelRunner.h"
#include "MLModelRunner/ONNXModelRunner/agent.h"
#include "MLModelRunner/ONNXModelRunner/environment.h"

namespace llvm {
class ONNXModelRunner : public MLModelRunner {
public:
  ONNXModelRunner(MLBridge::Environment *env,
                  std::map<std::string, Agent *> agents,
                  LLVMContext *Ctx = nullptr);
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
} // namespace llvm
#endif // ONNX_MODELRUNNER_H