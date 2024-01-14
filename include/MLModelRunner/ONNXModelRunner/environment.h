//===----------------------------------------------------------------------===//
//
// Part of the ml-llvm-tools Project, under the BSD 4-Clause License.
// See the LICENSE.txt file under ml-llvm-tools directory for license
// information.
//
//===----------------------------------------------------------------------===//

#ifndef ONNX_MODELRUNNER_ENVIRONMENT_H
#define ONNX_MODELRUNNER_ENVIRONMENT_H

#include "MLModelRunner/ONNXModelRunner/agent.h"

typedef signed Action;

namespace MLBridge {
class Environment {
  bool done = false;
  std::string nextAgent = "";

protected:
  std::map<std::string, Observation &> obsMap;

public:
  bool checkDone() { return done == true; };
  void setDone() { done = true; }
  void resetDone() { done = false; }
  std::string getNextAgent() { return nextAgent; };
  void setNextAgent(std::string name) { nextAgent = name; }
  virtual Observation &step(Action action) = 0;
  virtual Observation &reset() = 0;
};
} // namespace MLBridge
#endif // ONNX_MODELRUNNER_ENVIRONMENT_H
