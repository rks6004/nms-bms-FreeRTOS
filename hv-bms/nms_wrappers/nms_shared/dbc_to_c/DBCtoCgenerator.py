import re
import os
import glob

# Directory containing .dbc files (current directory)
dbc_dir = './dbc_files'
generated_code_dir = './generated_code'

def swap_instances(filename, suffix):
    with open(filename, 'r', encoding='utf-8') as file:
        file_as_string = file.read()

    # Use word boundaries to match function names everywhere
    pack = re.sub(r"\bpack_message\b", f"pack_message_{suffix.lower()}", file_as_string)
    unpack = re.sub(r"\bunpack_message\b", f"unpack_message_{suffix.lower()}", pack)
    message_dlc = re.sub(r"\bmessage_dlc\b", f"message_dlc_{suffix.lower()}", unpack)

    message_dlc = message_dlc.replace(f'#include "{suffix}.h"', f'#include "{suffix}_CAN.h"')

    with open(filename, 'w', encoding='utf-8') as file:
        file.write(message_dlc)

if __name__ == "__main__":
    dbc_files = glob.glob(os.path.join(dbc_dir, '*.dbc'))
    for dbc_path in dbc_files:
        base = os.path.basename(dbc_path)
        name, _ = os.path.splitext(base)
        # Run dbcc commands
        os.system(f'./program/dbcc -u -k -s -t -o {generated_code_dir} {dbc_path}')
        os.system(f'./program/dbcc -j -o {generated_code_dir} {dbc_path}')
        # Generated .c, .h, and .json files
        c_file = os.path.join(generated_code_dir, f'{name}.c')
        h_file = os.path.join(generated_code_dir, f'{name}.h')
        json_file = os.path.join(generated_code_dir, f'{name}.json')
        # Replace function names and file name if files exist
        if os.path.exists(c_file):
            swap_instances(c_file, name)
            new_c_file = os.path.join(generated_code_dir, f'{name}_CAN.c')
            os.rename(c_file, new_c_file)
        if os.path.exists(h_file):
            swap_instances(h_file, name)
            new_h_file = os.path.join(generated_code_dir, f'{name}_CAN.h')
            os.rename(h_file, new_h_file)
        if os.path.exists(json_file):
            new_json_file = os.path.join(generated_code_dir, f'{name}_CAN.json')
            os.rename(json_file, new_json_file)
