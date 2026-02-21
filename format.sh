# This script formats all files in the format-checked directories.
find app common -iname '*.h' -o -iname '*.c' -o -iname '*.cc' | xargs clang-format -i