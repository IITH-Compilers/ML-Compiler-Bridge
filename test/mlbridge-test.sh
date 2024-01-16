#!/usr/bin/bash
BLUE='\033[0;34m'
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color
BOLD='\033[1m'

REPO_DIR=$HOME/ml-llvm-project/MLCompilerBridge
BUILD_DIR=$REPO_DIR/build_release
SERVER_FILE=$REPO_DIR/test/mlbridge-test.py

STATUS=0
SERVER_PID=0

run_test() {
    sleep 1
    $@
    if [ $? != 0 ]; then
        STATUS=1
        echo -e "$(tput bold)${RED}[Test Failed] Error detected by compiler side.${NC}"
        kill $SERVER_PID
        SERVER_PID=0
    fi  

    if [ $SERVER_PID != 0 ]; then
        wait $SERVER_PID
        if [ $? != 0 ]; then
            STATUS=1
            echo -e "$(tput bold)${RED}[Test Failed] Error detected by model side.${NC}"
        fi  
    fi

    if [ $STATUS == 0 ]; then
        echo -e "${GREEN}${BOLD}[Test Passed] Datatypes transmitted succesfully.${NC}"
    fi

    STATUS=0
    SERVER_PID=0
}

# source deactivate 
# source activate ml_loopdist_env

echo -e "${BLUE}${BOLD}Testing MLBridge [pipe-bytes]${NC}"
python $SERVER_FILE --use_pipe=True --data_format=bytes --pipe_name=mlbridgepipe --silent=True &
SERVER_PID=$!
run_test $BUILD_DIR/MLCompilerBridgeTest --test-config=pipe-bytes --test-pipe-name=mlbridgepipe --silent

echo -e "${BLUE}${BOLD}Testing MLBridge [pipe-json]${NC}"
python $SERVER_FILE --use_pipe=True --data_format=json --pipe_name=mlbridgepipe2 --silent=True  &
SERVER_PID=$!
run_test $BUILD_DIR/MLCompilerBridgeTest --test-config=pipe-json --test-pipe-name=mlbridgepipe2 --silent

# python $SERVER_FILE --use_grpc --server_port=50065 &
# echo "Test [grpc]:"
# run_test $BUILD_DIR/MLCompilerBridgeTest --test-config=grpc --test-server-address="0.0.0.0:50065"

# echo "Test [onnx]:"
# $BUILD_DIR/MLCompilerBridgeTest --test-config=onnx
