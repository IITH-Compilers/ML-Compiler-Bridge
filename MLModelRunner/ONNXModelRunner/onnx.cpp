//===- onnx.cpp - ONNX Interface with CPP Runtime --------------*- C++ -*-===//
//
// Part of the MLCompilerBridge Project, under the Apache License v2.0 with LLVM
// Exceptions. See the LICENSE file for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file defines the ONNXModel class, which is a wrapper around the ONNX
/// C++ interface.
///
//===----------------------------------------------------------------------===//

#include "MLModelRunner/ONNXModelRunner/onnx.h"
#include "onnxruntime_cxx_api.h"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iostream>
#include <numeric>

ONNXModel::ONNXModel(const char *model_path) : model_path(model_path) {
  env = new Ort::Env(ORT_LOGGING_LEVEL_WARNING, "test");
  session = new Ort::Session(*env, model_path, Ort::SessionOptions{nullptr});
}

Ort::Value ONNXModel::getInputValue(llvm::SmallVector<float, 100> &input,
                                    int inputIdx) {
  auto typeInfo = session->GetInputTypeInfo(inputIdx);
  auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();
  auto inputDims = tensorInfo.GetShape();
  std::replace_if(
      inputDims.begin(), inputDims.end(), [](int64_t &i) { return i < 0; }, 1);

  size_t inputTensorSize = std::accumulate(inputDims.begin(), inputDims.end(),
                                           1, std::multiplies<int>());
  assert(inputTensorSize == input.size());

  auto memory_info =
      Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
  auto inputTmp = Ort::Value::CreateTensor<float>(
      memory_info, input.data(), inputTensorSize, inputDims.data(),
      inputDims.size());
  auto inputTensor = &inputTmp;
  assert(inputTensor->IsTensor());
  return inputTmp;
}

void ONNXModel::run(llvm::SmallVector<float, 100> &input,
                    llvm::SmallVector<float, 100> &output) {
  Ort::AllocatorWithDefaultOptions allocator;

  int input_count = session->GetInputCount();
  llvm::SmallVector<llvm::SmallVector<float, 100>, 10> inputList;
  inputList.push_back(input);
  llvm::SmallVector<float, 100> dummy_input;
  dummy_input.insert(dummy_input.end(), 1);
  for (int i = 1; i < input_count; i++) {
    inputList.push_back(dummy_input);
  }

  llvm::SmallVector<std::string, 10> inputNameList;
  for (int i = 0; i < input_count; i++) {
    auto inputName = session->GetInputNameAllocated(i, allocator);
    auto inputNameStr = inputName.get();
    inputNameList.push_back(inputNameStr);
  }

  std::vector<Ort::Value> input_final;
  llvm::SmallVector<const char *, 10> inputNameStr_final;

  for (int i = 0; i < input_count; i++) {
    input_final.insert(input_final.end(), getInputValue(inputList[i], i));
    inputNameStr_final.insert(inputNameStr_final.end(),
                              inputNameList[i].c_str());
  }

  auto outputName = session->GetOutputNameAllocated(0, allocator);
  auto outputNameStr = outputName.get();

  auto outputTensors =
      session->Run(Ort::RunOptions{nullptr}, inputNameStr_final.data(),
                   input_final.data(), input_count, &outputNameStr, 1);

  assert(outputTensors.size() == 1 && outputTensors.front().IsTensor());

  auto outputDims =
      outputTensors.front().GetTensorTypeAndShapeInfo().GetShape()[1];

  auto outVal = outputTensors.front().GetTensorMutableData<float>();

  output = llvm::SmallVector<float, 100>(outVal, outVal + outputDims);
  std::replace_if(
      output.begin(), output.end(), [](double x) { return std::isnan(x); },
      -1.17549e+038);
}
