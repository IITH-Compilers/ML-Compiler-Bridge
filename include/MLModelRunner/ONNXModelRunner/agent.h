//=== MLModelRunner/ONNXModelRunner/agent.h - Agent Model Helper - C++ -===//
//
// Part of the MLCompilerBridge Project, under the Apache License v2.0 with LLVM
// Exceptions. See the LICENSE file for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===---------------------------------------------------------------------===//
///
/// \file
/// Agent class to support ML/RL model inference via ONNX
///
/// Usage:
/// 1. Construct an agent object with the path to the ONNX model
/// 2. Call computeAction() to get the action from the model
///
//===----------------------------------------------------------------------===//

#ifndef ONNX_MODELRUNNER_AGENT_H
#define ONNX_MODELRUNNER_AGENT_H

#include "MLModelRunner/ONNXModelRunner/onnx.h"
#include "MLModelRunner/ONNXModelRunner/utils.h"

#include <stdio.h>
#include <string>

namespace MLBridge {
/// Agent is a wrapper around the ONNXModel class, interfaces with the
/// Environment class to support ML/RL model inference via ONNXModel.
class Agent {
  ONNXModel *model;

public:
  Agent(std::string model_path);
  /// Runs the ONNX model on the input Observation and returns the output
  unsigned computeAction(Observation &obs);
};
} // namespace MLBridge

#endif // ONNX_MODELRUNNER_AGENT_H
