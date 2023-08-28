#include "bitstreamSerializer.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/raw_ostream.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <optional>
#include <string>
#include <vector>

using namespace llvm;
using namespace std;

void BitstreamSerializer::setFeature(const std::string &name,
                                     const int &value) {
  if(name.substr(0, 5) == "regID") {
    // errs() << name << ": " << value << "\n";
  }
  auto *valuePtr = new int(value);
  featuresint[name] = valuePtr;
  tensorSpecs.push_back(TensorSpec::createSpec<int>(name, {1}));
  rawData.push_back(valuePtr);
}

void BitstreamSerializer::setFeature(const std::string &name,
                                     const float &value) {
  auto *valuePtr = new float(value);
  featuresfloat[name] = valuePtr;
  tensorSpecs.push_back(TensorSpec::createSpec<float>(name, {1}));
  rawData.push_back(valuePtr);
}

void BitstreamSerializer::setFeature(const std::string &name,
                                     const double &value) {
  auto *valuePtr = new double(value);
  featuresdouble[name] = valuePtr;
  tensorSpecs.push_back(TensorSpec::createSpec<double>(name, {1}));
  rawData.push_back(valuePtr);
}

void BitstreamSerializer::setFeature(const std::string &name,
                                     const std::string &value) {
  auto *valuePtr = new std::string(value);
  featuresstring[name] = valuePtr;
  long size = value.length();
  tensorSpecs.push_back(TensorSpec::createSpec<uint8_t>(name, {size}));
  rawData.push_back((void *)valuePtr->c_str());
}

void BitstreamSerializer::setFeature(const std::string &name,
                                     const bool &value) {
  auto *valuePtr = new bool(value);
  featuresbool[name] = valuePtr;
  tensorSpecs.push_back(TensorSpec::createSpec<uint8_t>(name, {1}));
  rawData.push_back(valuePtr);
}

void BitstreamSerializer::setFeature(const std::string &name,
                                     const std::vector<int> &value) {
  auto *valuePtr = new std::vector<int>(value);
  featuresVectorint[name] = valuePtr;
  tensorSpecs.push_back(
      TensorSpec::createSpec<int>(name, {static_cast<long>(valuePtr->size())}));
  rawData.push_back(valuePtr->data());
}

void BitstreamSerializer::setFeature(const std::string &name,
                                     const std::vector<float> &value) {
  auto *valuePtr = new std::vector<float>(value);
  featuresVectorfloat[name] = valuePtr;
  tensorSpecs.push_back(
      TensorSpec::createSpec<float>(name, {static_cast<long>(valuePtr->size())}));
  rawData.push_back(valuePtr->data());
}

void BitstreamSerializer::setFeature(const std::string &name,
                                     const std::vector<double> &value) {
  auto *valuePtr = new std::vector<double>(value);
  featuresVectordouble[name] = valuePtr;
  tensorSpecs.push_back(
      TensorSpec::createSpec<double>(name, {static_cast<long>(valuePtr->size())}));
  rawData.push_back(valuePtr->data());
}

void BitstreamSerializer::setFeature(const std::string &name,
                                     const std::vector<std::string> &value) {
  llvm_unreachable("Currently std::vector<std::string> not supported");
}

void BitstreamSerializer::setFeature(const std::string &name,
                                     const std::vector<bool> &value) {
  llvm_unreachable("Currently std::vector<bool> not supported");
}

void *BitstreamSerializer::getSerializedData() {
  errs() << "In BitstreamSerializer getSerializedData...\n";
  std::unique_ptr<raw_ostream> OS =
      std::make_unique<raw_string_ostream>(Buffer);
  json::OStream J(*OS);
  J.object([&]() {
    J.attributeArray("features", [&]() {
      for (size_t I = 0; I < tensorSpecs.size(); ++I) {
        tensorSpecs[I].toJSON(J);
      }
    });
  });
  J.flush();
  OS->write("\n", 1);
  errs() << "rawData.size(): " << rawData.size() << "\n";
  for (size_t I = 0; I < rawData.size(); ++I) {
    OS->write(reinterpret_cast<const char *>(rawData[I]),
              tensorSpecs[I].getTotalTensorBufferSize());
  }
  OS->write("\n", 1);
  OS->flush();
  auto *out = new std::string(Buffer);
  errs() << "Buffer: " << Buffer << "\n";
  cleanDataStructures();
  return out;
}

void *BitstreamSerializer::deserializeUntyped(void *data) {
  return reinterpret_cast<std::string *>(data)->data();
}