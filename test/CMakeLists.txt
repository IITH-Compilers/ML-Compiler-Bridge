find_package(LLVM 10.0.0 REQUIRED CONFIG)
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})

# if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fPIC")
# endif()

add_library(LLVMMLBridgeTest OBJECT MLBridgeTest.cpp)
file(GLOB MODEL_OBJECTS ${CMAKE_CURRENT_SOURCE_DIR}/tf_models/*.o)

foreach(MODEL_OBJECT ${MODEL_OBJECTS})
  target_link_libraries(LLVMMLBridgeTest PRIVATE ${MODEL_OBJECT})
endforeach()
target_link_libraries(LLVMMLBridgeTest PRIVATE ModelRunnerUtils)
target_include_directories(LLVMMLBridgeTest PRIVATE ${CMAKE_BINARY_DIR}/include ${TENSORFLOW_AOT_PATH}/include)
