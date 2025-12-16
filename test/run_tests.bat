@echo off
set "script_dir=%~dp0"
echo "Script dir: %script_dir%""
set dbu_file=%~f1
REM if [%dbu_file%] == [] set "dbu_file=%script_dir%mobile.dbu"
if [%dbu_file%] == [] set "dbu_file=mobile.dbu"
echo "DBU file: %dbu_file%"
set ucom_to_csv_file=%~f2
if [%ucom_to_csv_file%] == [] set ucom_to_csv_file="..\out\build\x64-release\ucom_to_csv\ucom_to_csv.exe"
echo "ucom_to_csv file: %ucom_to_csv_file%""
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
%python_cmd% -m unittest discover -s ..\ucom_decoder_py\tests -p "test_*.py"

if errorlevel 1 (
    echo Test failed... exiting
    set fail=1
    goto :end
)
echo ***********************************************************************


set "test_script=%script_dir%test_csv.py"
echo ***********************************************************************
echo Testing ucom_to_csv (C++)
echo Running ucom_to_csv test with: %python_cmd% "%test_script%" -u "%dbu_file%" -c %ucom_to_csv_file%
%python_cmd% "%test_script%" -u "%dbu_file%" -c %ucom_to_csv_file%

if errorlevel 1 (
    echo "ucom_to_csv test failed... exiting"
    set fail=1
    goto :end
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

