;--------------------------------
;Includes

  !include "MUI2.nsh"
  !include "LogicLib.nsh"
  !include "x64.nsh"

  
;--------------------------------
;General

  ;Name and file
  Name "DroidPad"
  OutFile "droidpad-setup.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\DroidPad"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKLM "Software\DroidPad" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin

;--------------------------------
;Variables

  Var StartMenuFolder

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

Function .onInit
    ${If} ${RunningX64}
        ${DisableX64FSRedirection}
        SetRegView 64
	StrCpy $INSTDIR "$PROGRAMFILES64\DroidPad"
    ${EndIf}
  ; Remove old DroidPad version
  ; Check to see if already installed
  ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{046B525C-FEC6-42A2-A637-53CC2F58100E}_is1" "UninstallString"
  ${If} ${FileExists} $R0
    ${OrIf} ${FileExists} "C:\Program Files\DroidPad\unins000.exe"
    ${OrIf} ${FileExists} "C:\Program Files (x86)\DroidPad\unins000.exe"
      ${If} ${Cmd} 'MessageBox MB_YESNO "You currently have a previous version of DroidPad installed; would you like to remove it first?$\nThis is recommended as it is no longer needed.$\nThis will also remove the old joystick software used by DroidPad from this computer,$\nwhich is also no longer needed. When prompted, please uninstall the old joystick driver, PPJoy." IDYES'
  
        nsExec::Exec $R0
    ${EndIf}
  ${EndIf}
  
FunctionEnd


;--------------------------------
;Pages

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "COPYING"
  ; !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  
  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\DroidPad" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH
  
  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "DroidPad" DESC_DPInstall

;64-bit stuff
	SetOutPath "$INSTDIR"
  
    ${If} ${RunningX64}
	File "build\winexport64\droidpad.exe"
	File "build\winexport64\droidpad.exe.manifest"
	File "build\winexport64\libgcc_s_sjlj-1.dll"
	File "build\winexport64\libstdc++-6.dll"
	File "build\winexport64\wxbase28u_gcc_custom.dll"
	File "build\winexport64\wxbase28u_net_gcc_custom.dll"
	File "build\winexport64\wxbase28u_xml_gcc_custom.dll"
	File "build\winexport64\wxmsw28u_adv_gcc_custom.dll"
	File "build\winexport64\wxmsw28u_core_gcc_custom.dll"
	File "build\winexport64\wxmsw28u_html_gcc_custom.dll"
	File "build\winexport64\wxmsw28u_xrc_gcc_custom.dll"
	${Else}
	File "build\winexport32\droidpad.exe"
	File "build\winexport32\droidpad.exe.manifest"
	File "build\winexport32\mingwm10.dll"
	File "build\winexport32\wxbase28u_gcc_custom.dll"
	File "build\winexport32\wxbase28u_net_gcc_custom.dll"
	File "build\winexport32\wxbase28u_xml_gcc_custom.dll"
	File "build\winexport32\wxmsw28u_adv_gcc_custom.dll"
	File "build\winexport32\wxmsw28u_core_gcc_custom.dll"
	File "build\winexport32\wxmsw28u_html_gcc_custom.dll"
	File "build\winexport32\wxmsw28u_xrc_gcc_custom.dll"
	${EndIf}
	
	; The data is the same for both platforms
	SetOutPath "$INSTDIR\data"
	File "build\winexport32\data\icon.xpm"
	File "build\winexport32\data\layout.xrc"
	File "build\winexport32\data\iconlarge.png"
	File "build\winexport32\data\reactos-logo.png"
	SetOutPath "$INSTDIR\data\docs"
	File "build\winexport32\data\docs\intro.zip"
	SetOutPath "$INSTDIR\data\adb"
	File "build\winexport32\data\adb\adb.exe"
	File "build\winexport32\data\adb\AdbWinApi.dll"
	File "build\winexport32\data\adb\AdbWinUsbApi.dll"
	SetOutPath "$INSTDIR\data\driver"
	${If} ${RunningX64}
	SetOutPath "$INSTDIR\data\driver\amd64"
	File "build\winexport32\data\driver\amd64\hidkmdf.sys"
	File "build\winexport32\data\driver\amd64\droidpad.cat"
	File "build\winexport32\data\driver\amd64\droidpad.inf"
	File "build\winexport32\data\driver\amd64\droidpad.sys"
	File "build\winexport32\data\driver\amd64\WdfCoInstaller01009.dll"
	File "build\winexport32\data\driver\amd64\WUDFUpdate_01009.dll"
	${Else}
	SetOutPath "$INSTDIR\data\driver\x86"
	File "build\winexport32\data\driver\x86\hidkmdf.sys"
	File "build\winexport32\data\driver\x86\droidpad.cat"
	File "build\winexport32\data\driver\x86\droidpad.inf"
	File "build\winexport32\data\driver\x86\droidpad.sys"
	File "build\winexport32\data\driver\x86\WdfCoInstaller01009.dll"
	File "build\winexport32\data\driver\x86\WUDFUpdate_01009.dll"
	${EndIf}

	DetailPrint "Installing Driver"
	nsExec::Exec "$\"$INSTDIR\droidpad.exe$\" -s"
  
  ;Store installation folder
  WriteRegStr HKLM "Software\DroidPad" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  MessageBox MB_OK "Thank you for installing DroidPad.$\nTo access the complete set of features available,$\nplease make sure to update the Android application in Google Play."
  
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\DroidPad.lnk" "$INSTDIR\droidpad.exe"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder\Advanced"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Advanced\Install Driver.lnk" "$INSTDIR\droidpad.exe" "-s"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Advanced\Remove Driver.lnk" "$INSTDIR\droidpad.exe" "-u"
  
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_DPInstall ${LANG_ENGLISH} "Install DroidPad"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_DPInstall)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

Section "Uninstall"

	SetOutPath "$INSTDIR"
  
	DetailPrint "Removing Driver"
  nsExec::Exec "$\"$INSTDIR\droidpad.exe$\" -u"
  
	Delete "$INSTDIR\droidpad.exe"
	Delete "$INSTDIR\droidpad.exe.manifest"
	Delete "$INSTDIR\libgcc_s_sjlj-1.dll"
	Delete "$INSTDIR\libstdc++-6.dll"
	Delete "$INSTDIR\mingwm10.dll"
	Delete "$INSTDIR\wxbase28u_gcc_custom.dll"
	Delete "$INSTDIR\wxbase28u_net_gcc_custom.dll"
	Delete "$INSTDIR\wxbase28u_xml_gcc_custom.dll"
	Delete "$INSTDIR\wxmsw28u_adv_gcc_custom.dll"
	Delete "$INSTDIR\wxmsw28u_core_gcc_custom.dll"
	Delete "$INSTDIR\wxmsw28u_html_gcc_custom.dll"
	Delete "$INSTDIR\wxmsw28u_xrc_gcc_custom.dll"
	Delete "$INSTDIR\data\icon.xpm"
	Delete "$INSTDIR\data\layout.xrc"
	Delete "$INSTDIR\data\iconlarge.png"
	Delete "$INSTDIR\data\reactos-logo.png"
	Delete "$INSTDIR\data\docs\intro.zip"
	Delete "$INSTDIR\data\adb\adb.exe"
	Delete "$INSTDIR\data\adb\AdbWinApi.dll"
	Delete "$INSTDIR\data\adb\AdbWinUsbApi.dll"
	Delete "$INSTDIR\data\driver\amd64\hidkmdf.sys"
	Delete "$INSTDIR\data\driver\amd64\droidpad.cat"
	Delete "$INSTDIR\data\driver\amd64\droidpad.inf"
	Delete "$INSTDIR\data\driver\amd64\droidpad.sys"
	Delete "$INSTDIR\data\driver\amd64\WdfCoInstaller01009.dll"
	Delete "$INSTDIR\data\driver\amd64\WUDFUpdate_01009.dll"
	Delete "$INSTDIR\data\driver\x86\hidkmdf.sys"
	Delete "$INSTDIR\data\driver\x86\droidpad.cat"
	Delete "$INSTDIR\data\driver\x86\droidpad.inf"
	Delete "$INSTDIR\data\driver\x86\droidpad.sys"
	Delete "$INSTDIR\data\driver\x86\WdfCoInstaller01009.dll"
	Delete "$INSTDIR\data\driver\x86\WUDFUpdate_01009.dll"
	RMDir "$INSTDIR\data\driver\amd64"
	RMDir "$INSTDIR\data\driver\x86"
	RMDir "$INSTDIR\data\driver"
	RMDir "$INSTDIR\data\docs"
	RMDir "$INSTDIR\data\adb"
	RMDir "$INSTDIR\data"
  

  Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR"
  
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    
  Delete "$SMPROGRAMS\$StartMenuFolder\Advanced\Install Driver.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Advanced\Remove Driver.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder\Advanced"
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\DroidPad.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  
  DeleteRegKey /ifempty HKLM "Software\DroidPad"

SectionEnd
