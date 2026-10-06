#!/usr/bin/env bash

set -euo pipefail

# Verify formatting without checking generated protobuf sources.
find app common -type f \( -iname '*.h' -o -iname '*.c' -o -iname '*.cc' \) \
  ! -iname '*.pb.h' ! -iname '*.pb.c' ! -iname '*.pb.cc' \
  -print0 | xargs -0 -r clang-format --dry-run -Werror
