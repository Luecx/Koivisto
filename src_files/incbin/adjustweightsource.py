from pathlib import Path
import sys
import os

SOURCE_DIRECTORY = Path(__file__).resolve().parent.parent
WEIGHTS_FILE     = SOURCE_DIRECTORY / "weights.txt"

if sys.argv[1] == "incbin":
    with open(WEIGHTS_FILE, "w") as weights_file:
        weights_file.write("#include \"incbin/incbin.h\"\n")
        weights_file.write("#define INCBIN_STYLE INCBIN_STYLE_CAMEL\n")
        weights_file.write("INCBIN(Eval, EVALFILE);")
else:
    BINARY_WEIGHTS = sys.argv[1]
    with open(WEIGHTS_FILE, "w") as weights_file:
        weights_file.write("#include <cstdint>\n")
        weights_file.write("uint8_t gEvalData[]{")
        with open(BINARY_WEIGHTS, "rb") as binary_weights:
            byte = binary_weights.read(1)
            while byte != b"":
                weights_file.write(f"0x{byte.hex()},")
                # Do stuff with byte.
                byte = binary_weights.read(1)
        weights_file.write("};")

print(SOURCE_DIRECTORY)
print(WEIGHTS_FILE)