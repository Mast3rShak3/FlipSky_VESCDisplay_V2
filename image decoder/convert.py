import re

# Read your header file
with open("input_image.h", "r") as f:
    data = f.read()

# Find all hex values (0x..)
hex_values = re.findall(r"0x[0-9A-Fa-f]{2}", data)

# Convert to bytes
byte_data = bytes(int(h, 16) for h in hex_values)

# Save to PNG
with open("output_image.png", "wb") as f:
    f.write(byte_data)


print("Image saved as output_image.png")
