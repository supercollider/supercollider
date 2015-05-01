!cd ${SC_INSTALL_ROOT}

!include "Sections.nsh"
!include "FileFunc.nsh"
!include "WordFunc.nsh"
!include "MUI2.nsh"


Name "SuperCollider ${SC_VERSION}"
OutFile ${SC_DST_DIR}\SuperCollider-${SC_VERSION}-win32.exe
InstallDir $PROGRAMFILES\SuperCollider-${SC_VERSION}

!define MUI_ICON "sc_cube.ico"
!define MUI_UNICON "sc_cube.ico"

; Get install-dir from registry if available
InstallDirRegKey HKCU "Software\SuperCollider\${SC_VERSION}" ""

;Addition to refresh shell-icons
!define SHCNE_ASSOCCHANGED 0x08000000
!define SHCNF_IDLIST 0

;Interface Settings
!define MUI_ABORTWARNING

; --- PAGES ---

!insertmacro MUI_PAGE_WELCOME

!insertmacro MUI_PAGE_LICENSE SuperCollider\LICENSE.txt

; --- Hack to display Readme.md easily ---
!define MUI_PAGE_HEADER_TEXT "Read me!"
!define MUI_PAGE_HEADER_SUBTEXT "Find below useful information regarding installation and usage"
!define MUI_LICENSEPAGE_TEXT_TOP "Press Page Down to see the rest of the information"
!define MUI_LICENSEPAGE_TEXT_BOTTOM "This information will be available in your SuperCollider application folder as README.txt"
!define MUI_LICENSEPAGE_BUTTON "Next >"
!insertmacro MUI_PAGE_LICENSE SuperCollider\README.md

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
    File /r SuperCollider\*

    ;Store installation folder
	WriteRegStr HKCU "Software\SuperCollider\${SC_VERSION}" "" $INSTDIR

    ;Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"

	; Register uninstaller in Add-/remove programs and add some Metadata
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SuperCollider-${SC_VERSION}" "DisplayName" "SuperCollider Version ${SC_VERSION}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SuperCollider-${SC_VERSION}" "UninstallString" "$INSTDIR\Uninstall.exe"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SuperCollider-${SC_VERSION}" "HelpLink" "http://doc.sccode.org/"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SuperCollider-${SC_VERSION}" "URLUpdateInfo" "http://sourceforge.net/projects/supercollider/files/Windows/"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SuperCollider-${SC_VERSION}" "URLInfoAbout" "http://supercollider.sourceforge.net/"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SuperCollider-${SC_VERSION}" "DisplayVersion" "${SC_VERSION}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SuperCollider-${SC_VERSION}" "DisplayIcon" "$INSTDIR\sclang.exe"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SuperCollider-${SC_VERSION}" "NoModify" 1
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SuperCollider-${SC_VERSION}" "NoRepair" 1

	;Associate file-types scd, sc and schelp with SuperCollider
	WriteRegStr HKCR ".scd" "" "SuperCollider.Document"
	WriteRegStr HKCR "SuperCollider.Document" "" "SuperCollider Document"
	WriteRegStr HKCR "SuperCollider.Document\DefaultIcon" "" "$INSTDIR\sclang.exe,0"
	WriteRegStr HKCR "SuperCollider.Document\shell\open\command" "" '"$INSTDIR\scide.exe" "%1"'

	WriteRegStr HKCR ".sc" "" "SuperCollider.ClassFile"
	WriteRegStr HKCR "SuperCollider.ClassFile" "" "SuperCollider ClassFile"
	WriteRegStr HKCR "SuperCollider.ClassFile\DefaultIcon" "" "$INSTDIR\sclang.exe,0"
	WriteRegStr HKCR "SuperCollider.ClassFile\shell\open\command" "" '"$INSTDIR\scide.exe" "%1"'

	WriteRegStr HKCR ".schelp" "" "SuperCollider.HelpFile"
	WriteRegStr HKCR "SuperCollider.HelpFile" "" "SuperCollider HelpFile"
	WriteRegStr HKCR "SuperCollider.HelpFile\DefaultIcon" "" "$INSTDIR\sclang.exe,0"
	WriteRegStr HKCR "SuperCollider.HelpFile\shell\open\command" "" '"$INSTDIR\scide.exe" "%1"'

	; Create Startmenu item
	SetShellVarContext all
	createDirectory "$SMPROGRAMS\SuperCollider"
	createShortCut "$SMPROGRAMS\SuperCollider\SuperCollider-${SC_VERSION}.lnk" "$INSTDIR\scide.exe" "" "$INSTDIR\scide.exe"

	; Refresh shell-icons
	Call RefreshShellIcons

SectionEnd

Section "Uninstall"

    RMDir /r "$INSTDIR"

	DeleteRegKey HKCU "Software\SuperCollider\${SC_VERSION}"
	DeleteRegKey /ifempty HKCU "Software\SuperCollider"

	;Remove filetype associations
	DeleteRegKey HKCR ".scd"
	DeleteRegKey HKCR "SuperCollider.Document"

	DeleteRegKey HKCR ".sc"
	DeleteRegKey HKCR "SuperCollider.ClassFile"

	DeleteRegKey HKCR ".schelp"
	DeleteRegKey HKCR "SuperCollider.HelpFile"

	;Try to remove StartMenu item
	SetShellVarContext all
	delete "$SMPROGRAMS\SuperCollider\SuperCollider-${SC_VERSION}.lnk"
	rmDir "$SMPROGRAMS\SuperCollider"

	;Remove from Add-/remove programs
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SuperCollider-${SC_VERSION}"

SectionEnd

; --- FUNCTIONS ---

Function .onInit
    IntOp $0 ${SF_SELECTED} | ${SF_RO}
    SectionSetFlags ${core_sect} $0
FunctionEnd

Function RefreshShellIcons
  ; By jerome tremblay - april 2003
  System::Call 'shell32.dll::SHChangeNotify(i, i, i, i) v \
  (${SHCNE_ASSOCCHANGED}, ${SHCNF_IDLIST}, 0, 0)'
FunctionEnd
