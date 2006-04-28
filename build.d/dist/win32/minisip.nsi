;
;   Minisip Win32 install script
;
;   Copyright (C) 2006  Mikael Magnusson
;
;   This program is free software; you can redistribute it
;   and/or modify it under the terms of the GNU General
;   Public License as published by the Free Software
;   Foundation; either version 2 of the License, or (at your
;   option) any later version.
;
;   This program is distributed in the hope that it will be
;   useful, but WITHOUT ANY WARRANTY; without even the
;   implied warranty of MERCHANTABILITY or FITNESS FOR A
;   PARTICULAR PURPOSE.  See the GNU General Public License
;   for more details.
;
;   You should have received a copy of the GNU General
;   Public License along with this program; if not, write to
;   the Free Software Foundation, Inc., 59 Temple Place,
;   Suite 330, Boston, MA 02111-1307 USA
;

!ifndef VERSION
!error "VERSION undefined"
!endif

!ifndef MINISIPDIR
!error "MINISIPDIR undefined"
!endif

!ifndef SSLDIR
!error "SSLDIR undefined"
!endif

; Use Modern UI
!include "MUI.nsh"

Name "Minisip ${VERSION}"
;XPStyle on

;Function .onInit
;;  StrCpy $1 "0"
;;  System::Call 'libeay32::SSLeay(i) t (r1) .r2'
;;  MessageBox MB_YESNO "SSL version '$2' Continue?" IDYES
;
;  MessageBox MB_YESNO "This will install Minisip ${VERSION}. Continue?" IDYES NoAbort
;    Abort ; causes installer to quit.
;  NoAbort:
;
;  StrCpy $INSTDIR "$PROGRAMFILES\Minisip\"
;FunctionEnd

InstallDir "$PROGRAMFILES\Minisip"

OutFile ${OUTFILE}


;
; Variables
;
Var MUI_TEMP
Var STARTMENU_FOLDER


;
; Pages
;
!insertmacro MUI_PAGE_LICENSE "copying.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY

;Start Menu Folder Page Configuration
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Minisip" 
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

!insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

;
; Minisip section
;
Section "Minisip"
SectionIn 1 RO
AddSize 500

SetOutPath $INSTDIR
File ${MINISIPDIR}/bin/*.exe
File ${MINISIPDIR}/bin/*.dll

SetOutPath $INSTDIR\plugins
File ${MINISIPDIR}/lib/libminisip/plugins/*.dll
File ${MINISIPDIR}/lib/libminisip/plugins/*.la

SetOutPath $INSTDIR\share
File ${MINISIPDIR}/share/minisip/insecure.png
File ${MINISIPDIR}/share/minisip/minisip.glade
;File ${MINISIPDIR}/share/minisip/minisip.png
File ${MINISIPDIR}/share/minisip/noplay.png
File ${MINISIPDIR}/share/minisip/norecord.png
File ${MINISIPDIR}/share/minisip/play.png
File ${MINISIPDIR}/share/minisip/record.png
File ${MINISIPDIR}/share/minisip/secure.png
File ${MINISIPDIR}/share/minisip/tray_icon.png

WriteUninstaller "$INSTDIR\Uninstall.exe"

!insertmacro MUI_STARTMENU_WRITE_BEGIN Application

CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Minisip.lnk" "$INSTDIR\minisip_gtkgui.exe"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"

!insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;
; OpenSSL section
;
Section "OpenSSL"
SectionIn 1

SetOutPath $INSTDIR
File ${SSLDIR}/libeay32.dll
File ${SSLDIR}/ssleay32.dll
SectionEnd

;
; Strings
;


;
; Uninstaller
;
Section "Uninstall"

; Delete EXEs and DLLs
Delete "$INSTDIR\libmutil*.dll"
Delete "$INSTDIR\libmcrypto*.dll"
Delete "$INSTDIR\libmikey*.dll"
Delete "$INSTDIR\libmnetutil*.dll"
Delete "$INSTDIR\libmsip*.dll"
Delete "$INSTDIR\libminisip*.dll"
Delete "$INSTDIR\minisip_*.exe"

; Delete OpenSSL
Delete "$INSTDIR\libeay32.dll"
Delete "$INSTDIR\ssleay32.dll"

; Delete plugins
Delete "$INSTDIR\plugins\*.dll"
Delete "$INSTDIR\plugins\*.la"
RMDir "$INSTDIR\plugins"

; Delete bitmaps
Delete "$INSTDIR\share\insecure.png"
Delete "$INSTDIR\share\minisip.glade"
;Delete "$INSTDIR\share\minisip.png"
Delete "$INSTDIR\share\noplay.png"
Delete "$INSTDIR\share\norecord.png"
Delete "$INSTDIR\share\play.png"
Delete "$INSTDIR\share\record.png"
Delete "$INSTDIR\share\secure.png"
Delete "$INSTDIR\share\tray_icon.png"
RMDir "$INSTDIR\share"

Delete "$INSTDIR\Uninstall.exe"
RMDir "$INSTDIR"

!insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP

Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall.lnk"
Delete "$SMPROGRAMS\$MUI_TEMP\Minisip.lnk"

;Delete empty start menu parent diretories
StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"

startMenuDeleteLoop:
	ClearErrors
    RMDir $MUI_TEMP
    GetFullPathName $MUI_TEMP "$MUI_TEMP\.."
    
    IfErrors startMenuDeleteLoopDone
  
    StrCmp $MUI_TEMP $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
  startMenuDeleteLoopDone:

SectionEnd
