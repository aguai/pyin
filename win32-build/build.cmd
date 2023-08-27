
rem  Run this from within the top-level project dir: win32-build\build.cmd

rem  NB this nowadays only makes a 64-bit Windows build, not a 32-bit one!

echo on

set STARTPWD=%CD%

set vcvarsall="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"

if not exist %vcvarsall% (
@   echo "Could not find MSVC vars batch file"
@   exit /b 2
)

set WIXDIR=C:\Program Files (x86)\WiX Toolset v3.11
if not exist "%WIXDIR%\bin" (
@   echo Could not find WiX Toolset
@   exit /b 2
)

set NAME=Open Source Developer, Christopher Cannam

set ORIGINALPATH=%PATH%
set PATH=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Bin;%PATH%

set ARG=%1
shift
if "%ARG%" == "sign" (
@   echo NOTE: sign option specified, will attempt to codesign exe and msi
@   echo NOTE: starting by codesigning an unrelated executable, so we know
@   echo NOTE: whether it'll work before doing the entire build
copy "%WIXDIR%\bin\light.exe" signtest.exe
signtool sign /v /n "%NAME%" /t http://time.certum.pl /fd sha1 /a signtest.exe
if errorlevel 1 exit /b %errorlevel%
signtool verify /pa signtest.exe
if errorlevel 1 exit /b %errorlevel%
del signtest.exe
@   echo NOTE: success
) else (
@   echo NOTE: sign option not specified, will not codesign anything
)

call %vcvarsall% amd64

set PATH=%SMLNJDIR%\bin;%WIXDIR%\bin;%PATH%

cd %STARTPWD%

cd win32-build
msbuild pyin.vcxproj /t:Rebuild /p:Configuration=Release
if %errorlevel% neq 0 exit /b %errorlevel%

cd %STARTPWD%

call %vcvarsall% x86

cd win32-build
msbuild pyin.vcxproj /t:Rebuild /p:Configuration=Release
if %errorlevel% neq 0 exit /b %errorlevel%

if "%ARG%" == "sign" (
@echo Signing plugins
signtool sign /v /n "%NAME%" /t http://time.certum.pl /fd sha1 /a Release\pyin.dll
signtool verify /pa Release\pyin.dll
signtool sign /v /n "%NAME%" /t http://time.certum.pl /fd sha1 /a x64\Release\pyin.dll
signtool verify /pa x64\Release\pyin.dll
)

del pyin.msi
candle -v pyin.wxs
light -ext WixUIExtension -v pyin.wixobj
if %errorlevel% neq 0 exit /b %errorlevel%
del pyin.wixobj
del pyin.wixpdb

if "%ARG%" == "sign" (
@echo Signing package
signtool sign /v /n "%NAME%" /t http://time.certum.pl /fd sha1 /a pyin.msi
signtool verify /pa pyin.msi
)

set PATH=%ORIGINALPATH%

cd %STARTPWD%
@echo Done

