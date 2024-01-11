if(LLVM_MLBRIDGE)
  add_llvm_component_library(ModelRunnerUtils
  MLConfig.cpp
  )
  target_include_directories(ModelRunnerUtils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
endif(LLVM_MLBRIDGE)

add_library(ModelRunnerCUtils OBJECT MLConfig.cpp)
