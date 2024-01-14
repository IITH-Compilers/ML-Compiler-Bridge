//===----------------------------------------------------------------------===//
//
// Part of the ml-llvm-tools Project, under the BSD 4-Clause License.
// See the LICENSE.txt file under ml-llvm-tools directory for license
// information.
//
// Base Environment class to support ML model inference
//
// How to use?
// 1. Create an environment class inheriting from MLBridge::Environment
// 2. Implement the step() and reset() methods
// 3. Create an ONNXModelRunner object with the environment and the agents
//===----------------------------------------------------------------------===//

#ifndef ONNX_MODELRUNNER_ENVIRONMENT_H
#define ONNX_MODELRUNNER_ENVIRONMENT_H

#include "MLModelRunner/ONNXModelRunner/agent.h"
#include <map>

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
