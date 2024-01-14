//=== SerDes/jsonSerDes.h - Json Serialization/Deserialization ---*- C++
//
// Part of the MLCompilerBridge Project
//
//===------------------===//

#ifndef JSON_SERIALIZER_H
#define JSON_SERIALIZER_H

#include "MLModelRunner/Utils/JSON.h"
#include "SerDes/baseSerDes.h"
#include <string>
#include <utility>

namespace MLBridge {
class JsonSerDes : public BaseSerDes {
public:
  JsonSerDes() : BaseSerDes(BaseSerDes::Kind::Json){};

  static bool classof(const BaseSerDes *S) {
    return S->getKind() == BaseSerDes::Kind::Json;
  }

#define SET_FEATURE(TYPE)                                                      \
  void setFeature(const std::string &name, const TYPE &value) override {       \
    J[name] = value;                                                           \
  }                                                                            \
  void setFeature(const std::string &name, const std::vector<TYPE> &value)     \
      override {                                                               \
    J[name] = llvm::json::Array(value);                                              \
  }
  SUPPORTED_TYPES(SET_FEATURE)
#undef SET_FEATURE

  void *getSerializedData() override;

  void cleanDataStructures() override { J = llvm::json::Object(); }

private:
  void *deserializeUntyped(void *data) override;
  void *desJson(llvm::json::Value *V);

private:
  llvm::json::Object J;
};
} // namespace MLBridge

#endif
