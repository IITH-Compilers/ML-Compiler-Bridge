# ML-Compiler-Bridge

`ML-Compiler-Bridge` is a compiler agnostic library to aid in ML-Enabled Compiler Optimizations. ML-Compiler-Bridge supports both training and inference scenarios. Library exposes Python and C/C++ APIs to interface
with the Python-based ML models and a C/C++ compiler. This design allows ML model development within a traditional Python framework while making end-to-end integration with an optimizing compiler possible and efficient.

This repo contains the source code and relevant information described in our paper, ["The Next 700 ML-Enabled Compiler Optimizations"](https://arxiv.org/abs/2311.10800).
Please see [here](https://iith-compilers.github.io/ML-Compiler-Bridge/) for documentation and other details.

> The Next 700 ML-Enabled Compiler Optimizations, S. VenkataKeerthy, Siddharth Jain, Umesh Kalvakuntla, Pranav Sai Gorantla, Rajiv Shailesh Chitale, Eugene Brevdo, Albert Cohen, Mircea Trofin and Ramakrishna Upadrasta. CC 2024.

[![Build and Tests](https://github.com/IITH-Compilers/MLCompilerBridge/actions/workflows/build.yml/badge.svg)](https://github.com/IITH-Compilers/MLCompilerBridge/actions/workflows/build.yml)
[![Doxygen Action](https://github.com/IITH-Compilers/MLCompilerBridge/actions/workflows/main.yml/badge.svg)](https://github.com/IITH-Compilers/MLCompilerBridge/actions/workflows/main.yml)
[![pre-commit checks](https://github.com/IITH-Compilers/MLCompilerBridge/actions/workflows/formatting.yml/badge.svg)](https://github.com/IITH-Compilers/MLCompilerBridge/actions/workflows/formatting.yml)


![Image](https://github.com/IITH-Compilers/ML-Compiler-Bridge/raw/main/images/component-ml-compiler-bridge.png)

## Features
* **Unified Framework:** Comes with a suite of two inter-process and two in-process model runners and three serialization-deserialization mechanisms to support interleaved and non-interleaved communication between models and compiler.
* **Multi-language Support:** Exposes C++ and C APIs to interface model runners and serializers with the compilers and Python APIs to interface inter-process model runners with ML models.
* **Compiler and ML-Framework Independence:** Provides compiler and ML-Framework independent APIs, and supports easier integration with compilers like LLVM, MLIR, and Pluto and ML Frameworks like TensorFlow, PyTorch, JAX, etc.
* **Deeper Integration:** Enables deeper integration of ML models within the compiler in a framework-independent manner to support easier inference in case of ML driven compiler optimizations.

## Requirements
* cmake (>= 3.10)
* GNU Make (4.2.1)
* LLVM (10.X) - [src](https://github.com/llvm/llvm-project/tree/release/10.x), [release](https://releases.llvm.org/download.html#10.0.1)
* Python (3.10), C++17
* gRPC v1.34 and protobuf v3.13 - for gRPC Model Runner
    * Building GRPC from Source: Please follow [`Build GRPC with cmake`](https://grpc.io/docs/languages/cpp/quickstart/) v1.34 (protobuf v3.13) to build GRPC from source.
    * In the above tutorial setting `DCMAKE_INSTALL_PREFIX` may not be necessary and the default install prefix can be used.
    * The following dependencies will be required for Python: `pip install grpcio-tools`.
* [ONNXRuntime](https://github.com/microsoft/onnxruntime/releases) v1.13.1
* TensorFlow - for TF Model Runner (AOT flow)
    * Tested with TensorFlow 2.13.0
* Other python requirements are available in [mlbridge.yml](./mlbridge.yml)
    * Conda/Anaconda based virtual environment is assumed

(Experiments are done on an Ubuntu 20.04 machine)

## Setup
`ML-Compiler-Bridge` can be built as a stand-alone library to generate `.a` files that can in turn be linked with any compiler.
1. `mkdir build && cd build`
2. `cmake [-DCMAKE_BUILD_TYPE=Release|Debug] [-DCMAKE_INSTALL_PREFIX=<Install_path>] -DONNXRUNTIME_ROOTDIR=<Path to ONNX install dir> -DTENSORFLOW_AOT_PATH=<Path to TensorFlow pip install dir> ../`
3. `make -j [&& make install]`

This process would generate `libMLCompilerBridge.a` and `libMLCompilerBridgeC.a` libraries under `build/lib` directory, required headers under `build/include` directory. `libMLCompilerBridgeC.a` exposes C APIs for using with C-based compilers like Pluto, where as `libMLCompilerBridge.a` exposes C++ APIs that can be used with any compiler written in C++.

Python end points are available under [`CompilerInterface`](./CompilerInterface/).

To ensure the correctness, run `make verify-all`

### Using ML-Compiler-Bridge with LLVM and MLIR

`ML-Compiler-Bridge` can be integrated and built along with the LLVM project. This can be done by adding this repository as a new project and setting `LLVM_MLBRIDGE` option to `ON`.

You can check the [CMakeLists.txt](https://github.com/IITH-Compilers/ml-llvm-project/blob/mlbridge-lib/llvm/CMakeLists.txt) of the [`ml-llvm-project`](https://github.com/IITH-Compilers/ml-llvm-project/tree/mlbridge-lib) repository which demonstrates such an integration.

The passes that need to make use of this library can then just link with `LLVMMLBridge`.

Example `CMakeLists.txt` of an LLVM pass that would use the library is shown below.

```CMakeLists.txt
add_llvm_component_library(LLVMMLPass
pass.cpp
ml.cpp

ADDITIONAL_HEADER_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/includes

DEPENDS
  LLVMMLBridge
  intrinsics_gen
)
target_link_libraries(LLVMMLPass PRIVATE LLVMMLBridge)

```
To use TensorFlow AOT Model Runner, you need to make use of `tf_find_and_compile` method exposed in [`cmake/modules/TensorFlowCompile.cmake`](cmake/modules/TensorFlowCompile.cmake) in the CMakeLists.txt of your pass with appropriate arguments. An example of integrating TF AOT Model with inlining pass is shown [here](https://github.com/IITH-Compilers/ml-llvm-project/blob/tfmodel/llvm/lib/Analysis/CMakeLists.txt).

## Examples
TBD

## Artifacts
Libraries are autogenerated for every relevant check-in with GitHub actions. Such generated artifacts are tagged along with the successful runs of [`Publish`]() action.

## Citation
```
@misc{venkatakeerthy-2023-MLCompilerBridge,
      title={The Next 700 ML-Enabled Compiler Optimizations},
      author={S. VenkataKeerthy and Siddharth Jain and Umesh Kalvakuntla and Pranav Sai Gorantla and Rajiv Shailesh Chitale and Eugene Brevdo and Albert Cohen and Mircea Trofin and Ramakrishna Upadrasta},
      year={2023},
      eprint={2311.10800},
      archivePrefix={arXiv},
      primaryClass={cs.PL}
}
```

## Contributions
Please feel free to raise issues to file a bug, pose a question, or initiate any related discussions. Pull requests are welcome :)

## License
ML-Compiler-Bridge is released under Apache 2.0 license with LLVM Exceptions. See LICENSE file for more details.
