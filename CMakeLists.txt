cmake_minimum_required(VERSION 3.10)

project(MLCompilerBridge)

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fPIC")
set (CMAKE_CXX_STANDARD 17)

option(LLVM_MLBRIDGE "MLCompilerBridge install for LLVM" OFF)
option(DEBUG "Enable debug mode" ON)

if(NOT LLVM_MLBRIDGE)
  find_package(LLVM 10.0.0 REQUIRED CONFIG)
  include_directories(${LLVM_INCLUDE_DIRS})
  link_directories(${LLVM_LIBRARY_DIR})
  add_compile_definitions(C_LIBRARY)
endif()

add_subdirectory(MLModelRunner)
add_subdirectory(SerDes)
add_subdirectory(test)

if(LLVM_MLBRIDGE)
  include(AddLLVM)
  include(HandleLLVMOptions)
  include(LLVMDistributionSupport)

  add_llvm_library(LLVMMLBridge
  tools.cpp

  ADDITIONAL_HEADER_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include

  LINK_LIBS
  ModelRunnerLib
  $<TARGET_OBJECTS:SerDesLib>
  tf_xla_runtime
  protobuf::libprotobuf
  )
  target_include_directories(LLVMMLBridge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
  install(TARGETS LLVMMLBridge DESTINATION lib)
  if(DEBUG)
    target_compile_definitions(LLVMMLBridge PRIVATE DEBUG)
  endif()

else()
  llvm_map_components_to_libnames(llvm_libs support core irreader analysis TransformUtils)

  add_library(MLCompilerBridge STATIC tools.cpp)
  target_link_libraries(MLCompilerBridge PUBLIC SerDesLib ModelRunnerLib ONNXModelRunnerLib LLVM-10 ${llvm_libs})

  add_executable(MLCompilerBridgeTest $<TARGET_OBJECTS:MLBridgeCPPTest>)
  target_link_libraries(MLCompilerBridgeTest PUBLIC MLCompilerBridge)
  set_property(TARGET MLCompilerBridge PROPERTY POSITION_INDEPENDENT_CODE 1)

  add_library(MLCompilerBridgeC STATIC $<TARGET_OBJECTS:ModelRunnerCWrapper>)
  target_link_libraries(MLCompilerBridgeC PUBLIC SerDesCLib ModelRunnerCLib ONNXModelRunnerLib LLVM-10 ${llvm_libs})
  target_include_directories(MLCompilerBridgeC PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${LLVM_INCLUDE_DIRS})

  target_compile_features(MLCompilerBridgeC PRIVATE cxx_std_17)
  set_property(TARGET MLCompilerBridgeC PROPERTY POSITION_INDEPENDENT_CODE 1)
  install(TARGETS MLCompilerBridgeC DESTINATION lib)
  install(TARGETS MLCompilerBridge DESTINATION lib)

  if(DEBUG)
  target_compile_definitions(MLCompilerBridge PRIVATE DEBUG)
  target_compile_definitions(MLCompilerBridgeC PRIVATE DEBUG)
  endif()

endif(LLVM_MLBRIDGE)

install(TARGETS MLCompilerBridgeC DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY CompilerInterface DESTINATION include/python/MLCompilerBridge)
