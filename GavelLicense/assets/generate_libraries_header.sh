#!/bin/bash

# Enable strict mode
set -euo pipefail
IFS=$'\n\t'
shopt -s nullglob

# Source helper scripts or define fallback
if ! source common.sh 2> /dev/null; then
  log_info() { echo "$@"; }
fi

if ! source libraries_helper.sh 2> /dev/null; then
  log_info() { echo "$@"; }
fi

ACTION="${1:-"--build"}"
START_DIR="${2:-.}"
OUTPUT_DIR="${3:-.}"
OUTPUT_FILE="$OUTPUT_DIR"/libraries.h
TMP_FILE=$(mktemp)
TMP_HEADER=$(mktemp)

build_library_database $TMP_FILE

# Remove old temp files on exit
trap 'rm -f "$TMP_FILE" "$TMP_HEADER"' EXIT

if [[ $ACTION == "--clean" ]]; then
  while IFS=',' read -r dir name version license full_license header_file; do
    Delete "$OUTPUT_DIR"/"$header_file"
  done < "$TMP_FILE"
  Delete "$OUTPUT_FILE"
  exit 0
fi

if [[ $ACTION == "--build" ]]; then
  while IFS=',' read -r dir name version license full_license header_file; do
    createfileheader.sh "$HOME_DIR"/libraries/"$dir"/"$license" "$OUTPUT_DIR"/"$header_file" "$full_license" "$full_license"
  done < "$TMP_FILE"

  # Generate header into TMP_HEADER
  {
    echo "/* Auto-generated library definitions */"
    echo "#ifndef LIBRARIES_H"
    echo "#define LIBRARIES_H"
    echo
    echo '#if __has_include("library_used.h")'
    echo '  #include "library_used.h"'
    echo "#else"
    while IFS=',' read -r dir name version license full_license header_file; do
      HEADER_MACRO=$(get_library_used "$name")
      echo "#define ${HEADER_MACRO}"
    done < "$TMP_FILE"
    echo "#endif"
    echo
    while IFS=',' read -r dir name version license full_license header_file; do
      HEADER_MACRO="${name^^}"
      HEADER_MACRO="${HEADER_MACRO// /_}" # Replace spaces with underscores
      HEADER_MACRO="${HEADER_MACRO%%.*}"
      echo "#ifdef ${HEADER_MACRO}_USED"
      echo "#include \"$header_file\""
      echo "#endif"
    done < "$TMP_FILE"
    echo
    echo "typedef struct {"
    echo "  const char* name;"
    echo "  const char* version;"
    echo "  const char* license_file;"
    echo "  const unsigned char* license_buffer;"
    echo "  const unsigned int license_size;"
    echo "  const char* license_name;"
    echo "} LibraryInfo;"
    echo
    echo '#define NULL_LIBRARY_ENTRY {"null", "0.0", "null", nullptr, 0, '"}"
    echo
    echo "static const LibraryInfo libraries[] = {"

    while IFS=',' read -r dir name version license full_license header_file; do
      HEADER_MACRO="$full_license"
      HEADER_MACRO="${HEADER_MACRO%%.*}"
      HEADER_MACRO=$(echo "$HEADER_MACRO" | tr -cd '[:alpha:]')
      HEADER_MACRO="${HEADER_MACRO^^}_H"
      echo "#ifdef $HEADER_MACRO"
      variable_name="${full_license%%.*}"
      variable_name=$(echo "$variable_name" | tr -cd '[:alpha:]')
      echo "    {\"$name\", \"$version\", \"$full_license\", $variable_name, ${variable_name}_len, ${variable_name}_string},"
      echo "#else"
      echo "    NULL_LIBRARY_ENTRY,"
      echo "#endif"
    done < "$TMP_FILE"

    echo "};"
    echo
    echo "#define LIBRARY_COUNT (sizeof(libraries) / sizeof(libraries[0]))"
    echo
    echo "/* Individual library macros */"
    index=0
    while IFS=',' read -r dir name version license full_license header_file; do
      macro_name=$(echo "$name" | tr '[:lower:]' '[:upper:]' | tr ' .-/' '____')
      echo "#define ${macro_name}_LICENSE LICENSE_ADD(libraries[$index], $index)"
      index=$((index + 1))
    done < "$TMP_FILE"

    PLATFORM_VERSION=$(arduino-cli board details -b rp2040:rp2040:rpipico | grep "Board version" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
    echo "#define RASPBERRY_PICO_LICENSE LICENSE_ADD(\"Arduino-Pico\", \"${PLATFORM_VERSION}\", \"Pico_LICENSE\", -1)"
    echo
    echo "#endif /* LIBRARIES_H */"
  } > "$TMP_HEADER"

  # Only replace if different
  if [ ! -f "$OUTPUT_FILE" ] || ! cmp -s "$TMP_HEADER" "$OUTPUT_FILE"; then
    mv "$TMP_HEADER" "$OUTPUT_FILE"
    log_passed "Header file updated: $OUTPUT_FILE"
  else
    log_info "No changes detected. Existing header kept: $OUTPUT_FILE"
  fi

  exit 0
fi

exit 1
