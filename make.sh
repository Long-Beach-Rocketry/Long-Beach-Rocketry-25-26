#!/bin/bash

# Function to display usage
usage() {
  echo "Usage: $0 -t <target_name> [-a <app_name>] [-r] [-c]"
  exit 1
}

# Initialize variables
TARGET=""
APP=""
MODE="Debug"
CLEAN=0

# Parse command-line arguments
while getopts ":t:a:r:c:" opt; do
  case ${opt} in
    ( t )
      TARGET="$OPTARG"
      ;;
    ( a )
      APP="$OPTARG"
      ;;
    ( \? )
      echo "Invalid option: -$OPTARG" 1>&2
      usage
      ;;
    ( : )
      if [[ "$OPTARG" == "c" ]]; then
        CLEAN=1
      elif [[ "$OPTARG" == "r" ]]; then
        MODE="Release"
      else
        echo "Option -$OPTARG requires an argument." 1>&2
        usage
      fi
      ;;
  esac
done

# Check if the target name is provided
if [ -z "$TARGET" ]; then
  usage
fi

if [ "$CLEAN" == 1 ]; then
    echo "Performing clean build..."
    rm -rf build/$TARGET
fi

cmake --preset=$TARGET -DTARGET_APP=$APP -DCMAKE_BUILD_TYPE=$MODE
pushd build/$TARGET
cmake --build .
popd