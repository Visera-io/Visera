@ECHO off
set VISERA_ROOT_DIR=%~dp0
set VISERA_CACHE_DIR=%VISERA_ROOT_DIR%Cache
set VISERA_SCRIPTS_DIR=%VISERA_ROOT_DIR%Scripts
set VISERA_PYTHON_DIR=%VISERA_ROOT_DIR%Python
set VISERA_PYTHON_VERSION=3.10.15
set VISERA_PYTHON_INTERPRETER_DIR=%VISERA_PYTHON_DIR%\Python-%VISERA_PYTHON_VERSION%

call python %VISERA_SCRIPTS_DIR%\CLI.py

@REM @REM Check local Python environment
@REM if not exist %VISERA_PYTHON_INTERPRETER_DIR%\python.bat (
@REM     @REM Download CPython
@REM     mkdir %VISERA_CACHE_DIR%\
@REM     mkdir %VISERA_CACHE_DIR%\Downloads\
@REM     if not exist %VISERA_CACHE_DIR%\Downloads\Python.tar.xz (
@REM         @REM Check cache to accelerate reinstallation.
@REM         call curl -o %VISERA_CACHE_DIR%\Downloads\Python.tar.xz https://www.python.org/ftp/python/%VISERA_PYTHON_VERSION%/Python-%VISERA_PYTHON_VERSION%.tar.xz
@REM         call tar -xf %VISERA_CACHE_DIR%\Downloads\Python.tar.xz -C %VISERA_PYTHON_DIR%
@REM     )

@REM     @REM Install CPython
@REM     call %VISERA_PYTHON_INTERPRETER_DIR%\PCBuild\build.bat
    
@REM     @REM Install Pip
@REM     call %VISERA_PYTHON_INTERPRETER_DIR%\python -m ensurepip --upgrade

@REM     @REM Install Packages
@REM     call %VISERA_PYTHON_INTERPRETER_DIR%\python -m pip install --upgrade pip
@REM     call %VISERA_PYTHON_INTERPRETER_DIR%\python -m pip install requests

@REM     @REM Clean Cache
@REM     attrib -r %VISERA_CACHE_DIR%\Downloads\Python.tar.xz
@REM     del %VISERA_CACHE_DIR%\Downloads\Python.tar.xz
@REM )

@REM call %VISERA_PYTHON_INTERPRETER_DIR%\python %VISERA_SCRIPTS_DIR%\CLI.py