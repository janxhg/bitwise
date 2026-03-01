import os
import subprocess
import argparse
import sys

def run_test(bin_path, test_file):
    print(f"Running test: {test_file}")
    
    # Check if we expect this test to fail
    expect_fail = False
    try:
        with open(test_file, 'r', encoding='utf-8') as f:
            content = f.read()
            if "// EXPECT_ERROR" in content:
                expect_fail = True
    except:
        pass

    try:
        result = subprocess.run([bin_path, test_file], capture_output=True, text=True, timeout=10)
        
        passed = False
        if expect_fail:
            passed = (result.returncode != 0)
            status = "PASS (Expected Failure)" if passed else "FAIL (Expected failure but succeeded)"
        else:
            passed = (result.returncode == 0)
            status = "PASS" if passed else "FAIL"

        if passed:
            print(f"  [{status}] {test_file}")
            return True
        else:
            print(f"  [{status}] {test_file}")
            if not expect_fail:
                print(f"  Output: {result.stdout}")
                print(f"  Error: {result.stderr}")
            return False
    except Exception as e:
        print(f"  [ERROR] {test_file}: {str(e)}")
        return False

def main():
    parser = argparse.ArgumentParser(description="Bitwise Compiler Test Runner")
    parser.add_argument("--bin", required=True, help="Path to bwc executable")
    args = parser.parse_args()

    bin_path = os.path.abspath(args.bin)
    if not os.path.exists(bin_path):
        print(f"Error: Executable not found at {bin_path}")
        sys.exit(1)

    test_dirs = ["tests/validation", "tests/demos"]
    success = True

    for test_dir in test_dirs:
        full_dir = os.path.join(os.getcwd(), test_dir)
        if not os.path.exists(full_dir):
            continue
        
        for file in os.listdir(full_dir):
            if file.endswith(".bw"):
                test_file = os.path.join(test_dir, file)
                if not run_test(bin_path, test_file):
                    success = False

    if success:
        print("\nAll tests passed!")
        sys.exit(0)
    else:
        print("\nSome tests failed.")
        sys.exit(1)

if __name__ == "__main__":
    main()
