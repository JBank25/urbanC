#!/bin/bash


#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 <filename_without_extension>"
    exit 1
fi

base_name="$1"
target_dir="$base_name"

# Create the target directory if it doesn't exist
mkdir -p "$target_dir"

# Move files with matching name (any extension) to the target directory
mv "$base_name"* "$target_dir/" 2>/dev/null

# Check if any files were moved
if [ $? -eq 0 ]; then
    echo "Files moved successfully to $target_dir/"
else
    echo "No matching files found for $base_name"
fi
