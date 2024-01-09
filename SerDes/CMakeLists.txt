set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/include)

if(LLVM_INSTALL)
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
  target_include_directories(SerDesCLib PUBLIC ${TENSORFLOW_AOT_PATH}/include)
endif()

target_include_directories(SerDesLib PUBLIC ${TENSORFLOW_AOT_PATH}/include)