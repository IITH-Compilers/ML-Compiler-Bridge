//=== SerDes/bitstreamSerDes.h - Bitstream Serialization/Deserialization ---*-
// C++ -*-===//
//
// Part of the MLCompilerBridge Project
//
//===------------------===//

#ifndef BITSTREAM_SERIALIZER_H
#define BITSTREAM_SERIALIZER_H

#include "SerDes/TensorSpec.h"
#include "SerDes/baseSerDes.h"
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>


namespace MLBridge {
class BitstreamSerDes : public BaseSerDes {
public:
  BitstreamSerDes() : BaseSerDes(Kind::Bitstream) {
    Buffer = "";
    tensorSpecs = std::vector<TensorSpec>();
    rawData = std::vector<const void *>();

#define TEMPORARY_STORAGE_INIT(TYPE)                                           \
  features##TYPE = {};                                                         \
  featuresVector##TYPE = {};
    SUPPORTED_TYPES(TEMPORARY_STORAGE_INIT)
#undef TEMPORARY_STORAGE_INIT
  };
#define SET_FEATURE(TYPE)                                                      \
  void setFeature(const std::string &, const TYPE &) override;                 \
  void setFeature(const std::string &, const std::vector<TYPE> &) override;
  SUPPORTED_TYPES(SET_FEATURE)
#undef SET_FEATURE

  void *getSerializedData() override;

  void cleanDataStructures() override {
    Buffer = "";
    tensorSpecs = std::vector<TensorSpec>();
    rawData = std::vector<const void *>();

#define TEMPORARY_STORAGE_CLEAN(TYPE)                                          \
  for (auto &it : features##TYPE) {                                            \
    delete it.second;                                                          \
  }                                                                            \
  features##TYPE.clear();                                                      \
  features##TYPE = {};                                                         \
  for (auto &it : featuresVector##TYPE) {                                      \
    delete it.second;                                                          \
  }                                                                            \
  featuresVector##TYPE.clear();                                                \
  featuresVector##TYPE = {};
    SUPPORTED_TYPES(TEMPORARY_STORAGE_CLEAN)
#undef TEMPORARY_STORAGE_CLEAN
  }

private:
  void *deserializeUntyped(void *) override;
  std::vector<TensorSpec> tensorSpecs;
  std::vector<const void *> rawData;
  std::string Buffer;

#define TEMPORARY_STORAGE_DEF(TYPE)                                            \
  std::map<std::string, TYPE *> features##TYPE;                                \
  std::map<std::string, std::vector<TYPE> *> featuresVector##TYPE;
  SUPPORTED_TYPES(TEMPORARY_STORAGE_DEF)
#undef TEMPORARY_STORAGE_DEF
};
} // namespace MLBridge

#endif
