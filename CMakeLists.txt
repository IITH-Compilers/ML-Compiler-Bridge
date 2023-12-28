cmake_minimum_required(VERSION 3.10)

project(MLCompilerBridge)


set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS} ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include) 

set (CMAKE_CXX_STANDARD 17)

add_subdirectory(MLModelRunner)
add_subdirectory(SerDes)
link_directories("/usr/lib/llvm-10/lib")


# add_library(MLCompilerBridge STATIC $<TARGET_OBJECTS:ModelRunnerLib> $<TARGET_OBJECTS:SerDesLib>)
# target_link_libraries(MLCompilerBridge PUBLIC protobuf::libprotobuf)  
# set_property(TARGET MLCompilerBridge PROPERTY POSITION_INDEPENDENT_CODE 1)

option(LLVM_INSTALL "MLCompilerBridge install for LLVM" OFF)
if(LLVM_INSTALL)
  include(AddLLVM)
  include(HandleLLVMOptions)
  include(LLVMDistributionSupport)
  
  add_llvm_library(LLVMMLBridge
  tools.cpp
  
  ADDITIONAL_HEADER_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  
  LINK_LIBS
  $<TARGET_OBJECTS:ModelRunnerLib>
  $<TARGET_OBJECTS:SerDesLib>
  protobuf::libprotobuf
  )
  target_include_directories(LLVMMLBridge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
else()
  find_package(LLVM 10.0.0 REQUIRED CONFIG)
  include_directories(${LLVM_INCLUDE_DIRS}) 
  
endif(LLVM_INSTALL)


# llvm_map_components_to_libnames(llvm_libs all)
llvm_map_components_to_libnames(llvm_libs support core irreader analysis TransformUtils)

message("LLVM Includes: ${LLVM_INCLUDE_DIRS}")
message("LLVM Libs: ${llvm_libs}")

add_library(MLCompilerBridgeC STATIC $<TARGET_OBJECTS:ModelRunnerCWrapper>)
# link_directories("${ONNXRUNTIME_ROOTDIR}/lib")

target_link_libraries(MLCompilerBridgeC PUBLIC SerDesCLib ModelRunnerCLib LLVM-10 ${llvm_libs})
target_include_directories(MLCompilerBridgeC PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_property(TARGET MLCompilerBridgeC PROPERTY POSITION_INDEPENDENT_CODE 1)

message("protobuf Libs: ${Protobuf_LIBRARIES}")
message("protobuf Include: ${Protobuf_INCLUDE_DIRS}")
# message("protobuf includes: ${Protobuf}")

# add_library(MLCompilerBridgeC SHARED $<TARGET_OBJECTS:ModelRunnerCLib>)
# add_dependencies(MLCompilerBridgeC copy-ml-llvm-bridge-include)
# set_property(TARGET MLCompilerBridgeC PROPERTY POSITION_INDEPENDENT_CODE 1)
