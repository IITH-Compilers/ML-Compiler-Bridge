//===----------------------------------------------------------------------===//
//
// Part of the MLCompilerBridge Project, under the Apache 2.0 License.
// See the LICENSE file under home directory for license and copyright
// information.
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

protected:
  std::vector<float> FeatureVector;
};

Observation &MLBridgeTestEnv::step(Action Action) {
  CurrObs.clear();
  std::copy(FeatureVector.begin(), FeatureVector.end(),
            std::back_inserter(CurrObs));
  llvm::outs() << "Action: " << Action << "\n";
  setDone();
  return CurrObs;
}

Observation &MLBridgeTestEnv::reset() {
  std::copy(FeatureVector.begin(), FeatureVector.end(),
            std::back_inserter(CurrObs));
  return CurrObs;
}
