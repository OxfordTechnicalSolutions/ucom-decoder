@echo off
REM Determine Python command
set python_cmd=
set fail=
python --version >nul 2>&1
if errorlevel 1 goto :NotPython
set python_cmd=python
goto :FoundPython

:NotPython
python3 --version >nul 2>&1
if errorlevel 1 goto :NotPython3
set python_cmd=python3
goto :FoundPython

:NotPython3
py --version >nul 2>&1
if errorlevel 1 goto :NotPy
set python_cmd=py
goto :FoundPython

:NotPy
echo Failed to determine Python command name
exit /b 1

:FoundPython
if [%python_cmd%] == [] (
    echo Python command name not defined: error in batch file
    exit /b 1
)
echo Python: using %python_cmd%

REM Setup Python virtual environment
echo Creating Python virtual environment. Please wait...
%python_cmd% -m venv .venv
call .venv/Scripts/activate.bat

REM Installing requirements
echo Installing requirements. Please wait...
set "requirements_file=%script_dir%requirements.txt"
%python_cmd% -m pip install -r "%requirements_file%"

REM Run unittest discovery on all test_*.py files
echo ***********************************************************************
echo "Running unit tests on Python bindings (also tests associated C++ code)"
%python_cmd% -m unittest discover -s . -p "test_*.py"

if errorlevel 1 (
    echo Test failed... exiting
    set fail=1
)

echo ***********************************************************************

:end

if [%fail%] == [] (
    echo Result: All tests passed.
) else (
    echo Result: Test^(s^) failed.
)

echo ***********************************************************************
echo Removing virtual environment...
REM Remove Python virtual environment
call deactivate
rmdir .venv /s /q
echo Done.
echo ***********************************************************************

if [%fail%] == [] (
    exit /b 0
) else (
    exit /b 1
)

