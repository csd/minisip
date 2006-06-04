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

!ifndef INSTALLDIR
!error "INSTALLDIR undefined"
!else
!define MINISIPDIR ${INSTALLDIR}
!endif

; Use Modern UI
!define MUI_COMPONENTSPAGE_SMALLDESC
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


!macro ForEachPkg command prefix postfix
${command} "${prefix}libmutil${postfix}"
${command} "${prefix}libmcrypto${postfix}"
${command} "${prefix}libmikey${postfix}"
${command} "${prefix}libmnetutil${postfix}"
${command} "${prefix}libmsip${postfix}"
${command} "${prefix}libmstun${postfix}"
${command} "${prefix}libminisip${postfix}"
!macroend

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

InstType "Minimal"
InstType "Full"

;
; Main group
;
SectionGroup "Minisip"

;
; Minisip section
;
Section "Program"
SectionIn 1 2 RO
AddSize 500

SetOutPath $INSTDIR
File copying.txt

!ifndef NOFILES
SetOutPath $INSTDIR\bin
!insertmacro ForEachPkg File "${MINISIPDIR}/bin/" "*.dll"
File ${MINISIPDIR}/bin/*.exe

SetOutPath $INSTDIR\lib\libminisip\plugins
File /nonfatal ${MINISIPDIR}/lib/libminisip/plugins/*.dll
File /nonfatal ${MINISIPDIR}/lib/libminisip/plugins/*.la

SetOutPath $INSTDIR\share\minisip
File ${MINISIPDIR}/share/minisip/insecure.png
File ${MINISIPDIR}/share/minisip/minisip.glade
File ${MINISIPDIR}/share/minisip/minisip.png
File ${MINISIPDIR}/share/minisip/noplay.png
File ${MINISIPDIR}/share/minisip/norecord.png
File ${MINISIPDIR}/share/minisip/play.png
File ${MINISIPDIR}/share/minisip/record.png
File ${MINISIPDIR}/share/minisip/secure.png
File ${MINISIPDIR}/share/minisip/tray_icon.png
!endif

WriteUninstaller "$INSTDIR\Uninstall.exe"

!insertmacro MUI_STARTMENU_WRITE_BEGIN Application

CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Minisip.lnk" "$INSTDIR\bin\minisip_gtkgui.exe"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"

!insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;
; Minisip development
;
SectionGroup "Development"
Section "Files"
SectionIn 2

!ifndef NOFILES
SetOutPath $INSTDIR\include
File /r ${MINISIPDIR}/include/*

SetOutPath $INSTDIR\lib
!insertmacro ForEachPkg File "${MINISIPDIR}/lib/" ".*a"

SetOutPath $INSTDIR\lib\pkgconfig
!insertmacro ForEachPkg File "${MINISIPDIR}/lib/pkgconfig/" ".pc"

SetOutPath $INSTDIR\share\aclocal
!insertmacro ForEachPkg File "${MINISIPDIR}/share/aclocal/" ".m4"
!endif
; End NOFILES

SectionEnd

;
; Minisip examples
;
Section "Examples"
SectionIn 2

SetOutPath $INSTDIR\share\libmutil\examples
File ${MINISIPDIR}/share/libmutil/examples/*

SetOutPath $INSTDIR\share\libmnetutil\examples
File ${MINISIPDIR}/share/libmnetutil/examples/*

SectionEnd
; End Extra
SectionGroupEnd
; End Minisip
SectionGroupEnd

;
; OpenSSL section
;
!ifdef SSLDIR
Section "OpenSSL"
SectionIn 1 2

!ifndef NOFILES
SetOutPath $INSTDIR\bin
File ${SSLDIR}/libeay32.dll
File ${SSLDIR}/ssleay32.dll
!endif

SectionEnd
!endif

;
; PortAudio V19 section
;
!ifdef PORTAUDIODIR
Section "PortAudio V19"
SectionIn 1 2

!ifndef NOFILES
SetOutPath $INSTDIR\bin
File ${PORTAUDIODIR}/libportaudio-2.dll
!endif

SectionEnd
!endif

;
; Strings
;


;
; Uninstaller
;
Section "Uninstall"

Delete "$INSTDIR\copying.txt"

; Delete EXEs and DLLs
!insertmacro ForEachPkg Delete "$INSTDIR\bin\" "*.dll"
Delete "$INSTDIR\bin\minisip_*.exe"

; Delete OpenSSL
!ifdef SSLDIR
Delete "$INSTDIR\bin\libeay32.dll"
Delete "$INSTDIR\bin\ssleay32.dll"
!endif

; Delete PortAudio
!ifdef PORTAUDIODIR
Delete "$INSTDIR\bin\libportaudio-2.dll"
!endif

; Delete plugins
Delete "$INSTDIR\lib\libminisip\plugins\*.dll"
Delete "$INSTDIR\lib\libminisip\plugins\*.la"

; Delete bitmaps
Delete "$INSTDIR\share\minisip\insecure.png"
Delete "$INSTDIR\share\minisip\minisip.glade"
Delete "$INSTDIR\share\minisip\minisip.png"
Delete "$INSTDIR\share\minisip\noplay.png"
Delete "$INSTDIR\share\minisip\norecord.png"
Delete "$INSTDIR\share\minisip\play.png"
Delete "$INSTDIR\share\minisip\record.png"
Delete "$INSTDIR\share\minisip\secure.png"
Delete "$INSTDIR\share\minisip\tray_icon.png"

; Delete development files
!insertmacro ForEachPkg Delete "$INSTDIR\lib\" ".*a"
!insertmacro ForEachPkg Delete "$INSTDIR\lib\pkgconfig\" ".pc"
!insertmacro ForEachPkg Delete "$INSTDIR\share\aclocal\" ".m4"

; Delete header files
RMDir /r "$INSTDIR\include\libmutil"
RMDir /r "$INSTDIR\include\libmcrypto"
RMDir /r "$INSTDIR\include\libmikey"
RMDir /r "$INSTDIR\include\libmnetutil"
RMDir /r "$INSTDIR\include\libmsip"
RMDir /r "$INSTDIR\include\libmstun"
RMDir /r "$INSTDIR\include\libminisip"

; Delete examples
!insertmacro ForEachPkg Delete "$INSTDIR\share\" "\examples\*"

RMDir "$INSTDIR\lib\libminisip\plugins"
RMDir "$INSTDIR\lib\libminisip"
RMDir "$INSTDIR\share\minisip"
RMDir "$INSTDIR\bin"
RMDir "$INSTDIR\include"
RMDir "$INSTDIR\lib\pkgconfig"
RMDir "$INSTDIR\lib"
RMDir "$INSTDIR\share\aclocal"
RMDir "$INSTDIR\share\libmutil\examples"
RMDir "$INSTDIR\share\libmutil"
RMDir "$INSTDIR\share\libmnetutil\examples"
RMDir "$INSTDIR\share\libmnetutil"
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
