#!/usr/bin/env python3
"""
png_header_converter.py
Bidirectional converter between PNG files and C header arrays.
- PNG to .H: Converts PNG file to raw bytes C array
- .H to PNG: Extracts PNG data from C header and saves as PNG file
"""

import os
import sys
import re

def convert_png_to_header(input_path, output_path, array_name="background_image", bytes_per_line=16):
    """Convert PNG file to C header file with raw PNG data."""
    
    # Validate input file
    if not os.path.exists(input_path):
        raise FileNotFoundError(f"Input file not found: {input_path}")
    
    # Read the entire PNG file as binary data
    with open(input_path, 'rb') as f:
        png_data = f.read()
    
    print(f"PNG file loaded: {len(png_data)} bytes")
    
    # Generate header content
    array_name_upper = array_name.upper()
    header_lines = [
        f"// array size is {len(png_data)}",
        f"static const unsigned char {array_name}[] PROGMEM = {{",
    ]
    
    # Format bytes as hex, grouped by lines
    for i in range(0, len(png_data), bytes_per_line):
        line_bytes = []
        end_index = min(i + bytes_per_line, len(png_data))
        
        for j in range(i, end_index):
            line_bytes.append(f"0x{png_data[j]:02x}")
        
        line = "  " + ", ".join(line_bytes)
        if end_index < len(png_data):
            line += ","
        header_lines.append(line)
    
    header_lines.append("};")
    
    # Write to file
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write('\n'.join(header_lines))
    
    return len(png_data)

def convert_header_to_png(input_path, output_path):
    """Convert C header file back to PNG file."""
    
    # Validate input file
    if not os.path.exists(input_path):
        raise FileNotFoundError(f"Input file not found: {input_path}")
    
    # Read the header file
    with open(input_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    print(f"Header file loaded: {input_path}")
    
    # Extract hex values using regex
    # Look for patterns like 0x89, 0xFF, etc.
    hex_pattern = r'0x([0-9a-fA-F]{2})'
    hex_matches = re.findall(hex_pattern, content)
    
    if not hex_matches:
        raise ValueError("No hex values found in header file. Make sure it contains a C array with 0x__ format.")
    
    # Convert hex strings to bytes
    byte_data = bytearray()
    for hex_str in hex_matches:
        byte_data.append(int(hex_str, 16))
    
    print(f"Extracted {len(byte_data)} bytes from header")
    
    # Verify it looks like PNG data (check PNG signature)
    if len(byte_data) < 8 or byte_data[:4] != b'\x89PNG':
        print("Warning: Data doesn't start with PNG signature. This might not be a valid PNG file.")
        response = input("Continue anyway? (y/n): ").strip().lower()
        if response != 'y':
            print("Operation cancelled.")
            return 0
    
    # Write binary data to PNG file
    with open(output_path, 'wb') as f:
        f.write(byte_data)
    
    print(f"PNG file saved: {output_path}")
    return len(byte_data)

def main():
    """Main interactive function."""
    print("=== PNG ↔ C Header Converter ===\n")
    
    # Choose conversion direction
    print("Select conversion mode:")
    print("1. PNG to .H (PNG file → C header)")
    print("2. .H to PNG (C header → PNG file)")
    
    choice = input("\nEnter choice (1 or 2): ").strip()
    
    if choice == "1":
        # PNG to Header
        print("\n--- PNG to .H Mode ---")
        
        input_path = input("Input PNG file (path): ").strip().strip('"')
        if not input_path:
            print("Error: No input file specified.")
            return
        
        output_path = input("Output header file (e.g. background_image.h): ").strip().strip('"')
        if not output_path:
            print("Error: No output file specified.")
            return
        
        array_name = input("C array name (default: background_image): ").strip()
        if not array_name:
            array_name = "background_image"
        
        bytes_per_line_input = input("Bytes per line in output (default 16): ").strip()
        bytes_per_line = 16
        if bytes_per_line_input:
            try:
                bytes_per_line = int(bytes_per_line_input)
                if bytes_per_line <= 0:
                    bytes_per_line = 16
            except ValueError:
                print("Invalid number, using default 16")
        
        # Convert PNG to header
        try:
            array_size = convert_png_to_header(input_path, output_path, array_name, bytes_per_line)
            print(f"\n✅ Header saved to {output_path}")
            print(f"Array size: {array_size} bytes")
            print(f"Contains: Raw PNG file data")
            
        except FileNotFoundError as e:
            print(f"Error: {e}")
        except Exception as e:
            print(f"Error processing file: {e}")
    
    elif choice == "2":
        # Header to PNG
        print("\n--- .H to PNG Mode ---")
        
        input_path = input("Input header file (.h): ").strip().strip('"')
        if not input_path:
            print("Error: No input file specified.")
            return
        
        output_path = input("Output PNG file (e.g. output.png): ").strip().strip('"')
        if not output_path:
            print("Error: No output file specified.")
            return
        
        # Convert header to PNG
        try:
            file_size = convert_header_to_png(input_path, output_path)
            print(f"\n✅ PNG file saved to {output_path}")
            print(f"File size: {file_size} bytes")
            
        except FileNotFoundError as e:
            print(f"Error: {e}")
        except Exception as e:
            print(f"Error processing file: {e}")
    
    else:
        print("Invalid choice. Please enter 1 or 2.")

if __name__ == "__main__":
    main()
