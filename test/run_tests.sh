#!/bin/bash
# Usage: ./run_tests.sh [dbu_file] [ucom_to_csv_file]

# Get script directory
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
echo "Script directory: $script_dir"

# Get arguments or defaults
dbu_file="${1:-mobile.dbu}"
ucom_to_csv_file="${2:-../build/ucom_to_csv/ucom_to_csv}"

echo "DBU file: $dbu_file"
echo "ucom_to_csv file: $ucom_to_csv_file"

# Detect Python command
python_cmd=""
if command -v python &>/dev/null; then
    python_cmd="python"
elif command -v python3 &>/dev/null; then
    python_cmd="python3"
elif command -v py &>/dev/null; then
    python_cmd="py"
else
    echo "Failed to determine Python command name"
    exit 1
fi

echo "Python: using $python_cmd"

# Create Python virtual environment
echo "Creating Python virtual environment. Please wait..."
$python_cmd -m venv .venv
source .venv/bin/activate

# Install requirements
echo "Installing requirements. Please wait..."
$python_cmd -m pip install -r "$script_dir/requirements.txt"

echo "***********************************************************************"
# Run unittest discovery on all test_*.py files
echo "Running unit tests on Python bindings (also tests associated C++ code)"
$python_cmd -m unittest discover -s "../ucom_decoder_py/tests" -p "test_*.py"

if [ $? -ne 0 ]; then
    echo "Unit tests failed... exiting"
    fail=1
fi

if [ -z "$fail" ]; then 
    echo "***********************************************************************"
    echo Running ucom_to_csv test with: $python_cmd "$script_dir/test_csv.py" -u "$dbu_file%" -c "$ucom_to_csv_file"
    # Run the Python script
    $python_cmd "$script_dir/test_csv.py" -u "$dbu_file" -c "$ucom_to_csv_file"

    if [ $? -ne 0 ]; then
        echo "ucom_to_csv test failed... exiting"
        fail=1
    fi
   echo "***********************************************************************"
fi



# Final status
if [ -z "$fail" ]; then
    echo "Result: All tests passed."
else
    echo "Result: Test(s) failed."
fi

echo "***********************************************************************"
echo "Removing virtual environment..."

# Deactivate and remove virtual environment
deactivate
rm -rf .venv

echo Done.
echo "***********************************************************************"

if [ -z "$fail" ]; then
    exit 0
else
    exit 1
fi