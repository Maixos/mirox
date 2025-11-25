#!/bin/bash
#
# install_deps.sh
# Install dependency files from the current directory into a target path.
#

set -e

TARGET_DIR="/usr/local/mirox"      # Destination directory
CURRENT_DIR=$(pwd)                 # Absolute path of current directory
SCRIPT_NAME=$(basename "$0")       # Script name

echo "[INFO] Installing dependencies from: $CURRENT_DIR"
echo "[INFO] Target installation directory: $TARGET_DIR"

# Create target directory if not exists
if ! sudo mkdir -p "$TARGET_DIR"; then
    echo "[ERROR] Failed to create target directory: $TARGET_DIR"
    exit 1
fi

echo "[INFO] Copying files..."

# Files/directories to ignore
IGNORE_LIST=(
    "."
    ".git"
    ".gitignore"
    "setup.sh"
    "$SCRIPT_NAME"
)

# Build find exclusion parameters
FIND_EXCLUDE=()
for item in "${IGNORE_LIST[@]}"; do
    FIND_EXCLUDE+=(! -name "$item")
done

# Copy all relevant files to target
if ! find . -maxdepth 1 "${FIND_EXCLUDE[@]}" -exec sudo cp -fr {} "$TARGET_DIR" \; ; then
    echo "[ERROR] Failed to copy files to $TARGET_DIR"
    exit 1
fi

echo "[INFO] Dependencies successfully installed to: $TARGET_DIR"
