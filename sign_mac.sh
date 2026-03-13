#!/usr/bin/env bash
# Sign CurveFit .app for macOS distribution
# Usage: ./sign_mac.sh /path/to/CurveFitScientificApp.app
#
# Required: DEVELOPER_ID_CERT - "Developer ID Application: Your Name (TEAM_ID)"
# Or pass as second argument: ./sign_mac.sh app.app "Developer ID Application: ..."
set -e

if [ $# -lt 1 ]; then
    echo "Usage: $0 <path-to-.app> [identity]"
    echo "Example: $0 install/CurveFitScientificApp.app"
    echo "Identity from DEVELOPER_ID_CERT or 2nd argument"
    exit 1
fi

APP_PATH="$1"
if [ ! -d "${APP_PATH}" ]; then
    echo "Error: App not found: ${APP_PATH}"
    exit 1
fi

IDENTITY="${2:-$DEVELOPER_ID_CERT}"
if [ -z "${IDENTITY}" ]; then
    echo "Error: No signing identity. Set DEVELOPER_ID_CERT or pass as 2nd argument."
    echo "Example: DEVELOPER_ID_CERT='Developer ID Application: Name (ABC123)'"
    exit 1
fi

echo "=== Signing ${APP_PATH} ==="
codesign --deep --force --options runtime \
    --sign "${IDENTITY}" \
    --timestamp \
    "${APP_PATH}"

echo "=== Verifying signature ==="
codesign --verify --deep --strict "${APP_PATH}"
echo "Signature verified successfully."
