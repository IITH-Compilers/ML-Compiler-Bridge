#!/bin/bash

REPO_DIR=$HOME/repos/ml-llvm-project/MLCompilerBridge
BUILD=$1

if [[ -z "$BUILD" ]]
then 
  echo "Pls provide build type"
  echo "exiting..."
  exit 1
fi


cmake \
  -G Ninja \
  -S $REPO_DIR \
  -B $REPO_DIR/build_${BUILD,,} \
  -DONNXRUNTIME_ROOTDIR=$HOME/Downloads/onnxruntime-linux-x64-cuda-1.13.1 \
  -DLLVM_INSTALL=OFF \
  -DTENSORFLOW_AOT_PATH=$HOME/anaconda3/envs/mlgo-new/lib/python3.10/site-packages/tensorflow \
  -DCMAKE_BUILD_TYPE="$BUILD" \
  -DCMAKE_INSTALL_PREFIX=$REPO_DIR/install 

