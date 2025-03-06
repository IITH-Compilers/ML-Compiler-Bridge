//=== PTModelRunner.cpp - PTModelRunner Implementation ---*- C++ -*-===//
//
// Part of the MLCompilerBridge Project
//
//===------------------===//

#include "MLModelRunner/PTModelRunner.h"

#include "MLModelRunner/MLModelRunner.h"
#include "SerDes/TensorSpec.h"
// #include "SerDes/baseSerDes.h"
#include "SerDes/pytorchSerDes.h"
#include "llvm/Support/ErrorHandling.h"
#include <torch/torch.h>
#include <torch/csrc/inductor/aoti_runner/model_container_runner_cpu.h> // or model_container_runner_cuda.h for CUDA

#include <memory>
#include <vector>
#include <string>


using TensorVec = std::vector<torch::Tensor>;

namespace MLBridge
{

    PTModelRunner::PTModelRunner(const char* modelPath, llvm::LLVMContext &Ctx)
        : MLModelRunner(MLModelRunner::Kind::PTAOT, BaseSerDes::Kind::Pytorch, &Ctx)
    {
        // this->SerDes = new PytorchSerDes();
	      llvm::errs() << "ModelPathName: " << std::string(modelPath) << "[END]\n";
        c10::InferenceMode mode;
        this->CompiledModel = new torch::inductor::AOTIModelContainerRunnerCpu(std::string(modelPath));
    }



  void *PTModelRunner::evaluateUntyped()
  {
    SerDes->getRequest();

    if (reinterpret_cast<TensorVec*>(this->SerDes->getRequest())->empty())
    {
      llvm::errs() << "Input vector is empty.\n";
      return nullptr;
    }

    try
    {
      TensorVec* outputTensors = static_cast<TensorVec*>(this->SerDes->getResponse());
      // 2 torch::Tensor of size 1
      torch::Tensor state_ins = torch::ones(1);
      torch::Tensor seq_lens = torch::ones(1);
      static_cast<TensorVec*>(this->SerDes->getRequest())->push_back(state_ins);
      static_cast<TensorVec*>(this->SerDes->getRequest())->push_back(seq_lens);
      auto outputs = static_cast<torch::inductor::AOTIModelContainerRunnerCpu*>(this->CompiledModel)->run((*static_cast<TensorVec*>(this->SerDes->getRequest())));

      for (auto i = outputs.begin(); i != outputs.end(); ++i)
        (*(outputTensors)).push_back(*i);
      void *rawData = this->SerDes->deserializeUntyped(outputTensors);
      return rawData;
    }
    catch (const c10::Error &e)
    {
      llvm::errs() << "Error during model evaluation: " << e.what() << "\n";
      return nullptr;
    }
  }

  // template <typename U, typename T, typename... Types>
  // void PTModelRunner::populateFeatures(const std::pair<U, T> &var1,
  //                                      const std::pair<U, Types> &...var2)
  // {
  //   llvm::errs() << "Inside populate of ptmodelrunner\n";
  //   SerDes->setFeature(var1.first, var1.second);
  //   PTModelRunner::populateFeatures(var2...);
  //   llvm::errs() << reinterpret_cast<TensorVec*>(this->SerDes->getRequest())->size() << "[In Runner after pop, len of req]\n";

  // }

} // namespace MLBridge
