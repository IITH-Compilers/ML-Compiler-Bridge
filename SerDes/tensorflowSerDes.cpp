//=== MLCompilerBridge/SerDes/tensorflowSerDes.cpp - Tensorflow SerDes Implementation ---*- C++ -*-===//
//
// Part of the MLCompilerBridge Project
//
//===------------------===//

#include "SerDes/tensorflowSerDes.h"
#include "SerDes/baseSerDes.h"

// #define EXCEPT_LONG(M) M(int) M(float) M(double) M(std::string) M(bool)
namespace MLBridge {
#define SET_FEATURE(TYPE)                                                      \
  void TensorflowSerDes::setFeature(const std::string &Name,                   \
                                    const TYPE &Value) {                       \
    std::string prefix = "feed_";                                              \
    const int Index = CompiledModel->LookupArgIndex(prefix + Name);            \
    if (Index >= 0)                                                            \
      *reinterpret_cast<TYPE *>(CompiledModel->arg_data(Index)) = Value;       \
  }
SUPPORTED_TYPES(SET_FEATURE)
#undef SET_FEATURE

void TensorflowSerDes::setFeature(const std::string &Name,
                                  const std::vector<int64_t> &Value) {
  std::string prefix = "feed_";
  const int Index = CompiledModel->LookupArgIndex(prefix + Name);
  std::copy(Value.begin(), Value.end(),
            static_cast<int64_t *>(CompiledModel->arg_data(Index)));
}

void TensorflowSerDes::setFeature(const std::string &Name,
                                  const std::vector<float> &Value) {
  std::string prefix = "feed_";
  const int Index = CompiledModel->LookupArgIndex(prefix + Name);
  std::copy(Value.begin(), Value.end(),
            static_cast<float *>(CompiledModel->arg_data(Index)));
}

void TensorflowSerDes::setFeature(const std::string &Name,
                                  const std::vector<double> &Value) {
  std::string prefix = "feed_";
  const int Index = CompiledModel->LookupArgIndex(prefix + Name);
  std::copy(Value.begin(), Value.end(),
            static_cast<double *>(CompiledModel->arg_data(Index)));
}

void TensorflowSerDes::setFeature(const std::string &Name,
                                  const std::vector<std::string> &Value) {
  std::string prefix = "feed_";
  const int Index = CompiledModel->LookupArgIndex(prefix + Name);
  std::copy(Value.begin(), Value.end(),
            static_cast<std::string *>(CompiledModel->arg_data(Index)));
}

void TensorflowSerDes::setFeature(const std::string &Name,
                                  const std::vector<bool> &Value) {
  std::string prefix = "feed_";
  const int Index = CompiledModel->LookupArgIndex(prefix + Name);
  std::copy(Value.begin(), Value.end(),
            static_cast<bool *>(CompiledModel->arg_data(Index)));
}

void TensorflowSerDes::setFeature(const std::string &Name,
                                  const std::vector<int> &Value) {
  std::string prefix = "feed_";
  const int Index = CompiledModel->LookupArgIndex(prefix + Name);
  std::copy(Value.begin(), Value.end(),
            static_cast<int *>(CompiledModel->arg_data(Index)));
}
} // namespace MLBridge
