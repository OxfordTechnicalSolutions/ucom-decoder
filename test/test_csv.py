from ucomgen.ucom_definitions import UcomTestMessage
from ucomgen.dbx import DBULoader
from ucomgen.generator import UcomGenerator
import argparse
import sys
import pandas as pd
import subprocess
import os
import shutil
import glob
import re
   
def parse_args() -> tuple[str,str]:
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "-u", "--dbu", type=str, required=False, help="DBU file to test"
    )
    parser.add_argument(
        "-c", "--ucom_to_csv", type=str, required=False, help="Path to ucom_to_csv.exe"
    )

    args = parser.parse_args()
    
    return args.dbu, args.ucom_to_csv

def compare_csv_files(file1, file2):
    """Compares two CSV files to check if their contents are identical
    Parameters:
        file1 (str): Filename of first CSV file
        file2 (str): Filename of second CSV file
    Returns:
        bool: True if the files match, False otherwise
    """
    # Read the CSV files into DataFrames
    try:
        df1 = pd.read_csv(file1)
        df2 = pd.read_csv(file2)
    except Exception as e:
        print(f"Error comparing CSVs: {e.message if hasattr(e, 'message') else e}")
        return False

    # Check if columns match
    if list(df1.columns) != list(df2.columns):
        print("The columns do not match!")
        print("File 1 columns:", df1.columns.tolist())
        print("File 2 columns:", df2.columns.tolist())
        return False

    # print("Columns match. Comparing rows...")

    # Compare the data

    # Check if they are identical
    if df1.equals(df2):
        # print("The two CSV files have identical data.")
        return True
    else:
        # print("Differences found:")
        # Find rows in df1 not in df2
        diff1 = pd.concat([df1, df2]).drop_duplicates(keep=False)
        print(diff1)
        return False

def compare_message_data(messages : dict[int, UcomTestMessage]) -> bool:
    """Runs compare_csv_files() on all of the enabled UcomMessages in messages
    Expects the comparable CSV files to be named: data_<uid>.csv and output_<uid>.csv

    Returns:
        bool: True if all files match, False otherwise
    """

    message : UcomTestMessage
    success = True
    for _ , message in messages.items():
        if not message.enabled:
            continue
        if not compare_csv_files(f"test_data/data_{message.uid()}.csv", f"test_data/output_{message.uid()}.csv"):
            success = False

    return success

def run_ucom_to_csv(ucom_to_csv : str, dbu_path : str) -> tuple[bool, str]:
    """Runs ucom_to_csv on 'test_data/data.ucom to decode and generate CSV files
    
    Output in 'test_data'
    """
    print(f"Reading .dbu file: {dbu_path}")
    result = subprocess.run([ucom_to_csv, "-u", dbu_path, "-f", "test_data/data.ucom"],
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                            universal_newlines=True)
    
    with open('ucom_to_csv.log', 'wt') as f:
        f.write(result.stdout + result.stderr)

    # Regex to find folder name before "\output_"
    match = re.search(r'Creating output file:\s*([^\s\\]+)[\/\\]output_', result.stdout)

    folder_name = ''
    if match:
        folder_name = match.group(1)

    # Move the files from the time-stamped folder into 'test_data'
    if len(folder_name) == 0:
        return False, folder_name

    for name in glob.glob(f'{folder_name}/*.csv'):
        shutil.copy(name, f'test_data/{os.path.basename(name)}')
        os.remove(name)
    
    os.removedirs(folder_name)

    return result.returncode == 0, folder_name

def remove_old_files():
    """Removes any pre-existing auto-generated test files
    """
    for name in glob.glob('test_data/*.csv'):
        os.remove(name)
    
    files_to_remove = ['test_data/data.ucom', 'ucom_to_csv.log']
    for file in files_to_remove:
        if os.path.exists(file):
            os.remove(file)

    

if __name__ == '__main__':
    # dbu_path = "D:\\GitLab\\RD784\\251029_ucom_decoder_config_1921682513\\mobile.dbu"
    # dbu_path = 'D:/GitLab/RD784/code-review-251009/xdev-xbns-ccs/utils/dbu-dbs-generators/input/oxts.dbu'

    remove_old_files()

    dbu_path, ucom_to_csv_path = parse_args()
    if dbu_path is None:
        dbu_path = 'mobile.dbu'

    if ucom_to_csv_path is None:
        ucom_to_csv_path = "ucom_to_csv.exe"

    dbu_loader = DBULoader(dbu_path)
    if not dbu_loader.load():
        sys.exit(1)

    messages = dbu_loader.get_messages()

    if len(messages) == 0:
        print("FAIL: no messages extracted from DBU")
        sys.exit(1)

    # Generate UCOM data (packets and CSV files). Saved to file.
    gen = UcomGenerator()
    gen.generate_data(messages)

    # Run ucom_to_csv
    success = run_ucom_to_csv(ucom_to_csv_path, dbu_path)
    if not success:
        print("FAIL: Error creating CSV files with ucom_to_csv")

    if compare_message_data(messages):
        print("PASS: All CSV files match")
        sys.exit(0)
    else:
        print("FAIL: Non-matching CSV data found")
        sys.exit(1)
    


