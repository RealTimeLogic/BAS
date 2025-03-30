# This script generates a secret key for the TPM. 
# Refer to the documentation for detailed usage and security considerations.
#  https://realtimelogic.com/ba/examples/MakoServer/readme.html#TPM
#
# The generated key is stored in src/NewEncryptionKey.h.
#
# To use the generated key in your project, compile with the flag:
#  -DNewEncryptionKey

import os
import random

# Determine the script's directory and the target path in 'src'
scriptDir = os.path.dirname(os.path.abspath(__file__))
outputDir = os.path.join(scriptDir, "src")
headerFile = os.path.join(outputDir, "NewEncryptionKey.h")

keyLength = 512  # 512-byte encryption key

def generateRandomKey(length):
    return [random.randint(0, 255) for _ in range(length)]

def formatKeyArray(key):
    formatted = ",\n    ".join(", ".join(f"0x{byte:02x}" for byte in key[i:i+10]) for i in range(0, len(key), 10))
    return f"static const U8 ENCRYPTIONKEY[] = {{\n    {formatted}\n}};"

def generateHeaderFile():
    key = generateRandomKey(keyLength)
    content = f"""/* TPM main secret key embedded in the binary.
   Change before compiling. The key can be any length,
   but must be > 255.
*/

{formatKeyArray(key)}
"""
    with open(headerFile, "w") as f:
        f.write(content)
    print(f"{headerFile} generated successfully.")

if __name__ == "__main__":
    generateHeaderFile()
