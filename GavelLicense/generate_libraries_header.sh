#!/bin/bash

# Enable strict mode
set -euo pipefail
IFS=$'\n\t'
shopt -s nullglob

# Source helper scripts or define fallback
if ! source common.sh 2>/dev/null; then
  log_info() { echo "$@"; }
fi

START_DIR="${1:-.}"
OUTPUT_FILE=${2:-"libraries.h"}
TMP_FILE=$(mktemp)
TMP_HEADER=$(mktemp)

# Remove old temp files on exit
trap 'rm -f "$TMP_FILE" "$TMP_HEADER"' EXIT

# Collect data
find "$START_DIR" -type d -name "Gavel*" -prune -o -type f -name "library.properties" -print 2>/dev/null | while read -r file; do
    dir_path=$(dirname "$file")
    dir_name=$(basename "$dir_path")

    lib_name=$(grep -E '^name=' "$file" | cut -d'=' -f2- | tr -d '\r')
    lib_version=$(grep -E '^version=' "$file" | cut -d'=' -f2- | tr -d '\r')

    [ -z "$lib_name" ] && lib_name="UNKNOWN"
    [ -z "$lib_version" ] && lib_version="0.0.0"

    license_file=$(find "$dir_path" -maxdepth 1 -type f \( \
        -iname "license" -o \
        -iname "license.*" -o \
        -iname "licence" -o \
        -iname "licence.*" -o \
        -iname "copying" -o \
        -iname "copying.*" -o \
        -iname "*.adoc" \
    \) -print -quit)

    [ -n "$license_file" ] && license_name=$(basename "$license_file") || license_name="N/A"

    echo "$dir_name,$lib_name,$lib_version,$license_name" >> "$TMP_FILE"
done

# Sort for consistency
sort "$TMP_FILE" -o "$TMP_FILE"

# Generate header into TMP_HEADER
{
    echo "/* Auto-generated library definitions */"
    echo "#ifndef LIBRARIES_H"
    echo "#define LIBRARIES_H"
    echo
    echo "typedef struct {"
    echo "  const char* name;"
    echo "  const char* version;"
    echo "  const char* license_file;"
    echo "} LibraryInfo;"
    echo
    echo "static const LibraryInfo libraries[] = {"

    while IFS=',' read -r dir name version license; do
        name=${name//\"/\\\"}
        version=${version//\"/\\\"}
        license=${license//\"/\\\"}
        # Prepend library name to license and replace spaces with underscores
        full_license="${name}_${license}"
        full_license=${full_license// /_}
        echo "    {\"$name\", \"$version\", \"$full_license\"},"
    done < "$TMP_FILE"

    echo "};"
    echo
    echo "#define LIBRARY_COUNT (sizeof(libraries) / sizeof(libraries[0]))"
    echo
    echo "/* Individual library macros */"
    index=0
    while IFS=',' read -r dir name version license; do
        macro_name=$(echo "$name" | tr '[:lower:]' '[:upper:]' | tr ' .-/' '____')
        echo "#define ${macro_name}_LICENSE LICENSE_ADD(libraries[$index])"
        index=$((index + 1))
    done < "$TMP_FILE"

    PLATFORM_VERSION=$(arduino-cli board details -b rp2040:rp2040:rpipico | grep "Board version" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
    echo "#define RASPBERRY_PICO_LICENSE LICENSE_ADD(\"Arduino-Pico\", \"${PLATFORM_VERSION}\", \"Pico_LICENSE\")"
    echo
    echo "#endif /* LIBRARIES_H */"
} > "$TMP_HEADER"

# Only replace if different
if [ ! -f "$OUTPUT_FILE" ] || ! cmp -s "$TMP_HEADER" "$OUTPUT_FILE"; then
    mv "$TMP_HEADER" "$OUTPUT_FILE"
    log_info "Header file updated: $OUTPUT_FILE"
else
    log_info "No changes detected. Existing header kept: $OUTPUT_FILE"
fi