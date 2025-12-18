#!/bin/bash
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
$python_cmd -m pip install -r "requirements.txt"

echo "***********************************************************************"
# Run unittest discovery on all test_*.py files
echo "Running unit tests on Python bindings (also tests associated C++ code)"
$python_cmd -m unittest discover -s . -p "test_*.py"

if [ $? -ne 0 ]; then
    echo "Unit tests failed... exiting"
    fail=1
fi

echo "***********************************************************************"

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