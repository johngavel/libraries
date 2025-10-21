#!/bin/bash

# Source helper scripts
if ! source common.sh 2> /dev/null; then
  echo "Error: common.sh not found. Please ensure it's in the same directory." >&2
  exit 1
fi

BUILD=$1
CURRENT_DIR=$2

case "$BUILD" in
  --clean) rm "$CURRENT_DIR/styleHTML.h" ;;
  --pre) ;;
  --post) ;;
  --build) bash htmlheader.sh $CURRENT_DIR/assets/style.html $CURRENT_DIR/styleHTML.h styleHTML ;;
  *)
    log_failed "Invalid Command Argument: $BUILD"
    exit 1
    ;;
esac

exit 0
