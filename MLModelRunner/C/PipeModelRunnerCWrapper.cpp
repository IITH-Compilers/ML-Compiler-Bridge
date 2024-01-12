#include "MLModelRunner/C/PipeModelRunner.h"

#include "MLModelRunner/MLModelRunner.h"
#include "MLModelRunner/PipeModelRunner.h"

#include <vector>

using namespace MLBridge;
struct PipeModelRunnerWrapper {
  MLModelRunner *model;
};

PipeModelRunnerWrapper *createPipeModelRunner(const char *OutboundName,
                                              const char *InboundName,
                                              int SerDesType) {
  PipeModelRunnerWrapper *obj = new PipeModelRunnerWrapper();
  obj->model = new PipeModelRunner(OutboundName, InboundName,
                                         (BaseSerDes::Kind)SerDesType);
  return obj;
}

void populateFloatFeatures(PipeModelRunnerWrapper *obj, const char *name,
                           const float *data, const int size) {
  auto dataVec = std::vector<float>(data, data + size);
  std::pair<std::string, std::vector<float>> p1(name, dataVec);
  obj->model->populateFeatures(p1);
}

void populateIntFeatures(PipeModelRunnerWrapper *obj, const char *name,
                         const int *data, const int size) {
  auto dataVec = std::vector<int>(data, data + size);
  std::pair<std::string, std::vector<int>> p1(name, dataVec);
  obj->model->populateFeatures(p1);
}

int evaluateIntFeatures(PipeModelRunnerWrapper *obj) {
  return obj->model->evaluate<int>();
}

float evaluateFloatFeatures(PipeModelRunnerWrapper *obj) {
  return obj->model->evaluate<float>();
}

void destroyPipeModelRunner(PipeModelRunnerWrapper *obj) {
  delete obj->model;
  delete obj;
}
