#include "protobufSerializer.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdint>
#include "google/protobuf/descriptor.h"
#include <type_traits>


inline void ProtobufSerializer::setFeature(std::string name, int &value) {
  Request->GetReflection()->SetInt32(
      Request, Request->GetDescriptor()->FindFieldByName(name), value);
}

inline void ProtobufSerializer::setFeature(std::string name, float &value) {
  Request->GetReflection()->SetFloat(
      Request, Request->GetDescriptor()->FindFieldByName(name), value);
}

inline void ProtobufSerializer::setFeature(std::string name, double &value) {
  Request->GetReflection()->SetDouble(
      Request, Request->GetDescriptor()->FindFieldByName(name), value);
}

inline void ProtobufSerializer::setFeature(std::string name, std::string &value) {
  Request->GetReflection()->SetString(
      Request, Request->GetDescriptor()->FindFieldByName(name), value);
}

inline void ProtobufSerializer::setFeature(std::string name, bool &value) {
  Request->GetReflection()->SetBool(
      Request, Request->GetDescriptor()->FindFieldByName(name), value);
}

inline void ProtobufSerializer::setFeature(std::string name, std::vector<int> &value) {
  auto ref = Request->GetReflection()->MutableRepeatedField<int>(
      Request, Request->GetDescriptor()->FindFieldByName(name));
  ref->Add(value.begin(), value.end());
}

inline void ProtobufSerializer::setFeature(std::string name, std::vector<float> &value) {
  auto ref = Request->GetReflection()->MutableRepeatedField<float>(
      Request, Request->GetDescriptor()->FindFieldByName(name));
  ref->Add(value.begin(), value.end());
}

inline void ProtobufSerializer::setFeature(std::string name, std::vector<double> &value) {
  auto ref = Request->GetReflection()->MutableRepeatedField<double>(
      Request, Request->GetDescriptor()->FindFieldByName(name));
  ref->Add(value.begin(), value.end());
}

void ProtobufSerializer::setFeature(std::string name,
                                    std::vector<std::string> &value) {
  auto reflection = Request->GetReflection();
  auto descriptor = Request->GetDescriptor();
  auto field = descriptor->FindFieldByName(name); 
  for (auto &v : value) {
    reflection->AddString(Request, field, v);
  }
}

inline void ProtobufSerializer::setFeature(std::string name, std::vector<bool> &value) {
  auto ref = Request->GetReflection()->MutableRepeatedField<bool>(
      Request, Request->GetDescriptor()->FindFieldByName(name));
  ref->Add(value.begin(), value.end());
}

std::string ProtobufSerializer::getSerializedData() {
  std::string data;
  Request->SerializeToString(&data);
  cleanDataStructures();
  return data;
}

inline void ProtobufSerializer::setRequest(void *Request) {
  this->Request = reinterpret_cast<Message*>(Request);
}

inline void ProtobufSerializer::setResponse(void *Response) {
  this->Response = reinterpret_cast<Message*>(Response);
}

void *ProtobufSerializer::deserializeUntyped(std::string data) {
  Response->ParseFromString(data);
  llvm::errs() << Response->DebugString();

  const Descriptor *descriptor = Response->GetDescriptor();
  const Reflection *reflection = Response->GetReflection();
  const FieldDescriptor *field = descriptor->field(0);

  if (field->label() == FieldDescriptor::LABEL_REPEATED) {
    if (field->type() == FieldDescriptor::Type::TYPE_INT32) {
      auto ref = reflection->GetRepeatedField<int32_t>(*Response, field);
      std::vector<int32_t> *ptr = new std::vector<int32_t>(
          ref.mutable_data(), ref.mutable_data() + ref.size());
      return ptr;
    }
    if (field->type() == FieldDescriptor::Type::TYPE_FLOAT) {
      auto ref = reflection->GetRepeatedField<float>(*Response, field);
      std::vector<float> *ptr = new std::vector<float>(
          ref.mutable_data(), ref.mutable_data() + ref.size());
      return ptr;
    }
    if (field->type() == FieldDescriptor::Type::TYPE_DOUBLE) {
      auto ref = reflection->GetRepeatedField<double>(*Response, field);
      std::vector<double> *ptr = new std::vector<double>(
          ref.mutable_data(), ref.mutable_data() + ref.size());
      return ptr;
    }
    if (field->type() == FieldDescriptor::Type::TYPE_STRING) {
      // yet to be tested
      llvm_unreachable("vector<string> deserialization yet to be done");
      std::vector<std::string> *ptr = new std::vector<std::string>();
      auto ref = reflection->GetRepeatedField<std::string>(*Response, field);
      for (auto &v : ref) {
        ptr->push_back(v);
      }
      return ptr;
    }
    if (field->type() == FieldDescriptor::Type::TYPE_BOOL) {
      auto ref = reflection->GetRepeatedField<bool>(*Response, field);
      std::vector<bool> *ptr = new std::vector<bool>(
          ref.mutable_data(), ref.mutable_data() + ref.size());
      return ptr;
    }
  }

  if (field->type() == FieldDescriptor::Type::TYPE_INT32) {
    int32_t value = reflection->GetInt32(*Response, field);
    int32_t *ptr = new int32_t(value);
    return ptr;
  }
  if (field->type() == FieldDescriptor::Type::TYPE_FLOAT) {
    float value = reflection->GetFloat(*Response, field);
    float *ptr = new float(value);
    return ptr;
  }
  if (field->type() == FieldDescriptor::Type::TYPE_DOUBLE) {
    double value = reflection->GetDouble(*Response, field);
    double *ptr = new double(value);
    return ptr;
  }
  if (field->type() == FieldDescriptor::Type::TYPE_STRING) {
    std::string value = reflection->GetString(*Response, field);
    std::string *ptr = new std::string(value);
    return ptr;
  }
  if (field->type() == FieldDescriptor::Type::TYPE_BOOL) {
    bool value = reflection->GetBool(*Response, field);
    bool *ptr = new bool(value);
    return ptr;
  }

  llvm_unreachable("Unknown type in protobuf serializer");
}

void ProtobufSerializer::cleanDataStructures() {
  Request->Clear();
  Response->Clear();
}