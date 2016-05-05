!cd ${SC_INSTALL_ROOT}

!include "Sections.nsh"
!include "FileFunc.nsh"
!include "WordFunc.nsh"
!include "MUI2.nsh"
!include "LogicLib.nsh"
!include "x64.nsh"

Name "${BUNDLE_NAME} ${SC_VERSION}"
OutFile ${SC_DST_DIR}\${BUNDLE_NAME}-${SC_VERSION}_${FILE_NAME_SUFIX}.exe

!define MUI_ICON ${SC_ICON}
!define MUI_UNICON ${SC_ICON}

; Get install-dir from registry if available
InstallDirRegKey HKCU "Software\${BUNDLE_NAME}\${SC_VERSION}" ""

;Addition to refresh shell-icons
!define SHCNE_ASSOCCHANGED 0x08000000
!define SHCNF_IDLIST 0

;Interface Settings
!define MUI_ABORTWARNING

; --- PAGES ---

!insertmacro MUI_PAGE_WELCOME

!insertmacro MUI_PAGE_LICENSE ${BUNDLE_NAME}\COPYING

; --- Hack to display Readme.md easily ---
!define MUI_PAGE_HEADER_TEXT "Read me!"
!define MUI_PAGE_HEADER_SUBTEXT "Find below useful information regarding installation and usage"
!define MUI_LICENSEPAGE_TEXT_TOP "Press Page Down to see the rest of the information"
!define MUI_LICENSEPAGE_TEXT_BOTTOM "For more Windows specific information please consult the file README_WINDOWS.md in the ${BUNDLE_NAME} application folder"
!define MUI_LICENSEPAGE_BUTTON "Next >"
!insertmacro MUI_PAGE_LICENSE ${BUNDLE_NAME}\README.md

!insertmacro MUI_PAGE_DIRECTORY

!insertmacro MUI_PAGE_INSTFILES

; --- Uninstaller Pages ---

!insertmacro MUI_UNPAGE_WELCOME

!insertmacro MUI_UNPAGE_CONFIRM

!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_UNPAGE_FINISH

; --- LANGUAGE ---

!insertmacro MUI_LANGUAGE "English"

; --- SECTIONS ---

Section "Core" core_sect
    SetOutPath $INSTDIR
    File /r ${BUNDLE_NAME}\*

    ;Store installation folder
  WriteRegStr HKCU "Software\${BUNDLE_NAME}\${SC_VERSION}" "" $INSTDIR

    ;Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"

  ; Register uninstaller in Add-/remove programs and add some Metadata
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${BUNDLE_NAME}-${SC_VERSION}" "DisplayName" "${BUNDLE_NAME} Version ${SC_VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${BUNDLE_NAME}-${SC_VERSION}" "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${BUNDLE_NAME}-${SC_VERSION}" "Publisher" "SuperCollider Community"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${BUNDLE_NAME}-${SC_VERSION}" "HelpLink" "http://doc.sccode.org/"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${BUNDLE_NAME}-${SC_VERSION}" "URLUpdateInfo" "https://github.com/supercollider/supercollider/releases/"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${BUNDLE_NAME}-${SC_VERSION}" "URLInfoAbout" "http://supercollider.github.io/"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${BUNDLE_NAME}-${SC_VERSION}" "DisplayVersion" "${SC_VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${BUNDLE_NAME}-${SC_VERSION}" "EstimatedSize" "${SC_SIZE}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${BUNDLE_NAME}-${SC_VERSION}" "DisplayIcon" "$INSTDIR\sclang.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${BUNDLE_NAME}-${SC_VERSION}" "NoModify" 1
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${BUNDLE_NAME}-${SC_VERSION}" "NoRepair" 1

  ;Associate file-types scd, sc and schelp with SuperCollider
  WriteRegStr HKCR ".scd" "" "${BUNDLE_NAME}.Document"
  WriteRegStr HKCR "${BUNDLE_NAME}.Document" "" "${BUNDLE_NAME} Document"
  WriteRegStr HKCR "${BUNDLE_NAME}.Document\DefaultIcon" "" "$INSTDIR\sclang.exe,0"
  WriteRegStr HKCR "${BUNDLE_NAME}.Document\shell\open\command" "" '"$INSTDIR\scide.exe" "%1"'

  WriteRegStr HKCR ".sc" "" "${BUNDLE_NAME}.ClassFile"
  WriteRegStr HKCR "${BUNDLE_NAME}.ClassFile" "" "${BUNDLE_NAME} ClassFile"
  WriteRegStr HKCR "${BUNDLE_NAME}.ClassFile\DefaultIcon" "" "$INSTDIR\sclang.exe,0"
  WriteRegStr HKCR "${BUNDLE_NAME}.ClassFile\shell\open\command" "" '"$INSTDIR\scide.exe" "%1"'

  WriteRegStr HKCR ".schelp" "" "${BUNDLE_NAME}.HelpFile"
  WriteRegStr HKCR "${BUNDLE_NAME}.HelpFile" "" "${BUNDLE_NAME} HelpFile"
  WriteRegStr HKCR "${BUNDLE_NAME}.HelpFile\DefaultIcon" "" "$INSTDIR\sclang.exe,0"
  WriteRegStr HKCR "${BUNDLE_NAME}.HelpFile\shell\open\command" "" '"$INSTDIR\scide.exe" "%1"'
  
  ; Add path information
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\App Paths\scide.exe" "" "$INSTDIR\scide.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\App Paths\scide.exe" "Path" "$INSTDIR"  

  ; Create Startmenu item
  SetShellVarContext all
  createDirectory "$SMPROGRAMS\${BUNDLE_NAME}"
  createShortCut "$SMPROGRAMS\${BUNDLE_NAME}\${BUNDLE_NAME}-${SC_VERSION}.lnk" "$INSTDIR\scide.exe" "" "$INSTDIR\scide.exe"

  ; Refresh shell-icons
  Call RefreshShellIcons

SectionEnd

Section "Uninstall"

    RMDir /r "$INSTDIR"

  DeleteRegKey HKCU "Software\${BUNDLE_NAME}\${SC_VERSION}"
  DeleteRegKey /ifempty HKCU "Software\${BUNDLE_NAME}"

  ;Remove filetype associations
  DeleteRegKey HKCR ".scd"
  DeleteRegKey HKCR "${BUNDLE_NAME}.Document"

  DeleteRegKey HKCR ".sc"
  DeleteRegKey HKCR "${BUNDLE_NAME}.ClassFile"

  DeleteRegKey HKCR ".schelp"
  DeleteRegKey HKCR "${BUNDLE_NAME}.HelpFile"
  
  ;Remove path information
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\App Paths\scide.exe"

  ;Try to remove StartMenu item
  SetShellVarContext all
  delete "$SMPROGRAMS\${BUNDLE_NAME}\${BUNDLE_NAME}-${SC_VERSION}.lnk"
  rmDir "$SMPROGRAMS\${BUNDLE_NAME}"

  ;Remove from Add-/remove programs
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${BUNDLE_NAME}-${SC_VERSION}"

SectionEnd

; --- FUNCTIONS ---

Function .onInit
  ${If} ${TARCH} == "x64"
    StrCpy $INSTDIR "$PROGRAMFILES64\${BUNDLE_NAME}-${SC_VERSION}"
  ${Else}
    StrCpy $INSTDIR "$PROGRAMFILES\${BUNDLE_NAME}-${SC_VERSION}"
  ${EndIf}
FunctionEnd

Function RefreshShellIcons
  ; By jerome tremblay - april 2003
  System::Call 'shell32.dll::SHChangeNotify(i, i, i, i) v \
  (${SHCNE_ASSOCCHANGED}, ${SHCNF_IDLIST}, 0, 0)'
FunctionEnd
