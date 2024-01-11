cmake_minimum_required(VERSION 3.10)

project(MLCompilerBridge)

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)

set (CMAKE_CXX_STANDARD 17)
message("LLVM_MLBRIDGE: ${LLVM_MLBRIDGE}")

if(NOT LLVM_MLBRIDGE)
  find_package(LLVM 10.0.0 REQUIRED CONFIG)
  include_directories(${LLVM_INCLUDE_DIRS}) 
  link_directories(${LLVM_LIBRARY_DIR})
  add_compile_definitions(C_LIBRARY)
endif()
message(TENSORFLOW_AOT_PATH: ${TENSORFLOW_AOT_PATH})

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
  endif()
  
add_subdirectory(MLModelRunner)
add_subdirectory(SerDes)

if(LLVM_MLBRIDGE)
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
endif(LLVM_MLBRIDGE)

llvm_map_components_to_libnames(llvm_libs support core irreader analysis TransformUtils)

add_custom_target(protobuf_version ALL 
COMMAND ${CMAKE_COMMAND} -E echo "protoc path = $<TARGET_FILE:protobuf::protoc> Using Protobuf ${Protobuf_VERSION}")

add_library(MLCompilerBridgeC STATIC $<TARGET_OBJECTS:ModelRunnerCWrapper>)
target_link_libraries(MLCompilerBridgeC PUBLIC SerDesCLib ModelRunnerCLib ONNXModelRunnerLib LLVM-10 ${llvm_libs})
target_include_directories(MLCompilerBridgeC PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${LLVM_INCLUDE_DIRS})

target_compile_options(MLCompilerBridgeC PUBLIC "-stdlib=libstdc++")
set_property(TARGET MLCompilerBridgeC PROPERTY POSITION_INDEPENDENT_CODE 1)

install(TARGETS MLCompilerBridgeC DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY CompilerInterface DESTINATION include/python/MLCompilerBridge)
