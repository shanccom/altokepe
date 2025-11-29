@echo off
REM =============================================================
REM Build y ejecución del Cliente Recepcionista (Qt6 + MinGW)
REM Autodetecta Qt/MinGW si no se definieron QT_ROOT y MINGW_BIN.
REM =============================================================

setlocal EnableDelayedExpansion
if not defined QT_ROOT (
  for /d %%D in (C:\Qt\6.*) do (
    if exist "%%D\mingw_64\bin\Qt6Core.dll" set QT_ROOT=%%D\mingw_64
  )
)
if not defined QT_ROOT (
  echo [ERROR] No se detecto QT_ROOT. Define QT_ROOT y vuelve a ejecutar.
  exit /b 1
)
if not defined MINGW_BIN (
  for /d %%M in (C:\Qt\Tools\mingw*64) do (
    if exist "%%M\bin\g++.exe" set MINGW_BIN=%%M\bin
  )
)
if not defined MINGW_BIN (
  echo [WARN] MinGW no detectado. Si usas otro, define MINGW_BIN.
)
echo [INFO] QT_ROOT=%QT_ROOT%
echo [INFO] MINGW_BIN=%MINGW_BIN%
set PATH=%MINGW_BIN%;%QT_ROOT%\bin;%PATH%
endlocal & set PATH=%PATH% & set QT_ROOT=%QT_ROOT% & set MINGW_BIN=%MINGW_BIN%

set PROJ_ROOT=%~dp0
cd /d "%PROJ_ROOT%clients\recepcionista"

REM Limpiar cache si generador diferente
if exist build\CMakeCache.txt (
  findstr /I "CMAKE_GENERATOR:INTERNAL=NMake Makefiles" build\CMakeCache.txt >nul && (
    echo [INFO] Cache con NMake detectada. Borrando build...
    rmdir /S /Q build
  )
)

if not exist build mkdir build
echo [INFO] Configurando CMake (MinGW Makefiles)...
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="%QT_ROOT%" -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
  echo [ERROR] Fallo configuracion CMake.
  exit /b 1
)

echo [INFO] Compilando ClienteRecepcionista...
cmake --build build -j --config Release
if errorlevel 1 (
  echo [ERROR] Fallo compilacion.
  exit /b 1
)

set EXE=build\ClienteRecepcionista.exe
if not exist %EXE% (
  echo [ERROR] No se encontró el ejecutable %EXE%
  exit /b 1
)

echo [INFO] Iniciando Cliente Recepcionista...
echo -------------------------------------------------------------
%EXE%
echo -------------------------------------------------------------
echo Cliente Recepcionista finalizado.
pause
exit /b 0
