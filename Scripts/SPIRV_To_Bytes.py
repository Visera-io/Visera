import sys

def spirv_to_header(input_file, output_file, array_name):
    with open(input_file, "rb") as f:
        spirv_bytes = f.read()

    with open(output_file, "w") as f:
        f.write(f"const unsigned char {array_name}[] = {{\n")
        for i, byte in enumerate(spirv_bytes):
            f.write(f" 0x{byte:02x},")
            if (i + 1) % 12 == 0:  # Wrap lines every 12 bytes
                f.write("\n")
        f.write("\n};\n")
        f.write(f"const size_t {array_name}_size = sizeof({array_name});\n")

# Usage: python spirv_to_header.py vertex.spv vertex_shader.h
if __name__ == "__main__":
    spirv_to_header(sys.argv[1], sys.argv[2], "vertexShaderSPV")
