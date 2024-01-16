//===- TFModelRunner.h ---- TF precompiled model runner  ------*- C++-*----===//
//
// Part of the MLCompilerBridge Project, under the Apache 2.0 License.
// See the LICENSE file under home directory for license and copyright
// information.
//
// (Preliminary version adopted from ReleaseModeModelRunner.h of LLVM 17.X)
//
//===----------------------------------------------------------------------===//
//
// This file implements a model runner wrapping an AOT compiled ML model.
// Only inference is supported.
//
//===----------------------------------------------------------------------===//

#ifndef TFMODELRUNNER_H
#define TFMODELRUNNER_H

#include "MLModelRunner/MLModelRunner.h"
#include "SerDes/TensorSpec.h"

#include <memory>
#include <vector>

namespace MLBridge {

/// TFModelRunner - TF Compiled model implementation of the
/// MLModelRunner. It uses an AOT-compiled SavedModel for efficient execution.
template <class TGen> class TFModelRunner final : public MLModelRunner {
public:
  /// FeatureNames' type should be an indexed collection of std::string, like
  /// std::array or std::vector, that has a size() method.
  TFModelRunner(llvm::StringRef DecisionName, llvm::LLVMContext &Ctx,
                llvm::StringRef FeedPrefix = "feed_",
                llvm::StringRef FetchPrefix = "fetch_")
      : MLModelRunner(MLModelRunner::Kind::TFAOT, BaseSerDes::Kind::Tensorflow,
                      &Ctx),
        CompiledModel(std::make_unique<TGen>()) {

    SerDes->setRequest(CompiledModel.get());

    assert(CompiledModel && "The CompiledModel should be valid");

    ResultIndex = CompiledModel->LookupResultIndex(FetchPrefix.str() +
                                                   DecisionName.str());
    assert(ResultIndex >= 0 && "Cannot find DecisionName in inlining model");
  }
  TFModelRunner(llvm::StringRef DecisionName,
                llvm::StringRef FeedPrefix = "feed_",
                llvm::StringRef FetchPrefix = "fetch_")
      : MLModelRunner(MLModelRunner::Kind::TFAOT, BaseSerDes::Kind::Tensorflow),
        CompiledModel(std::make_unique<TGen>()) {

    SerDes->setRequest(CompiledModel.get());

    assert(CompiledModel && "The CompiledModel should be valid");

    ResultIndex = CompiledModel->LookupResultIndex(FetchPrefix.str() +
                                                   DecisionName.str());
    assert(ResultIndex >= 0 && "Cannot find DecisionName in inlining model");
  }
  virtual ~TFModelRunner() = default;

  virtual void requestExit() override {
    llvm_unreachable("requestExit() is not supported in TFModelRunner");
  }

  static bool classof(const MLModelRunner *R) {
    return R->getKind() == MLModelRunner::Kind::Release;
  }

private:
  void *evaluateUntyped() override {
    CompiledModel->Run();
    return CompiledModel->result_data(ResultIndex);
  }

  int32_t ResultIndex = -1;
  std::unique_ptr<TGen> CompiledModel;
};

} // namespace MLBridge

#endif // TFMODELRUNNER_H
