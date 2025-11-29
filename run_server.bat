@echo off
REM =============================================================
REM Build y ejecución del Servidor Altokepe (Qt6 + MinGW)
REM Autodetecta Qt si no se definieron variables.
REM Puedes forzar: set QT_ROOT=... && set MINGW_BIN=...
REM =============================================================

setlocal EnableDelayedExpansion
if not defined QT_ROOT (
  echo [INFO] Buscando QT_ROOT en C:\Qt\6.*\mingw_64
  for /d %%D in (C:\Qt\6.*) do (
    if exist "%%D\mingw_64\bin\Qt6Core.dll" (
      set QT_ROOT=%%D\mingw_64
    )
  )
)
if not defined QT_ROOT (
  echo [ERROR] No se detecto QT_ROOT. Define manualmente antes de ejecutar.
  echo Ej: set QT_ROOT=C:\Qt\6.7.2\mingw_64
  pause
  exit /b 1
)

if not defined MINGW_BIN (
  echo [INFO] Buscando MINGW_BIN en C:\Qt\Tools\mingw*64\bin
  for /d %%M in (C:\Qt\Tools\mingw*64) do (
    if exist "%%M\bin\g++.exe" (
      set MINGW_BIN=%%M\bin
    )
  )
)
if not defined MINGW_BIN (
  echo [WARN] No se detecto MINGW_BIN. Si tienes otro MinGW ajusta variable.
)

echo [INFO] QT_ROOT=%QT_ROOT%
echo [INFO] MINGW_BIN=%MINGW_BIN%

set PATH=%MINGW_BIN%;%QT_ROOT%\bin;%PATH%
endlocal & set PATH=%PATH% & set QT_ROOT=%QT_ROOT% & set MINGW_BIN=%MINGW_BIN%

echo.
echo ===== Servidor Altokepe: Build (MinGW Makefiles) =====
set PROJ_ROOT=%~dp0
cd /d "%PROJ_ROOT%server"
REM Limpiar build anterior si el generador fue distinto (NMake vs MinGW)
if exist build\CMakeCache.txt (
  findstr /I "CMAKE_GENERATOR:INTERNAL=NMake Makefiles" build\CMakeCache.txt >nul
  if %errorlevel%==0 (
    echo [INFO] Detectado cache anterior con NMake Makefiles. Eliminando carpeta build...
    rmdir /S /Q build
  )
)

if not exist build mkdir build
echo Configurando CMake...
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="%QT_ROOT%" -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
  echo [ERROR] Fallo configuracion
  pause
  exit /b 1
)

echo Compilando...
cmake --build build -j --config Release
if errorlevel 1 (
  echo [ERROR] Fallo compilacion
  pause
  exit /b 1
)

echo Verificando ejecutable...
if exist build\ServidorAltokepe.exe (
  echo [OK] Ejecutable encontrado: build\ServidorAltokepe.exe
) else (
  echo [ERROR] No se encontro build\ServidorAltokepe.exe
  pause
  exit /b 1
)

echo Iniciando servidor (Ctrl+C para detener)...
echo -------------------------------------------------------------
build\ServidorAltokepe.exe
echo -------------------------------------------------------------
echo Servidor finalizado.
pause
exit /b 0
