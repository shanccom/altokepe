@echo off
REM =============================================================
REM Script de build para Server y Clientes (MinGW + Qt6)
REM Intenta autodetectar Qt y MinGW si no se proveen manualmente.
REM Puedes forzar rutas exportando QT_ROOT y MINGW_BIN antes de llamar.
REM =============================================================

setlocal EnableDelayedExpansion

REM Si el usuario ya definió variables fuera, las respetamos.
if not defined QT_ROOT (
  echo [INFO] QT_ROOT no definido, intentando autodeteccion en C:\Qt\6.*\mingw_64
  for /d %%D in (C:\Qt\6.*) do (
    if exist "%%D\mingw_64\bin\Qt6Core.dll" (
      set CANDIDATE=%%D\mingw_64
      set QT_ROOT=!CANDIDATE!
    )
  )
)

if not defined QT_ROOT (
  echo [ERROR] No se pudo detectar QT_ROOT. Define la variable manualmente.
  echo Ejemplo: set QT_ROOT=C:\Qt\6.7.2\mingw_64
  exit /b 1
)

if not defined MINGW_BIN (
  echo [INFO] MINGW_BIN no definido, buscando en C:\Qt\Tools\mingw*\bin
  for /d %%M in (C:\Qt\Tools\mingw*64) do (
    if exist "%%M\bin\g++.exe" (
      set MINGW_BIN=%%M\bin
    )
  )
)

if not defined MINGW_BIN (
  echo [WARN] No se encontro MinGW dentro de Qt Tools. Si usas otro MinGW ajusta MINGW_BIN.
)

echo [INFO] Usando QT_ROOT=%QT_ROOT%
echo [INFO] Usando MINGW_BIN=%MINGW_BIN%

set PATH=%MINGW_BIN%;%QT_ROOT%\bin;%PATH%
endlocal & set PATH=%PATH% & set QT_ROOT=%QT_ROOT% & set MINGW_BIN=%MINGW_BIN%

echo.
echo ===============================================
echo  Build Altokepe (Qt6 + MinGW)
echo  Generator: MinGW Makefiles
echo ===============================================
echo.

REM Directorio raíz del proyecto (este script está en la raíz)
set PROJ_ROOT=%~dp0
cd /d "%PROJ_ROOT%"

REM Función simple para build por carpeta
REM %1 = subcarpeta relativa al root
REM %2 = nombre amigable
call :build_dir server ServidorAltokepe
call :build_dir clients\recepcionista ClienteRecepcionista
call :build_dir clients\cocina\manager_chef ManagerChefClient
call :build_dir clients\cocina\manager_estacion ManagerEstacionClient
call :build_dir clients\ranking ClienteRanking

echo.
echo ===============================================
echo  Build finalizado. Ejecutables (buscar en build/ o build/Release segun caso)
echo  Servidor: server\build\ServidorAltokepe.exe
echo  Cliente Recepcionista: clients\recepcionista\build\ClienteRecepcionista.exe
echo  Manager Chef: clients\cocina\manager_chef\build\ManagerChefClient.exe
echo  Manager Estacion: clients\cocina\manager_estacion\build\ManagerEstacionClient.exe (mismo nombre interno por ahora)
echo  Ranking: clients\ranking\build\ClienteRanking.exe
echo ===============================================
echo.
echo Para ejecutar primero inicia el servidor y luego los clientes.
goto :eof

:build_dir
setlocal
set SUBDIR=%1
set NAME=%2
echo.
echo ---- [%NAME%] ----
if not exist "%SUBDIR%" (
  echo [SKIP] Carpeta no encontrada: %SUBDIR%
  endlocal & goto :eof
)
cd /d "%PROJ_ROOT%%SUBDIR%"
if not exist build mkdir build
echo Configurando CMake (%NAME%)...
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="%QT_ROOT%"
if errorlevel 1 (
  echo [ERROR] Fallo configuracion CMake en %SUBDIR%
  endlocal & goto :eof
)
echo Compilando (%NAME%)...
cmake --build build -j
if errorlevel 1 (
  echo [ERROR] Fallo compilacion en %SUBDIR%
  endlocal & goto :eof
)
echo [OK] %NAME% compilado.
endlocal
cd /d "%PROJ_ROOT%"
goto :eof
