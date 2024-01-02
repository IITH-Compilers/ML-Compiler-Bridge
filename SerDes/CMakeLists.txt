# add_llvm_library(LLVMSerDes
# protobufSerDes.cpp
# TensorSpec.cpp
# jsonSerDes.cpp
# protobufSerDes.cpp
# bitstreamSerDes.cpp
# )

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/include)


# add_library(SerDesLib OBJECT protobufSerDes.cpp TensorSpec.cpp jsonSerDes.cpp protobufSerDes.cpp bitstreamSerDes.cpp)
# set_property(TARGET SerDesLib PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(SerDesLib OBJECT TensorSpec.cpp jsonSerDes.cpp bitstreamSerDes.cpp protobufSerDes.cpp tensorflowSerDes.cpp)
# set_property(TARGET SerDesLib PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(SerDesCLib OBJECT TensorSpec.cpp jsonSerDes.cpp bitstreamSerDes.cpp)
# target_link_libraries(SerDesLib PUBLIC tf_xla_runtime)
# target_link_libraries(SerDesCLib PUBLIC tf_xla_runtime)
target_include_directories(SerDesLib PUBLIC ${TENSORFLOW_AOT_PATH}/include)
target_include_directories(SerDesCLib PUBLIC ${TENSORFLOW_AOT_PATH}/include)