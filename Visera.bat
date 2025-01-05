@ECHO off
set VISERA_ROOT_DIR=%~dp0
set VISERA_CACHE_DIR=%VISERA_ROOT_DIR%Cache
set VISERA_PYTHON_DIR=%VISERA_ROOT_DIR%Python
set VISERA_SCRIPTS_DIR=%VISERA_ROOT_DIR%Scripts

if "%1"=="" (
    @REM Check local Python environment
    set VISERA_PYTHON_VERSION="3.10.15"
    if not exist %VISERA_PYTHON_DIR%\python.bat (
        @REM Download CPython
        mkdir %VISERA_CACHE_DIR%\
        mkdir %VISERA_CACHE_DIR%\Downloads\
        if not exist %VISERA_CACHE_DIR%\Downloads\Python.tar.xz (
            @REM Check cache to accelerate reinstallation.
            call curl -o %VISERA_CACHE_DIR%\Downloads\Python.tar.xz https://www.python.org/ftp/python/%VISERA_PYTHON_VERSION%/Python-%VISERA_PYTHON_VERSION%.tar.xz
            call tar -xf %VISERA_CACHE_DIR%\Downloads\Python.tar.xz -C %VISERA_ROOT_DIR%
        )
        :: Remove read-only attribute if set
        attrib -r %VISERA_ROOT_DIR%Python-%VISERA_PYTHON_VERSION%
        ren %VISERA_ROOT_DIR%Python-%VISERA_PYTHON_VERSION% "Python"

        @REM Install CPython
        call %VISERA_PYTHON_DIR%\PCBuild\build.bat
        
        @REM Install Pip
        call %VISERA_PYTHON_DIR%\python -m ensurepip --upgrade

        @REM Install Packages
        call %VISERA_PYTHON_DIR%\python -m pip install --upgrade pip
        call %VISERA_PYTHON_DIR%\python -m pip install requests

        @REM Clean Cache
        attrib -r %VISERA_CACHE_DIR%\Downloads\Python.tar.xz
        del %VISERA_CACHE_DIR%\Downloads\Python.tar.xz
    )
    echo %VISERA_PYTHON_DIR%\python
    call %VISERA_PYTHON_DIR%\python %VISERA_SCRIPTS_DIR%\python_CLI.py
) else (
    @REM Transfer all arguments to Python CLI
    call %VISERA_PYTHON_DIR%\python %VISERA_SCRIPTS_DIR%\python_CLI.py %*
)