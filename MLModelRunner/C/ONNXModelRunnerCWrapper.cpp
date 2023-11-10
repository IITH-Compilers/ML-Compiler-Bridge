// #include "MLModelRunner/C/ONNXModelRunner.h"

// #include "MLModelRunner/MLModelRunner.h"
// #include "MLModelRunner/ONNXModelRunner/ONNXModelRunner.h"

// #include <vector>
// #include <map>
// #include <stdarg.h>


// struct ONNXModelRunnerWrapper {
//   MLModelRunner *model;
// };

// ONNXModelRunnerWrapper *createONNXModelRunner(int numAgents, ...) {
//   va_list args;
//   va_start(args, numAgents);
//   std::map<std::string, Agent *> agents;

//   for (int i = 0; i < numAgents; i+=2) {
//     char *agentName = va_arg(args, char *);
//     char *agentPath = va_arg(args, char *);
//     agents[agentName] = &agent(agentPath);
//   }

//   va_end(args);

//   ONNXModelRunnerWrapper *obj = new ONNXModelRunnerWrapper();
//   obj->model = new llvm::ONNXModelRunner(OutboundName, InboundName,
//                                          (BaseSerDes::Kind)SerDesType);
//   return obj;
// }

// void populateFloatFeatures(ONNXModelRunnerWrapper *obj, const char *name,
//                            const float *data, const int size) {
//   auto dataVec = std::vector<float>(data, data + size);
//   std::pair<std::string, std::vector<float>> p1(name, dataVec);
//   obj->model->populateFeatures(p1);
//   errs() << "populateFloatFeatures: " << name << " " << size << "\n";
// }

// void populateIntFeatures(ONNXModelRunnerWrapper *obj, const char *name,
//                          const int *data, const int size) {
//   auto dataVec = std::vector<int>(data, data + size);
//   std::pair<std::string, std::vector<int>> p1(name, dataVec);
//   obj->model->populateFeatures(p1);
// }

// int evaluateIntFeatures(ONNXModelRunnerWrapper *obj) {
//   return obj->model->evaluate<int>();
// }

// float evaluateFloatFeatures(ONNXModelRunnerWrapper *obj) {
//   return obj->model->evaluate<float>();
// }

// void destroyONNXModelRunner(ONNXModelRunnerWrapper *obj) {
//   delete obj->model;
//   delete obj;
// }
