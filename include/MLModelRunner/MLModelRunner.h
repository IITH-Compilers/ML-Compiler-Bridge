//===- MLModelRunner.h ---- ML model runner interface -----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Base MLModelRunner class which exposes APIs to set the features
// to be send to the ML model and get the result back from the model.
//
// How to support a new ModelRunner:
// 1. Create a new class which inherits from MLModelRunner.
// 2. Implement the evaluateUntyped() method.
//===----------------------------------------------------------------------===//
//

#ifndef ML_MODEL_RUNNER_H
#define ML_MODEL_RUNNER_H

#include "SerDes/baseSerDes.h"
#include "SerDes/bitstreamSerDes.h"
#include "SerDes/jsonSerDes.h"

#include <cstdlib>
#include <future>
#include <memory>
#include <string>
#include <type_traits>

#ifndef C_LIBRARY
#include "SerDes/protobufSerDes.h"
#include "SerDes/tensorflowSerDes.h"
#endif
namespace MLBridge {
class MLModelRunner {
public:
  // Disallows copy and assign.
  MLModelRunner(const MLModelRunner &) = delete;
  MLModelRunner &operator=(const MLModelRunner &) = delete;
  virtual ~MLModelRunner() = default;

  template <typename T>
  typename std::enable_if<std::is_fundamental<T>::value, T>::type evaluate() {
    return *reinterpret_cast<T *>(evaluateUntyped());
  }

  template <typename T>
  typename std::enable_if<
      std::is_fundamental<typename std::remove_pointer<T>::type>::value,
      void>::type
  evaluate(T &data, size_t &dataSize) {
    using BaseType = typename std::remove_pointer<T>::type;
    void *res = evaluateUntyped();
    T ret = static_cast<T>(malloc(SerDes->getMessageLength()));
    memcpy(ret, res, SerDes->getMessageLength());
    dataSize = SerDes->getMessageLength() / sizeof(BaseType);
    data = ret;
  }

  enum class Kind : int {
    Unknown,
    Release,
    Development,
    NoOp,
    Pipe,
    gRPC,
    ONNX,
    TFAOT
  };
  Kind getKind() const { return Type; }
  BaseSerDes::Kind getSerDesKind() const { return SerDesType; }

  virtual void requestExit() = 0;
  std::promise<void> *exit_requested;

  template <typename T, typename... Types>
  void populateFeatures(std::pair<std::string, T> &var1,
                        std::pair<std::string, Types> &...var2) {
    SerDes->setFeature(var1.first, var1.second);
    populateFeatures(var2...);
  }

  void populateFeatures() {}

  void setRequest(void *request) { SerDes->setRequest(request); }

  void setResponse(void *response) { SerDes->setResponse(response); }

protected:
  MLModelRunner(Kind Type, BaseSerDes::Kind SerDesType,
                llvm::LLVMContext *Ctx = nullptr)
      : Ctx(Ctx), Type(Type), SerDesType(SerDesType) {
    assert(Type != Kind::Unknown);
    initSerDes();
  }
  MLModelRunner(Kind Type, llvm::LLVMContext *Ctx = nullptr)
      : Ctx(Ctx), Type(Type), SerDesType(BaseSerDes::Kind::Unknown) {
    SerDes = nullptr;
  };

  virtual void *evaluateUntyped() = 0;

  llvm::LLVMContext *Ctx;
  const Kind Type;
  const BaseSerDes::Kind SerDesType;

protected:
  std::unique_ptr<BaseSerDes> SerDes;

private:
  void initSerDes() {
    switch (SerDesType) {
    case BaseSerDes::Kind::Json:
      SerDes = std::make_unique<JsonSerDes>();
      break;
    case BaseSerDes::Kind::Bitstream:
      SerDes = std::make_unique<BitstreamSerDes>();
      break;
#ifndef C_LIBRARY
    case BaseSerDes::Kind::Protobuf:
      SerDes = std::make_unique<ProtobufSerDes>();
      break;
    case BaseSerDes::Kind::Tensorflow:
      SerDes = std::make_unique<TensorflowSerDes>();
      break;
#endif
    case BaseSerDes::Kind::Unknown:
      SerDes = nullptr;
      break;
    }
  }
};
} // namespace MLBridge

#endif // LLVM_MLMODELRUNNER_H
