!define APP_NAME "CurveFit Scientific App"
!define APP_VERSION "${VERSION}"
!define APP_EXE "CurveFitScientificApp.exe"
!define INSTALL_DIR "$PROGRAMFILES64\${APP_NAME}"

!include "MUI2.nsh"

!define MUI_ICON "resources\app.ico"
!define MUI_UNICON "resources\app.ico"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "Russian"

Section "Install"
    SetOutPath "${INSTALL_DIR}"
    File /r "CurveFitScientificApp-${APP_VERSION}-win64\*.*"

    CreateShortcut "$DESKTOP\${APP_NAME}.lnk" "${INSTALL_DIR}\${APP_EXE}"
    CreateShortcut "$SMPROGRAMS\${APP_NAME}.lnk" "${INSTALL_DIR}\${APP_EXE}"
SectionEnd

Section "Uninstall"
    Delete "$DESKTOP\${APP_NAME}.lnk"
    Delete "$SMPROGRAMS\${APP_NAME}.lnk"
    RMDir /r "${INSTALL_DIR}"
SectionEnd
