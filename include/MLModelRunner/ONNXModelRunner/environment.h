//=== MLModelRunner/ONNXModelRunner/environment.h - ONNX Environment C++ -===//
//
// Part of the MLCompilerBridge Project, under the Apache License v2.0 with LLVM
// Exceptions. See the LICENSE file for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===---------------------------------------------------------------------===//
///
/// \file
/// Base Environment class to support ONNX based inference of RL models. This
/// class is used to define the environment for the agents to interact with.
///
/// The Environment should be defined by the compiler pass that is using the
/// MLCompilerBridge. The environment should be defined by inheriting from this
/// class and implementing the step() and reset() methods.
///
/// step() and reset() are typical methods used in RL environments.
///
/// The step() method takes an action as input and returns the observation
/// corresponding to the next state. The reset() method returns the initial
/// observation.
///
/// Usage:
/// 1. Create an environment class inheriting from MLBridge::Environment
/// 2. Implement step() and reset() methods
///
/// Example:
/// \code
/// class MyEnvironment : public MLBridge::Environment {
/// public:
///   Observation &step(Action action) override {
///     // Implement the step function here
///   }
///   Observation &reset() override {
///     // Implement the reset function here
///   }
/// };
/// \endcode
///
/// This environment can then be used by the ONNXModelRunner to interact with
/// the agents. getNextAgent() and setNextAgent() methods can be used to set the
/// next agent to interact with. These methods are used in step() and reset() to
/// get the next agent to interact with in case of multi-agent environment.
///
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
  /// CheckDone returns true if the termination condition is met at the end of
  /// the episode.
  bool checkDone() { return done == true; };

  /// SetDone sets the termination condition to true.
  void setDone() { done = true; }
  void resetDone() { done = false; }

  /// GetNextAgent returns the name/ID of the next agent to interact with.
  std::string getNextAgent() { return nextAgent; };

  /// SetNextAgent sets the name of the next agent to interact with.
  void setNextAgent(std::string name) { nextAgent = name; }

  /// Step function takes an action as input and returns the observation
  /// corresponding to the next state. This method should be implemented by the
  /// user. Typically this method should call setDone() if the termination
  /// condition is met. setNextAgent() can be called in this method to set the
  /// next agent to interact with.
  virtual Observation &step(Action action) = 0;

  /// Reset function returns the initial observation. This method should be
  /// implemented by the user. This method can internally call setNextAgent() to
  /// set the next agent to interact with.
  virtual Observation &reset() = 0;
};
} // namespace MLBridge
#endif // ONNX_MODELRUNNER_ENVIRONMENT_H
