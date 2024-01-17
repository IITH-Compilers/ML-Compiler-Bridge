//=== SerDes/baseSerDes.h - Base for serialization and deserialization C++ ===//
//
// Part of the MLCompilerBridge Project, under the Apache 2.0 License.
// See the LICENSE file under home directory for license and copyright
// information.
//
//===----------------------------------------------------------------------===//
//
// This is the base class for SerDes. It defines the interface for the
// serialization and deserialization of the data structures used for the
// communication by the MLModelRunner.
//
// Supporting new SerDes:
// 1. Create a new class which inherits from BaseSerDes.
// 2. Implement the setFeature(), getSerializedData(), cleanDataStructures() and
// deserializeUntyped() methods.
// 3. Add the new SerDes to the enum class Kind in this class.
//
//===----------------------------------------------------------------------===//

#ifndef BASE_SERDES_H
#define BASE_SERDES_H

#include "MLModelRunner/Utils/Debug.h"
#include "google/protobuf/extension_set.h"
#include "google/protobuf/message.h"

#include <cassert>
#include <map>
#include <string>
#include <vector>

using namespace std;

#define SUPPORTED_TYPES(M)                                                     \
  M(int)                                                                       \
  M(long)                                                                      \
  M(float)                                                                     \
  M(double)                                                                    \
  M(string)                                                                    \
  M(bool)

namespace MLBridge {
class BaseSerDes {
public:
  // setRepeatedField as pushback
  // setFeature as setFeature, setAttribute
  enum class Kind : int { Unknown, Json, Bitstream, Protobuf, Tensorflow };
  Kind getKind() const { return Type; }

#define SET_FEATURE(TYPE)                                                      \
  virtual void setFeature(const std::string &, const TYPE &) = 0;              \
  virtual void setFeature(const std::string &, const std::vector<TYPE> &){};
  SUPPORTED_TYPES(SET_FEATURE)
#undef SET_FEATURE

  // #ifdef __cplusplus
  virtual void setFeature(const std::string &name,
                          const google::protobuf::Message *value){};
  virtual void
  setFeature(const std::string &name,
             const std::vector<google::protobuf::Message *> &value){};
  // #endif

  // a hack to set the request and response structures in protobuf serializer
  virtual void setRequest(void *Request) {
    MLBRIDGE_DEBUG(std::cout << "In BaseSerializer setRequest...\n");
  };
  virtual void setResponse(void *Response){};
  virtual void *getSerializedData() = 0;
  virtual void *deserializeUntyped(void *data) = 0;
  size_t getMessageLength() { return MessageLength; }
  virtual void *getRequest(){ return nullptr; };
  virtual void *getResponse(){ return nullptr; };

protected:
  BaseSerDes(Kind Type) : Type(Type) { assert(Type != Kind::Unknown); }
  virtual void cleanDataStructures() = 0;
  const Kind Type;
  void *RequestVoid;
  void *ResponseVoid;
  size_t MessageLength;
};
} // namespace MLBridge

#endif
