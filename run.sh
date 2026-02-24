#!/bin/sh
set -e

GREEN='\033[0;32m'
RED='\033[0;31m'
CYAN='\033[0;36m'
YELLOW='\033[0;33m'
RESET='\033[0m'

COMMAND=$1
SOURCE_FILE="lc3_assembler.c"
BINARY_NAME=$(basename "$SOURCE_FILE" .c)
BINARY_PATH="/tmp/build-your-own-x/$BINARY_NAME"

mkdir -p /tmp/build-your-own-x/

case $COMMAND in
    clang|gcc)
        if [ "$COMMAND" = "clang" ]; then
            FLAGS="-Wall -Wextra -fsanitize=address -fcolor-diagnostics"
        else
            FLAGS="-g"
        fi
        
        if ! $COMMAND $FLAGS "$SOURCE_FILE" -o "$BINARY_PATH"; then
            echo -e "${RED}Error: Compilation failed using $COMMAND${RESET}"
            exit 1
        fi

        shift
        echo -e "${GREEN}[$COMMAND]${RESET} Assembling... ${CYAN}$@${RESET}"
        exec "$BINARY_PATH" "$@"
        ;;

    valgrind)
        shift
        echo -e "${YELLOW}Recompiling for Valgrind (no Sanitize)...${RESET}"
        gcc -g "$SOURCE_FILE" -o "$BINARY_PATH"

        echo -e "${GREEN}[valgrind]${RESET} Analyzing... ${CYAN}$*${RESET}"
        $COMMAND --leak-check=full --show-leak-kinds=all "$BINARY_PATH" "$@"
        ;;

    *)
        echo -e "${YELLOW}Usage:${RESET} $0 ${CYAN}<tool>${RESET} asm/${YELLOW}<file>${RESET}.asm"
        echo -e "  Tools: ${GREEN}gcc, clang, valgrind${RESET}"
        exit 1
        ;;
esac
