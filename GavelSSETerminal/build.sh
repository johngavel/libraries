#!/bin/bash

# Source helper scripts
if ! source common.sh 2>/dev/null; then
  echo "Error: common.sh not found. Please ensure it's in the same directory." >&2
  exit 1
fi

BUILD=$1
CURRENT_DIR=$2

case "$BUILD" in
    clean)    rm "$CURRENT_DIR/consoleScript.h";;
    pre)      ;;
    post)     ;;
    build)    bash htmlheader.sh $CURRENT_DIR/html/consoleScript.html $CURRENT_DIR/consoleScript.h scriptHTML;;
    *)        log_failed "Invalid Command Argument: $BUILD"; exit 1;;
esac

exit 0
