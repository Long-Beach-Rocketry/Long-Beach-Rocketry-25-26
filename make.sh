#!/usr/bin/env bash

set -euo pipefail

usage() {
  echo "Usage: $0 -t <target_name> [-a <app_name>] [-r] [-c]"
  exit 1
}

target=""
app=""
mode="Debug"
clean=0

while getopts ":t:a:rc" opt; do
  case "$opt" in
    t)
      target="$OPTARG"
      ;;
    a)
      app="$OPTARG"
      ;;
    r)
      mode="Release"
      ;;
    c)
      clean=1
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      usage
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      usage
      ;;
  esac
done

if [[ -z "$target" ]]; then
  usage
fi

build_dir="build/$target"

if [[ "$clean" -eq 1 ]]; then
  echo "Performing clean build..."
  rm -rf -- "$build_dir"
fi

cmake --preset="$target" -DTARGET_APP="$app" -DCMAKE_BUILD_TYPE="$mode"
cmake --build "$build_dir"
