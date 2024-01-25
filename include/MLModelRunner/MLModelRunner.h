//===- MLModelRunner.h ---- ML model runner interface -----------*- C++ -*-===//
//
// Part of the MLCompilerBridge Project, under the Apache License v2.0 with LLVM
// Exceptions. See the LICENSE file for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (Preliminary version adopted from MLModelRunner.h of LLVM 17.X)
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The MLModelRunner class is the main interface for interacting with the
/// ML models. The MLCompilerBridge uses the MLModelRunner class to set the
/// features to be sent to the model and get the result back from the model.
///
/// This class internally uses the SerDes class to serialize and deserialize the
/// features and result.
///
/// The MLModelRunner class is an abstract class and cannot be instantiated.
///
/// This class internally uses the SerDes class to serialize and deserialize the
/// features and result.
///
/// Supporting new Model Runners:
/// 1. Create a new class inheriting the MLModelRunner class.
/// 2. Override evaluateUntyped() method to call the model and get the result.
///
/// Using any of the existing Model Runners:
/// 1. Instantiate the model runner object with the appropriate arguments.
/// 2. Call populateFeatures() to set the features to be sent to the model.
/// 3. Call evaluate() to get the send and receive the result from the model.
/// Similar flows apply for both training and inference.
///
//===----------------------------------------------------------------------===//

#ifndef ML_MODEL_RUNNER_H
#define ML_MODEL_RUNNER_H

#include "SerDes/baseSerDes.h"
#include "SerDes/bitstreamSerDes.h"
#include "SerDes/jsonSerDes.h"

#include <cstdlib>
#include <fstream>
#include <future>
#include <memory>
#include <string>
#include <type_traits>

#ifndef C_LIBRARY
#include "SerDes/protobufSerDes.h"
#include "SerDes/tensorflowSerDes.h"
#endif
namespace MLBridge {

/// MLModelRunner - The main interface for interacting with the ML models.
class MLModelRunner {
public:
  std::ofstream of;
  // Disallows copy and assign.
  MLModelRunner(const MLModelRunner &) = delete;
  MLModelRunner &operator=(const MLModelRunner &) = delete;
  virtual ~MLModelRunner() = default;

  /// Main user-facing method for interacting with the ML models
  template <typename T>
  typename std::enable_if<std::is_fundamental<T>::value, T>::type evaluate() {
    return *reinterpret_cast<T *>(evaluateUntyped());
  }

  /// Main user-facing method for interacting with the ML models
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

  /// Type of the MLModelRunner
  enum class Kind : int { Unknown, Pipe, gRPC, ONNX, TFAOT };

  Kind getKind() const { return Type; }
  BaseSerDes::Kind getSerDesKind() const { return SerDesType; }

  virtual void requestExit() = 0;
  std::promise<void> *exit_requested;

  template <typename T, typename... Types>
  void passMetaInfo(std::pair<std::string, T> &var1,
                    std::pair<std::string, Types> &...var2) {
    of.open("observation.txt", std::ios::app);
    of << var1.first << ": " << var1.second << "\n";
    of.close();
    passMetaInfo(var2...);
  }

  void passMetaInfo() {}

  template <typename T> void dumpFeature(std::pair<std::string, T> &var1) {
    of.open("observation.txt", std::ios::app);
    of << var1.first << ": " << var1.second << "\n";
    of.close();

  }

  template <typename T>
  void dumpFeature(std::pair<std::string, std::vector<T>> &var1) {
    of.open("observation.txt", std::ios::app);
    of << var1.first << ": ";
    for (const auto &elem : var1.second) {
      of << elem << " ";
    }
    of << "\n";
    of.close();

  }
  /// User-facing interface for setting the features to be sent to the model.
  /// The features are passed as a list of key-value pairs.
  /// The key is the name of the feature and the value is the value of the
  /// feature. The value can be a scalar or a vector.
  template <typename T, typename... Types>
  void populateFeatures(std::pair<std::string, T> &var1,
                        std::pair<std::string, Types> &...var2) {
    SerDes->setFeature(var1.first, var1.second);
    dumpFeature(var1);
    // of.open("observation.txt",std::ios::app);
    // of << var1.first << ": " << reinterpret_cast<T>(var1.second) << "\n";
    // of.close();
    populateFeatures(var2...);
  }

  void populateFeatures() {}

  /// Mainly used in the case of gRPC where the request object is
  /// not known explicitly.
  void setRequest(void *request) { SerDes->setRequest(request); }

  /// Mainly used in the case of gRPC where the response object is
  /// not known explicitly.
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

  /// Should be implemented by the derived class to call the model and get the
  /// result.
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
