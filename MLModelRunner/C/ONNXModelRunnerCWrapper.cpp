//=== MLModelRunner/C/ONNXModelRunner.cpp - C API for ONNXModelRunner -----===//
//
// Part of the MLCompilerBridge Project, under the Apache License v2.0 with LLVM
// Exceptions. See the LICENSE file for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file defines the C APIs for ONNXModelRunner.
///
//===----------------------------------------------------------------------===//

#include "MLModelRunner/C/ONNXModelRunner.h"
#include "MLModelRunner/ONNXModelRunner/agent.h"
#include "MLModelRunner/ONNXModelRunner/utils.h"
#include "MLModelRunner/Utils/Debug.h"
#include "llvm/ADT/SmallVector.h"
#include <cassert>
#include <iostream>
#include <map>
#include <stdarg.h>
#include <vector>

using namespace MLBridge;
struct ONNXModelRunner {
  Environment *env;
  Agent *agent;
  std::map<std::string, Agent *> agents;
  ONNXModelRunner(Environment *env, std::map<std::string, Agent *> &&agents)
      : env(env), agents(agents) {}

  ONNXModelRunner(Agent *agent) : agent(agent) {}
};

struct Environment {
private:
  // Function pointer to the step and reset functions
  float *(*stepFunc)(Action action);
  float *(*resetFunc)();
  int numFeatures;
  std::string nextAgent;
  bool done;

public:
  Environment() : stepFunc(nullptr), resetFunc(nullptr) {}
  // EnvironmentImpl(float *(*stepFunc)(Action action), float *(*resetFunc)())
  //     : stepFunc(stepFunc), resetFunc(resetFunc) {}

  void setNumFeatures(int numFeatures) { this->numFeatures = numFeatures; }

  void setStepFunc(float *(*stepFunc)(Action action)) {
    this->stepFunc = stepFunc;
  }

  void setResetFunc(float *(*resetFunc)()) { this->resetFunc = resetFunc; }

  void setNextAgent(char *agentName) { nextAgent = agentName; }

  std::string getNextAgent() { return nextAgent; }

  Observation step(Action action) {
    assert(stepFunc != nullptr &&
           "Step function is null! Define step function on env");
    float *stepRes = stepFunc(action);
    return llvm::SmallVector<float, 100>(stepRes, stepRes + numFeatures);
  }

  Observation reset() {
    assert(resetFunc != nullptr &&
           "Reset function is null! Define reset function on env");
    float *resetRes = resetFunc();
    return llvm::SmallVector<float, 100>(resetRes, resetRes + numFeatures);
  }

  bool checkDone() { return done; }
  void setDone() { done = true; }
  void resetDone() { done = false; }
};

Environment *createEnvironment() { return new Environment(); }

void env_setDone(Environment *env) { env->setDone(); }

void env_resetDone(Environment *env) { env->resetDone(); }

bool env_checkDone(Environment *env) { return env->checkDone(); }

void env_setNumFeatures(Environment *env, int numFeatures) {
  env->setNumFeatures(numFeatures);
}

void env_setStepFunc(Environment *env, float *(*stepFunc)(Action action)) {
  env->setStepFunc(stepFunc);
}

void env_setResetFunc(Environment *env, float *(*resetFunc)()) {
  env->setResetFunc(resetFunc);
}

void env_setNextAgent(Environment *env, char *agentName) {
  env->setNextAgent(agentName);
}

ONNXModelRunner *createONNXModelRunner(Environment *env, int numAgents, ...) {
  assert(env != nullptr && "Environment is null!");

  va_list args;
  va_start(args, numAgents);
  std::map<std::string, Agent *> agents;

  for (int i = 0; i < numAgents; i += 2) {
    char *agentName = va_arg(args, char *);
    char *agentPath = va_arg(args, char *);
    agents[agentName] = new Agent(agentPath);
  }

  va_end(args);

  ONNXModelRunner *obj = new ONNXModelRunner(env, std::move(agents));
  return obj;
}

ONNXModelRunner *createSingleAgentOMR(char *agent_path) {
  Agent *agent = new Agent(agent_path);
  ONNXModelRunner *obj = new ONNXModelRunner(agent);
  return obj;
}

void evaluate(ONNXModelRunner *omr) {
  auto x = omr->env->reset();

  while (true) {
    Action action;
    // current agent
    // auto current_agent = omr->agents[omr->env->getNextAgent()];
    Agent *current_agent = omr->agent;
    action = current_agent->computeAction(x);
    MLBRIDGE_DEBUG(std::cout << "Action: " << action << "\n");
    x = omr->env->step(action);
    if (omr->env->checkDone()) {
      MLBRIDGE_DEBUG(std::cout << "Done🎉\n");
      break;
    }
  }
}

int singleAgentEvaluate(ONNXModelRunner *obj, float *inp, int inp_size) {
  Observation obs(inp, inp + inp_size);
  Action action = obj->agent->computeAction(obs);
  MLBRIDGE_DEBUG(std::cout << "action :: " << action << "\n");
  return action;
}

void destroyEnvironment(Environment *env) { delete env; }

void destroyONNXModelRunner(ONNXModelRunner *omr) { delete omr; }
