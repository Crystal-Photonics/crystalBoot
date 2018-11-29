SET PATH=%PATH%;C:\Qt\Tools\mingw530_32\bin

echo %PATH%

set CRYSTAL_BOOT_RELEASE_PATH_local= build-crystalBootImageCreator-Desktop_Qt_5_8_0_MinGW_32bit-Release\app\release
set CRYSTAL_BOOT_PATH_remote=\\amelie\austausch\Alle\CrystalTestFramework\_executables\crystal_boot_image_creator


copy %CRYSTAL_BOOT_RELEASE_PATH_local%\..\..\src\release\crystalBoot_image_creator.dll %CRYSTAL_BOOT_RELEASE_PATH_local%\ || goto :FAIL

C:\Qt\5.8\mingw53_32\bin\windeployqt -xml  %CRYSTAL_BOOT_RELEASE_PATH_local%\crystalBoot_image_creator.exe  || GOTO :FAIL

xcopy /s /Y %CRYSTAL_BOOT_RELEASE_PATH_local% %CRYSTAL_BOOT_PATH_remote%\

@goto :EOF
:FAIL
@echo off
IF %ERRORLEVEL% NEQ 0 (
	echo Error %ERRORLEVEL%, aborting
	EXIT /B %ERRORLEVEL%
)

