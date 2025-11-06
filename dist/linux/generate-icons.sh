#!/bin/bash
# Script to generate PNG icons from SVG source
# Requires: rsvg-convert or inkscape

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SVG_SOURCE="${SCRIPT_DIR}/hicolor/scalable/apps/com.ktechpit.pixelbatch.svg"
SIZES=(16 32 64 128 256)

for size in "${SIZES[@]}"; do
    OUTPUT_DIR="${SCRIPT_DIR}/hicolor/${size}x${size}/apps"
    OUTPUT_FILE="${OUTPUT_DIR}/com.ktechpit.pixelbatch.png"

    echo "Generating ${size}x${size} icon..."

    # Try rsvg-convert first
    if command -v rsvg-convert &> /dev/null; then
        rsvg-convert -w $size -h $size "$SVG_SOURCE" -o "$OUTPUT_FILE"
    # Try inkscape as fallback
    elif command -v inkscape &> /dev/null; then
        inkscape -w $size -h $size "$SVG_SOURCE" --export-filename="$OUTPUT_FILE"
    else
        echo "Error: Neither rsvg-convert nor inkscape found!"
        echo "Please install librsvg2-bin or inkscape"
        exit 1
    fi
done

echo "Icon generation complete!"

