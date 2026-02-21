#!/bin/sh
set -e

if [ -z "$1" ]; then
  echo "Uso: $0 <path/to/dir> [args...]"
  exit 1
fi

TARGET_DIR="$1"

SOURCE_FILE=$(find "$TARGET_DIR" -maxdepth 1 -type f -name '*.c' | head -n 1)

if [ -z "$SOURCE_FILE" ]; then
  echo "Could not find a file .c in $TARGET_DIR"
  exit 1
fi

BINARY_NAME=$(basename "$SOURCE_FILE" .c)

BINARY_PATH="/tmp/build-your-own-x/$BINARY_NAME"

mkdir -p /tmp/build-your-own-x

# clang -Wall -Wextra -g -fsanitize=address -fcolor-diagnostics "$SOURCE_FILE" -o "$BINARY_PATH"
gcc "$SOURCE_FILE" -o "$BINARY_PATH"

echo "Executing $BINARY_PATH"
shift
exec "$BINARY_PATH" "$@"
