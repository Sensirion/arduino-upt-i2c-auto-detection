import os
import shutil

ARDUINO_FILE_EXTENSION = ".ino"
TARGET_FILE_EXTENSION = ".cpp"
EXAMPLE_FOLDER = "examples"

# Create a .cpp file for all the .ino files in the examples dir (CAUTION: will overwrite existing)
print("PRE_SCRIPT: Copying .ino file contents to .cpp files.")
for e in os.listdir(EXAMPLE_FOLDER):
    if os.path.isdir(f"{EXAMPLE_FOLDER}/{e}") and os.path.isfile(f"{EXAMPLE_FOLDER}/{e}/{e}{ARDUINO_FILE_EXTENSION}"):
        arduino_file =f"{EXAMPLE_FOLDER}/{e}/{e}{ARDUINO_FILE_EXTENSION}"
        cpp_file = f"{EXAMPLE_FOLDER}/{e}/{e}{TARGET_FILE_EXTENSION}"
        shutil.copyfile(arduino_file, cpp_file)
        print(f'\tcopied {arduino_file} to {cpp_file}')
print("\t>> Done.")
