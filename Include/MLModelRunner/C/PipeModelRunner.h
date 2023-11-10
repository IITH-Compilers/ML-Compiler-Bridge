#ifndef PIPE_MODEL_RUNNER_WRAPPER_H
#define PIPE_MODEL_RUNNER_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

// Define an opaque pointer type for PipeModelRunnerWrapper
typedef struct PipeModelRunnerWrapper PipeModelRunnerWrapper;

// Function to create an instance of PipeModelRunnerWrapper
PipeModelRunnerWrapper *createPipeModelRunner(const char *outBoundName,
                                              const char *inBoundName,
                                              int serDesType);

// Function to call a method on PipeModelRunnerWrapper
void populateFloatFeatures(PipeModelRunnerWrapper *obj, const char *name,
                           const float *data, const int size);
void populateIntFeatures(PipeModelRunnerWrapper *obj, const char *name,
                         const int *data, const int size);
int evaluateIntFeatures(PipeModelRunnerWrapper *obj);
float evaluateFloatFeatures(PipeModelRunnerWrapper *obj);

// Function to destroy an instance of PipeModelRunnerWrapper
void destroyPipeModelRunner(PipeModelRunnerWrapper *obj);

// void test();

#ifdef __cplusplus
}
#endif

#endif // PIPE_MODEL_RUNNER_WRAPPER_H
