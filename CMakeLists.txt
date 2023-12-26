cmake_minimum_required(VERSION 3.10)

project(ML-Compiler-Bridge)

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
find_package(LLVM 10.0.0 REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS}) 
include_directories(${Protobuf_INCLUDE_DIRS} ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/Include) 

set (CMAKE_CXX_STANDARD 17)

# add_custom_target(copy-ml-llvm-bridge-include ALL
# COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Include ${CMAKE_BINARY_DIR}/Include
# DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Include
# )

add_subdirectory(MLModelRunner)
add_subdirectory(SerDes)

# add_library(LLVMMLBridge
# tools.cpp

# LINK_LIBS
# LLVMMLModelRunner
# LLVMSerDes
# protobuf::libprotobuf

# DEPENDS
# copy-ml-llvm-bridge-include 
# )

# add_library(MLCompilerBridge STATIC $<TARGET_OBJECTS:ModelRunnerLib> $<TARGET_OBJECTS:SerDesLib>)
# add_dependencies(MLCompilerBridge copy-ml-llvm-bridge-include)
# target_link_libraries(MLCompilerBridge PUBLIC protobuf::libprotobuf)  
# set_property(TARGET MLCompilerBridge PROPERTY POSITION_INDEPENDENT_CODE 1)


# add_library(MLCompilerBridgeC SHARED $<TARGET_OBJECTS:ModelRunnerCLib>)

# llvm_map_components_to_libnames(llvm_libs all)
llvm_map_components_to_libnames(llvm_libs support core irreader analysis TransformUtils)

message("LLVM Includes: ${LLVM_INCLUDE_DIRS}")
message("LLVM Libs: ${llvm_libs}")

add_library(MLCompilerBridgeC STATIC $<TARGET_OBJECTS:ModelRunnerCWrapper>)
link_directories("/usr/lib/llvm-10/lib")
link_directories("${ONNXRUNTIME_ROOTDIR}/lib")


find_package(protobuf CONFIG REQUIRED)
target_link_libraries(MLCompilerBridgeC PUBLIC  SerDesCLib ModelRunnerCLib ModelRunnerUtils LLVM-10 ${llvm_libs})

message("******************** ${CMAKE_CURRENT_SOURCE_DIR}/Include ${CMAKE_BINARY_DIR}")

target_include_directories(MLCompilerBridgeC PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
add_dependencies(MLCompilerBridgeC gRPCModelRunnerLib)
set_property(TARGET MLCompilerBridgeC PROPERTY POSITION_INDEPENDENT_CODE 1)

message("protobuf Libs: ${Protobuf_LIBRARIES}")
message("protobuf Include: ${Protobuf_INCLUDE_DIRS}")
# message("protobuf includes: ${Protobuf}")

# add_library(MLCompilerBridgeC SHARED $<TARGET_OBJECTS:ModelRunnerCLib>)
# add_dependencies(MLCompilerBridgeC copy-ml-llvm-bridge-include)
# set_property(TARGET MLCompilerBridgeC PROPERTY POSITION_INDEPENDENT_CODE 1)
