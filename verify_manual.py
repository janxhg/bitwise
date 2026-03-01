import os
import subprocess
import sys

# Script to verify the Bitwise compiler manually
BW_BIN = "build/Release/bwc.exe"
TEST_FILE = "tests/demos/snake_native.bw"
OUT_ASM = "build/snake_demo.asm"

if not os.path.exists(BW_BIN):
    print(f"Error: Binary not found at {BW_BIN}")
    sys.exit(1)

print("--- Step 1: Compiling Bitwise Source to Assembly ---")
try:
    with open(OUT_ASM, 'w') as f:
        result = subprocess.run([BW_BIN, TEST_FILE], stdout=f, stderr=subprocess.PIPE, text=True)
    
    if result.returncode == 0:
        print(f"Successfully compiled {TEST_FILE} to {OUT_ASM}")
        print("\n--- Step 2: Extracting First 20 lines of produced Assembly ---")
        with open(OUT_ASM, 'r') as f:
            lines = f.readlines()
            for line in lines[:20]:
                print(f"  {line.strip()}")
        print("\n--- Step 3: Verification of the Internal Pipeline ---")
        print(result.stderr)
    else:
        print(f"Compilation Failed: {result.stderr}")
except Exception as e:
    print(f"Error: {e}")
