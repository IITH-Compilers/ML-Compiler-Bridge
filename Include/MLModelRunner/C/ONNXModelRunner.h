#ifndef PIPE_MODEL_RUNNER_WRAPPER_H
#define PIPE_MODEL_RUNNER_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

// Define an opaque pointer type for ONNXModelRunnerWrapper
typedef struct ONNXModelRunnerWrapper ONNXModelRunnerWrapper;

// Function to create an instance of ONNXModelRunnerWrapper
ONNXModelRunnerWrapper *createONNXModelRunner(const char *outBoundName,
                                              const char *inBoundName,
                                              int serDesType);

// Function to call a method on ONNXModelRunnerWrapper
void populateFloatFeatures(ONNXModelRunnerWrapper *obj, const char *name,
                           const float *data, const int size);
void populateIntFeatures(ONNXModelRunnerWrapper *obj, const char *name,
                         const int *data, const int size);
int evaluateIntFeatures(ONNXModelRunnerWrapper *obj);
float evaluateFloatFeatures(ONNXModelRunnerWrapper *obj);

// Function to destroy an instance of ONNXModelRunnerWrapper
void destroyONNXModelRunner(ONNXModelRunnerWrapper *obj);

// void test();

#ifdef __cplusplus
}
#endif

#endif // PIPE_MODEL_RUNNER_WRAPPER_H
