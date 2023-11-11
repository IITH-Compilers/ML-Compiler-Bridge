#include "MLModelRunner/C/ONNXModelRunner.h"

#include "MLModelRunner/MLModelRunner.h"
#include "MLModelRunner/ONNXModelRunner/ONNXModelRunner.h"
#include "MLModelRunner/ONNXModelRunner/agent.h"
#include "MLModelRunner/ONNXModelRunner/environment.h"
#include "MLModelRunner/ONNXModelRunner/utils.h"

#include <map>
#include <stdarg.h>
#include <vector>

using namespace llvm;
struct ONNXModelRunnerWrapper {
  MLModelRunner *model;
};

class EnvironmentImpl : public MLBridge::Environment {
private:
  // Function pointer to the step and reset functions
  float *(*stepFunc)(Action action);
  float *(*resetFunc)();
  int numFeatures;

public:
  EnvironmentImpl() : stepFunc(nullptr), resetFunc(nullptr) {}
  // EnvironmentImpl(float *(*stepFunc)(Action action), float *(*resetFunc)())
  //     : stepFunc(stepFunc), resetFunc(resetFunc) {}

  void setNumFeatures(int numFeatures) { this->numFeatures = numFeatures; }

  void setStepFunc(float *(*stepFunc)(Action action)) {
    this->stepFunc = stepFunc;
  }

  void setResetFunc(float *(*resetFunc)()) { this->resetFunc = resetFunc; }

  Observation step(Action action) override {
    assert(stepFunc != nullptr &&
           "Step function is null! Define step function on env");
    float *stepRes = stepFunc(action);
    return SmallVector<float, 100>(stepRes, stepRes + numFeatures);
  }

  Observation reset() override {
    assert(resetFunc != nullptr &&
           "Reset function is null! Define reset function on env");
    float *resetRes = resetFunc();
    return SmallVector<float, 100>(resetRes, resetRes + numFeatures);
  }
  //   bool checkDone() override { return env.done; }
  //   void setDone() override { env.done = true; }
  //   void resetDone() override { env.done = false; }
};

struct Environment {
  EnvironmentImpl *env;
};

Environment *createEnvironment() {
  Environment *envWrapper = new Environment();
  envWrapper->env = new EnvironmentImpl();
  return envWrapper;
}

void env_setDone(Environment *env) { env->env->setDone(); }

void env_resetDone(Environment *env) { env->env->resetDone(); }

bool env_checkDone(Environment *env) { return env->env->checkDone(); }

void env_setNumFeatures(Environment *env, int numFeatures) {
  env->env->setNumFeatures(numFeatures);
}

void env_setStepFunc(Environment *env, float *(*stepFunc)(Action action)) {
  env->env->setStepFunc(stepFunc);
}

void env_setResetFunc(Environment *env, float *(*resetFunc)()) {
  env->env->setResetFunc(resetFunc);
}

void env_setNextAgent(Environment *env, char *agentName) {
  env->env->setNextAgent(agentName);
}

// Environment *createEnvironment(Observation (*stepFunc)(Action action),
//                                Observation (*resetFunc)()) {
//   EnvironmentImpl *env = new EnvironmentImpl(stepFunc, resetFunc);
//   Environment *envWrapper = new Environment();
//   envWrapper->env = env;
//   envWrapper->done = false;
//   return envWrapper;
// }

ONNXModelRunnerWrapper *createONNXModelRunner(Environment *env, int numAgents,
                                              ...) {
  assert(env->env != nullptr && "Environment is null!");

  va_list args;
  va_start(args, numAgents);
  std::map<std::string, Agent *> agents;

  for (int i = 0; i < numAgents; i += 2) {
    char *agentName = va_arg(args, char *);
    char *agentPath = va_arg(args, char *);
    agents[agentName] = new Agent(agentPath);
  }

  va_end(args);

  ONNXModelRunnerWrapper *obj = new ONNXModelRunnerWrapper();
  obj->model = new llvm::ONNXModelRunner(env->env, agents);
  return obj;
}

void evaluate(ONNXModelRunnerWrapper *obj) { obj->model->evaluate<int>(); }

void destroyEnvironment(Environment *env) {
  delete env->env;
  delete env;
}

void destroyONNXModelRunner(ONNXModelRunnerWrapper *obj) {
  delete obj->model;
  delete obj;
}
