#ifndef ONNX_MODEL_RUNNER_WRAPPER_H
#define ONNX_MODEL_RUNNER_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

// Define an opaque pointer type for ONNXModelRunnerWrapper
typedef struct ONNXModelRunnerWrapper ONNXModelRunnerWrapper;
typedef struct Environment Environment;
typedef signed Action;

Environment *createEnvironment();
void env_setDone(Environment *env);
void env_resetDone(Environment *env);
bool env_checkDone(Environment *env);
void env_setNumFeatures(Environment *env, int numFeatures);
void env_setStepFunc(Environment *env, float *(*stepFunc)(Action action));
void env_setResetFunc(Environment *env, float *(*resetFunc)());
void env_setNextAgent(Environment *env, char *agentName);

ONNXModelRunnerWrapper *createONNXModelRunner(Environment *env, int numAgents,
                                              ...);
void evaluate(ONNXModelRunnerWrapper *obj);
void destroyEnvironment(Environment *env);
void destroyONNXModelRunner(ONNXModelRunnerWrapper *obj);

#ifdef __cplusplus
}
#endif

#endif // ONNX_MODEL_RUNNER_WRAPPER_H
