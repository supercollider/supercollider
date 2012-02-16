!cd ${SC_SRC_DIR}

!include "Sections.nsh"
!include "FileFunc.nsh"
!include "WordFunc.nsh"
!include "MUI2.nsh"
!insertmacro MUI_LANGUAGE "English"

!define VERSION "3.5-rc1"
Name "SuperCollider ${VERSION}"
OutFile ${SC_DST_DIR}\SuperCollider-${VERSION}-win32.exe
InstallDir $PROGRAMFILES\SuperCollider-${VERSION}

!ifdef SC_ED
Var GEDIT_DIR
!endif

; --- PAGES ---

!define MUI_PAGE_HEADER_TEXT "License"
!define MUI_LICENSEPAGE_TEXT_TOP "Please review the license below."
!define MUI_LICENSEPAGE_TEXT_BOTTOM \
    "By installing this software you implicitely agree with this license."
!insertmacro MUI_PAGE_LICENSE SuperCollider\COPYING

!define MUI_PAGE_HEADER_TEXT "Installation components"
!insertmacro MUI_PAGE_COMPONENTS

!define MUI_PAGE_HEADER_TEXT "Installation Folder"
!insertmacro MUI_PAGE_DIRECTORY

!ifdef SC_ED
!define MUI_DIRECTORYPAGE_TEXT_DESTINATION "Gedit Folder"
!define MUI_DIRECTORYPAGE_VARIABLE $GEDIT_DIR
!define MUI_PAGE_HEADER_TEXT "Gedit Folder"
!define MUI_PAGE_CUSTOMFUNCTION_PRE skipIfNoSced
!define MUI_DIRECTORYPAGE_TEXT_TOP \
    "Setup will install Sced - a plugin for gedit text editor \
    that allows you to interact with SuperCollider from within the editor, \
    and adds syntax highlighting for SuperCollider code.$\n$\n\
    Please select a folder where gedit is installed:"
!insertmacro MUI_PAGE_DIRECTORY
!endif

!insertmacro MUI_PAGE_INSTFILES

; --- SECTIONS ---

Section "Core" core_sect
    SetOutPath $INSTDIR
    File /r SuperCollider\*
SectionEnd

!ifdef SC_ED
Section "Sced (gedit integration)" sced_sect
    SetOutPath $GEDIT_DIR
    File /r gedit\*
    FileOpen $0 $APPDATA\sced.config.json w
        IfErrors done
        ${WordReplace} $INSTDIR "\" "\\" "+" $1
        FileWrite $0 "{$\"supercollider-dir$\": $\"$1$\"}"
    FileClose $0
    done:
SectionEnd
!endif

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${core_sect} \
        "Language interpreter and sound synthesis server. Command-line only."
    !ifdef SC_ED
    !insertmacro MUI_DESCRIPTION_TEXT ${sced_sect} \
        "Integration with gedit text editor.$\n$\n\
        You need to have gedit installed."
    !endif
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; --- FUNCTIONS ---

Function .onInit
    IntOp $0 ${SF_SELECTED} | ${SF_RO}
    SectionSetFlags ${core_sect} $0
FunctionEnd

!ifdef SC_ED
Function skipIfNoSced
    SectionGetFlags ${sced_sect} $0
    IntOp $0 $0 & ${SF_SELECTED}
    IntCmp $0 0 is0 ok ok
    is0:
        Abort
    ok:
        ${Locate} $PROGRAMFILES32 "/G=0 /L=D /M=gedit*" geditFound
        StrCmp $GEDIT_DIR "" 0 done
        ${Locate} $PROGRAMFILES64 "/G=0 /L=D /M=gedit*" geditFound
    done:
FunctionEnd

Function geditFound
    StrCpy $GEDIT_DIR $R9
    Push "StopLocate"
FunctionEnd
!endif
