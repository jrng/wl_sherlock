@echo off

REM ### BEGIN OF CONFIG #####

set COMPILER=cl

REM ### END OF CONFIG #######

REM  -Od    set the optimization level (Od, O1, O2, O3)
REM  -Z7    generate .pdb files for debug info
REM  -Oi    generate instrinsics
REM  -GR-   disable runtime information
REM  -EHa-  disable exception handling
REM  -Gm-   disable minimal rebuild
REM  -MT    static linkage of c standard library

set COMPILER_FLAGS=-MT -nologo -Gm- -GR- -EHa- -Oi -Z7 -W2 -GS- -Gs10000000 -I"C:/cui/"
set LINKER_FLAGS=-stack:0x100000,0x100000 -subsystem:windows,6.0 -opt:ref -libpath:"C:/cui/" -incremental:no user32.lib gdi32.lib shell32.lib uxtheme.lib comdlg32.lib d3d11.lib dxguid.lib d3dcompiler.lib cui.lib

set BUILD_TYPE=%1

if "%BUILD_TYPE%"=="reldebug" (
    set COMPILER_FLAGS=%COMPILER_FLAGS% -O2 -DCUI_DEBUG_BUILD=1
) else if "%BUILD_TYPE%"=="release" (
    set COMPILER_FLAGS=%COMPILER_FLAGS% -O2
) else (
    set COMPILER_FLAGS=%COMPILER_FLAGS% -Od -DCUI_DEBUG_BUILD=1
)

if not exist "build" (
    mkdir "build"
)

pushd "build"

echo [COMPILE] %COMPILER% %COMPILER_FLAGS% -Fe"wl_sherlock.exe" "../src/wl_sherlock.c" /link %LINKER_FLAGS%
%COMPILER% %COMPILER_FLAGS% -Fe"wl_sherlock.exe" "../src/wl_sherlock.c" /link %LINKER_FLAGS%

popd
