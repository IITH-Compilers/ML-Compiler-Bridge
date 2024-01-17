//=== MLModelRunner/C/ONNXModelRunner.h - C API for ONNXModelRunner - C++ -===//
//
// Part of the MLCompilerBridge Project, under the Apache License v2.0 with LLVM
// Exceptions. See the LICENSE file for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===---------------------------------------------------------------------===//
///
/// \file
/// This file defines the C APIs for ONNXModelRunner.
/// This is a wrapper around the ONNXModelRunner class that provides an
/// interface for the MLCompilerBridge to interact with the ONNX models during
/// inference.
///
/// Usage for single agent:
/// 1. Create an ONNXModelRunner object using createSingleAgentOMR
/// 2. Evaluate the features using singleAgentEvaluate
/// 3. Destroy the instance of ONNXModelRunner using destroyONNXModelRunner
///
/// Usage for multiple agents:
/// 1. Create an Environment object using createEnvironment
/// 2. Set the number of features using env_setNumFeatures
/// 3. Set the step function using env_setStepFunc
/// 4. Set the reset function using env_setResetFunc
/// 5. Set the next agent using env_setNextAgent
/// 6. Create an ONNXModelRunner object using createONNXModelRunner
/// 7. Evaluate the features using evaluate
/// 8. Destroy the instance of ONNXModelRunner using destroyONNXModelRunner
/// 9. Destroy the instance of Environment using destroyEnvironment
///
/// Using Environment:
/// 1. Create an Environment object using createEnvironment
/// 2. Set the number of features using env_setNumFeatures
/// 3. Set the step function using env_setStepFunc
/// 4. Set the reset function using env_setResetFunc
/// 5. Set the next agent using env_setNextAgent
/// 6. Destroy the instance of Environment using destroyEnvironment after
/// calling
///   destroyONNXModelRunner.
///
/// Internally, the ONNXModelRunner will call the step function to get the next
/// action and the reset function to reset the environment. The step function
/// should return a pointer to an array of floats. The reset function should
/// return a pointer to an array of floats.
///
//===---------------------------------------------------------------------===//

#ifndef ONNX_MODEL_RUNNER_WRAPPER_H
#define ONNX_MODEL_RUNNER_WRAPPER_H

typedef struct ONNXModelRunner ONNXModelRunner;
typedef struct Environment Environment;
typedef signed Action;

#ifdef __cplusplus
extern "C" {
#endif

Environment *createEnvironment();
void env_setDone(Environment *env);
void env_resetDone(Environment *env);
bool env_checkDone(Environment *env);
void env_setNumFeatures(Environment *env, int numFeatures);
void env_setStepFunc(Environment *env, float *(*stepFunc)(Action action));
void env_setResetFunc(Environment *env, float *(*resetFunc)());
void env_setNextAgent(Environment *env, char *agentName);

ONNXModelRunner *createONNXModelRunner(Environment *env, int numAgents, ...);
ONNXModelRunner *createSingleAgentOMR(char *agent_path);
void evaluate(ONNXModelRunner *obj);
int singleAgentEvaluate(ONNXModelRunner *obj, float *inp, int inp_size);
void destroyEnvironment(Environment *env);
void destroyONNXModelRunner(ONNXModelRunner *obj);

#ifdef __cplusplus
}
#endif

#endif // ONNX_MODEL_RUNNER_WRAPPER_H
