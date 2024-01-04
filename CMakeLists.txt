cmake_minimum_required(VERSION 3.10)

project(MLCompilerBridge)


set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)

set (CMAKE_CXX_STANDARD 17)
 
if(NOT LLVM_INSTALL)
  find_package(LLVM 10.0.0 REQUIRED CONFIG)
  include_directories(${LLVM_INCLUDE_DIRS}) 
  message("FROM MAIN: ${LLVM_INCLUDE_DIRS}")
  message("FROM MAIN: ${LLVM_LIBRARY_DIR}")
  link_directories(${LLVM_LIBRARY_DIR} "${ONNXRUNTIME_ROOTDIR}/lib")
  add_compile_definitions(C_LIBRARY)
endif()
message(TENSORFLOW_AOT_PATH: ${TENSORFLOW_AOT_PATH})

# add_library(MLCompilerBridge STATIC $<TARGET_OBJECTS:ModelRunnerLib> $<TARGET_OBJECTS:SerDesLib>)
# target_link_libraries(MLCompilerBridge PUBLIC protobuf::libprotobuf)  
# set_property(TARGET MLCompilerBridge PROPERTY POSITION_INDEPENDENT_CODE 1)

if (DEFINED LLVM_HAVE_TF_AOT OR LLVM_HAVE_TFLITE)
  include(cmake/modules/TensorFlowCompile.cmake)
  set(LLVM_INLINER_MODEL_PATH_DEFAULT "models/inliner-Oz")

  set(LLVM_INLINER_MODEL_CURRENT_URL "<UNSPECIFIED>" CACHE STRING "URL to download the LLVM inliner model")
  
  if (DEFINED LLVM_HAVE_TF_AOT)
    
    tf_find_and_compile(
      ${LLVM_INLINER_MODEL_PATH}
      ${LLVM_INLINER_MODEL_CURRENT_URL}
      ${LLVM_INLINER_MODEL_PATH_DEFAULT}
      "../../llvm/lib/Analysis/models/gen-inline-oz-test-model.py"
      serve
      action
      InlinerSizeModel
      llvm::InlinerSizeModel
      ${LLVM_INCLUDE_DIR}/MLModelRunner/TFModel
      )
    endif()
    
    # if (LLVM_HAVE_TFLITE)
    #   list(APPEND MLLinkDeps
    #     tensorflow-lite::tensorflow-lite)
    # endif()
  endif()
  
add_subdirectory(MLModelRunner)
add_subdirectory(SerDes)

option(LLVM_INSTALL "MLCompilerBridge install for LLVM" OFF)
if(LLVM_INSTALL)
  include(AddLLVM)
  include(HandleLLVMOptions)
  include(LLVMDistributionSupport)
  
  add_llvm_library(LLVMMLBridge
  tools.cpp
  ${GeneratedMLSources}
  
  ADDITIONAL_HEADER_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${TENSORFLOW_AOT_PATH}/include
  
  LINK_LIBS
  ModelRunnerLib
  $<TARGET_OBJECTS:SerDesLib>
  protobuf::libprotobuf
  tf_xla_runtime
  )
  target_include_directories(LLVMMLBridge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${TENSORFLOW_AOT_PATH}/include)
endif(LLVM_INSTALL)

# target_include_directories(LLVMMLBridge PUBLIC ${TENSORFLOW_AOT_PATH}/include)

# llvm_map_components_to_libnames(llvm_libs all)
llvm_map_components_to_libnames(llvm_libs support core irreader analysis TransformUtils)

# message("LLVM Includes: ${LLVM_INCLUDE_DIRS}")
message("LLVM Libs: ${llvm_libs}")
# message("${TENSORFLOW_AOT_PATH}/include ----------------------------")

add_custom_target(protobuf_version ALL 
COMMAND ${CMAKE_COMMAND} -E echo "protoc path = $<TARGET_FILE:protobuf::protoc> Using Protobuf ${Protobuf_VERSION}")

add_library(MLCompilerBridgeC STATIC $<TARGET_OBJECTS:ModelRunnerCWrapper>)
# link_directories("${ONNXRUNTIME_ROOTDIR}/lib")

target_link_libraries(MLCompilerBridgeC PUBLIC SerDesCLib ModelRunnerCLib ONNXModelRunnerLib LLVM-10 ${llvm_libs})
target_include_directories(MLCompilerBridgeC PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${LLVM_INCLUDE_DIRS})
# print out the include directories
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

target_compile_options(MLCompilerBridgeC PUBLIC "-stdlib=libstdc++")
set_property(TARGET MLCompilerBridgeC PROPERTY POSITION_INDEPENDENT_CODE 1)

install(TARGETS MLCompilerBridgeC DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY CompilerInterface DESTINATION include/python/MLCompilerBridge)
message("protobuf Libs: ${Protobuf_LIBRARIES}")
message("protobuf Include: ${Protobuf_INCLUDE_DIRS}")
# message("protobuf includes: ${Protobuf}")

# add_library(MLCompilerBridgeC SHARED $<TARGET_OBJECTS:ModelRunnerCLib>)
# add_dependencies(MLCompilerBridgeC copy-ml-llvm-bridge-include)
# set_property(TARGET MLCompilerBridgeC PROPERTY POSITION_INDEPENDENT_CODE 1)
