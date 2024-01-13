set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

add_library(MLBridgeCPPTest OBJECT MLBridgeTest.cpp)
file(GLOB MODEL_OBJECTS ${CMAKE_CURRENT_SOURCE_DIR}/tf_models/*.o)

foreach(MODEL_OBJECT ${MODEL_OBJECTS})
  target_link_libraries(MLBridgeCPPTest PRIVATE ${MODEL_OBJECT})
endforeach()
target_link_libraries(MLBridgeCPPTest PRIVATE ModelRunnerUtils)
target_include_directories(MLBridgeCPPTest PRIVATE ${CMAKE_BINARY_DIR}/include ${TENSORFLOW_AOT_PATH}/include)
