#!/usr/bin/env bash

set -euo pipefail

# Format all C and C++ sources in the checked directories.
find app common -type f \( -iname '*.h' -o -iname '*.c' -o -iname '*.cc' \) \
  -print0 | xargs -0 -r clang-format -i
