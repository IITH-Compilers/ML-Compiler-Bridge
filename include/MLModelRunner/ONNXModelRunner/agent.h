//=== MLModelRunner/ONNXModelRunner/agent.h - Agent Model Helper - C++ -===//
//
// Part of the MLCompilerBridge Project, under the Apache 2.0 License.
// See the LICENSE file under home directory for license and copyright
// information.
//
//===---------------------------------------------------------------------===//
//
// Agent class to support ML/RL model inference via ONNX
//
// Usage:
// 1. Construct an agent object with the path to the ONNX model
// 2. Call computeAction() to get the action from the model
//
//===----------------------------------------------------------------------===//

#ifndef ONNX_MODELRUNNER_AGENT_H
#define ONNX_MODELRUNNER_AGENT_H

#include "MLModelRunner/ONNXModelRunner/onnx.h"
#include "MLModelRunner/ONNXModelRunner/utils.h"

#include <stdio.h>
#include <string>

namespace MLBridge {
class Agent {
public:
  ONNXModel *model;
  Agent(std::string model_path);
  unsigned computeAction(Observation &obs);
};
} // namespace MLBridge

#endif // ONNX_MODELRUNNER_AGENT_H
