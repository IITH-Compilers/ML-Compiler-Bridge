//===----------------------------------------------------------------------===//
//
// Part of the MLCompilerBridge Project, under the Apache License v2.0 with LLVM
// Exceptions. See the LICENSE file for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MLModelRunner/ONNXModelRunner/environment.h"
#include "MLModelRunner/ONNXModelRunner/utils.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

using namespace MLBridge;
class MLBridgeTestEnv : public Environment {
  Observation CurrObs;

public:
  MLBridgeTestEnv() { setNextAgent("agent"); };
  Observation &reset() override;
  Observation &step(Action) override;
  Action lastAction;

protected:
  std::vector<float> FeatureVector;
};

Observation &MLBridgeTestEnv::step(Action Action) {
  CurrObs.clear();
  std::copy(FeatureVector.begin(), FeatureVector.end(),
            std::back_inserter(CurrObs));
  lastAction = Action;
  setDone();
  return CurrObs;
}

Observation &MLBridgeTestEnv::reset() {
  std::copy(FeatureVector.begin(), FeatureVector.end(),
            std::back_inserter(CurrObs));
  return CurrObs;
}
