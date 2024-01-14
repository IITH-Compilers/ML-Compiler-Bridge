//===----------------------------------------------------------------------===//
//
// Part of the ml-llvm-tools Project, under the BSD 4-Clause License.
// See the LICENSE.txt file under ml-llvm-tools directory for license
// information.
//
// Agent class to support ML model inference
//
// How to use?
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

#define DEBUG_TYPE "rl-inference-engine"

namespace MLBridge {
class Agent {
public:
  ONNXModel *model;
  Agent(std::string model_path);
  unsigned computeAction(Observation &obs);
};
} // namespace MLBridge

#endif // ONNX_MODELRUNNER_AGENT_H
