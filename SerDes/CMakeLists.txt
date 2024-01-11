set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)

if(LLVM_MLBRIDGE)
  add_llvm_library(SerDesLib
  TensorSpec.cpp
  jsonSerDes.cpp
  bitstreamSerDes.cpp
  protobufSerDes.cpp
  tensorflowSerDes.cpp
  JSON.cpp
  )
else()
  add_library(SerDesLib OBJECT TensorSpec.cpp jsonSerDes.cpp bitstreamSerDes.cpp protobufSerDes.cpp tensorflowSerDes.cpp JSON.cpp)

  add_library(SerDesCLib OBJECT TensorSpec.cpp jsonSerDes.cpp bitstreamSerDes.cpp tensorflowSerDes.cpp protobufSerDes.cpp JSON.cpp)
  target_include_directories(SerDesCLib PRIVATE ${TENSORFLOW_AOT_PATH}/include)
endif()

target_include_directories(SerDesLib PRIVATE ${TENSORFLOW_AOT_PATH}/include)
