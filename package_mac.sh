#!/usr/bin/env bash
# Package SargisLab .app into DMG for macOS
# Usage: ./package_mac.sh <path-to-.app> [--sign] [--notarize]
#
# --sign      Sign .app before creating DMG (requires DEVELOPER_ID_CERT)
# --notarize  Sign, create DMG, notarize, staple (requires Apple credentials)
#             Output saved to dist/notarized/
set -e

APP_PATH=""
DO_SIGN=false
DO_NOTARIZE=false

for arg in "$@"; do
    case "$arg" in
        --sign) DO_SIGN=true ;;
        --notarize) DO_NOTARIZE=true ;;
        *) APP_PATH="$arg" ;;
    esac
done

if [ -z "${APP_PATH}" ] || [ ! -d "${APP_PATH}" ]; then
    echo "Usage: $0 <path-to-.app> [--sign] [--notarize]"
    echo "Example: $0 install/SargisLabScientificApp.app"
    echo "         $0 install/SargisLabScientificApp.app --sign --notarize"
    exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
APP_PATH="$(cd "$(dirname "${APP_PATH}")" && pwd)/$(basename "${APP_PATH}")"
DMG_NAME="SargisLabScientificApp"
QT_PREFIX="$(brew --prefix qt@6 2>/dev/null || echo /opt/homebrew/opt/qt@6)"
MACDEPLOYQT="${QT_PREFIX}/bin/macdeployqt"

if [ "$DO_NOTARIZE" = true ]; then
    DO_SIGN=true
    OUTPUT_DIR="${SCRIPT_DIR}/dist/notarized"
else
    OUTPUT_DIR="${SCRIPT_DIR}/dist"
fi

mkdir -p "${OUTPUT_DIR}"

echo "=== Running macdeployqt ==="
if [ ! -f "${MACDEPLOYQT}" ]; then
    echo "Error: macdeployqt not found at ${MACDEPLOYQT}"
    echo "Install Qt: brew install qt@6"
    exit 1
fi

"${MACDEPLOYQT}" "${APP_PATH}" -verbose=2

if [ "$DO_SIGN" = true ]; then
    echo "=== Signing .app ==="
    "${SCRIPT_DIR}/sign_mac.sh" "${APP_PATH}"
fi

echo "=== Creating DMG ==="
cd "${OUTPUT_DIR}"
DMG_PATH="${OUTPUT_DIR}/${DMG_NAME}.dmg"

if [ -f "${DMG_PATH}" ]; then
    rm -f "${DMG_PATH}"
fi

hdiutil create -volname "${DMG_NAME}" -srcfolder "${APP_PATH}" -ov -format UDZO "${DMG_PATH}"

if [ "$DO_NOTARIZE" = true ]; then
    echo "=== Notarizing DMG ==="
    "${SCRIPT_DIR}/notarize_mac.sh" "${DMG_PATH}"
fi

echo "=== Creating app zip ==="
(cd "$(dirname "${APP_PATH}")" && zip -r "${OUTPUT_DIR}/${DMG_NAME}.app.zip" "$(basename "${APP_PATH}")")

echo "=== Done ==="
echo "DMG: ${DMG_PATH}"
echo "App zip: ${OUTPUT_DIR}/${DMG_NAME}.app.zip"
echo "App: ${APP_PATH}"
