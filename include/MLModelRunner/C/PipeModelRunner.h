//===--- MLModelRunner/C/PipeModelRunner.h - C API for PipeModelRunner ---===//
//
// Part of the MLCompilerBridge Project, under the Apache License v2.0 with LLVM
// Exceptions. See the LICENSE file for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===---------------------------------------------------------------------===//
///
/// \file
/// This file defines the C API for PipeModelRunner.
/// PipeModelRunner is a wrapper around the MLModelRunner class that provides
/// an interface for the MLCompilerBridge to interact with the PipeModelRunner
/// class.
///
/// Usage:
/// 1. Create an instance of PipeModelRunnerWrapper using createPipeModelRunner
/// 2. Populate the features using populateXXXFeatures functions
/// 3. Evaluate the features using evaluateXXXFeatures functions
/// 4. Destroy the instance of PipeModelRunnerWrapper using
/// destroyPipeModelRunner
///
//===---------------------------------------------------------------------===//

#ifndef PIPE_MODEL_RUNNER_WRAPPER_H
#define PIPE_MODEL_RUNNER_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

// Define an opaque pointer type for PipeModelRunnerWrapper
typedef struct PipeModelRunnerWrapper PipeModelRunnerWrapper;

/// Creates an instance of PipeModelRunnerWrapper
PipeModelRunnerWrapper *createPipeModelRunner(const char *outBoundName,
                                              const char *inBoundName,
                                              int serDesType);

/// Populates the features of PipeModelRunnerWrapper
void populateFloatFeatures(PipeModelRunnerWrapper *obj, const char *name,
                           const float *data, const int size);
void populateIntFeatures(PipeModelRunnerWrapper *obj, const char *name,
                         const int *data, const int size);

/// Evaluates the features of PipeModelRunnerWrapper
int evaluateIntFeatures(PipeModelRunnerWrapper *obj);
float evaluateFloatFeatures(PipeModelRunnerWrapper *obj);

/// Destroys an instance of PipeModelRunnerWrapper
void destroyPipeModelRunner(PipeModelRunnerWrapper *obj);

#ifdef __cplusplus
}
#endif

#endif // PIPE_MODEL_RUNNER_WRAPPER_H
