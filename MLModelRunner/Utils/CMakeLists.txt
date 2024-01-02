# add_llvm_component_library(LLVMMLModelRunnerUtils TrainingLogger.cpp)

add_library(ModelRunnerCUtils OBJECT JSON.cpp MLConfig.cpp)

if(LLVM_INSTALL)
add_llvm_component_library(ModelRunnerUtils
    JSON.cpp
    MLConfig.cpp
)

target_include_directories(ModelRunnerUtils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
endif(LLVM_INSTALL)