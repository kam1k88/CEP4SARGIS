#!/usr/bin/env bash
# Notarize DMG for macOS distribution
# Usage: ./notarize_mac.sh /path/to/CurveFitScientificApp.dmg
#
# Required env vars (or GitHub Secrets):
#   APPLE_ID       - Apple ID email
#   TEAM_ID        - Team ID (10 chars)
#   APP_SPECIFIC_PASSWORD - App-specific password from appleid.apple.com
set -e

if [ $# -lt 1 ]; then
    echo "Usage: $0 <path-to-.dmg>"
    echo "Example: $0 dist/CurveFitScientificApp.dmg"
    exit 1
fi

DMG_PATH="$1"
if [ ! -f "${DMG_PATH}" ]; then
    echo "Error: DMG not found: ${DMG_PATH}"
    exit 1
fi

for var in APPLE_ID TEAM_ID APP_SPECIFIC_PASSWORD; do
    if [ -z "${!var}" ]; then
        echo "Error: $var is not set"
        exit 1
    fi
done

echo "=== Submitting for notarization ==="
xcrun notarytool submit "${DMG_PATH}" \
    --apple-id "${APPLE_ID}" \
    --team-id "${TEAM_ID}" \
    --password "${APP_SPECIFIC_PASSWORD}" \
    --wait

echo "=== Stapling notarization ticket to DMG ==="
xcrun stapler staple "${DMG_PATH}"

echo "=== Notarization complete ==="
echo "DMG: ${DMG_PATH}"
