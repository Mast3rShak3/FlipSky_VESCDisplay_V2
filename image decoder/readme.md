# PNG ↔ C Header Converter

A Python utility for converting PNG images to C header files and back. This tool is particularly useful for embedded systems development where you need to include image data directly in your code.

## Features

- **PNG to .H**: Convert PNG files to C arrays with raw binary data
- **.H to PNG**: Extract PNG data from C header files back to image files
- **PROGMEM support**: Generated arrays include `PROGMEM` keyword for Arduino/embedded use
- **Customizable formatting**: Configure array names and bytes per line
- **Data validation**: Checks PNG signatures when converting back to images

## Requirements

- Python 3.6 or higher
- No additional libraries required (uses only standard library)

## Installation

1. Download `covert.py`
2. Make sure Python 3 is installed on your system
3. No additional setup required

## Usage

### Running the Program

```bash
python covert.py
```

### Mode 1: PNG to .H (Image to Header)

Convert a PNG file to a C header array:

```
=== PNG ↔ C Header Converter ===

Select conversion mode:
1. PNG to .H (PNG file → C header)
2. .H to PNG (C header → PNG file)

Enter choice (1 or 2): 1

--- PNG to .H Mode ---
Input PNG file (path): my_image.png
Output header file (e.g. background_image.h): my_image.h
C array name (default: background_image): my_image
Bytes per line in output (default 16): 16
```

**Generated output example:**
```c
// array size is 2048
static const unsigned char my_image[] PROGMEM = {
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
  0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x08, 0x02, 0x00, 0x00, 0x00, 0x90, 0x77, 0x53,
  // ... more data ...
};
```

### Mode 2: .H to PNG (Header to Image)

Convert a C header array back to a PNG file:

```
Enter choice (1 or 2): 2

--- .H to PNG Mode ---
Input header file (.h): my_image.h
Output PNG file (e.g. output.png): recovered_image.png
```

## Input/Output Formats

### PNG to .H
- **Input**: Any valid PNG file
- **Output**: C header file (.h) containing:
  - Size comment
  - `static const unsigned char` array declaration
  - `PROGMEM` keyword for embedded systems
  - Hex-formatted byte data

### .H to PNG
- **Input**: C header file containing hex array data
- **Output**: Standard PNG image file
- **Supported formats**: Any C array with `0x##` hex values

## Command Line Tips

### File Paths
- Drag and drop files into terminal for automatic path insertion
- Use quotes around paths with spaces: `"C:\My Files\image.png"`
- Relative paths work: `./images/logo.png`

### Array Naming
- Use valid C identifier names (letters, numbers, underscores)
- Avoid spaces and special characters
- Examples: `logo_data`, `background_img`, `sprite_01`

## Use Cases

### Embedded Systems
```c
#include "my_image.h"

// Display the image
display_png(my_image, sizeof(my_image));
```

### Arduino Projects
```c
#include <Arduino.h>
#include "logo.h"

void setup() {
  // Use the image data stored in PROGMEM
  showImage(logo, sizeof(logo));
}
```

### Testing and Verification
1. Convert PNG to header for your project
2. Convert header back to PNG to verify data integrity
3. Compare original and recovered images

## Troubleshooting

### Common Issues

**"No hex values found"**
- Make sure the .h file contains a C array with `0x##` format
- Check that the file isn't corrupted or empty

**"Doesn't start with PNG signature"**
- The header data might not be a valid PNG file
- You can choose to continue anyway, but the output may not be a valid image

**"File not found"**
- Check the file path is correct
- Make sure the file exists and you have read permissions
- Try using absolute paths instead of relative paths

**"Invalid number" for bytes per line**
- Enter a positive integer (recommended: 8, 12, 16, or 20)
- Higher numbers create wider lines, lower numbers create more compact output

### File Permissions
- Ensure you have read access to input files
- Ensure you have write access to the output directory
- On Unix systems, you may need to use `chmod` if needed

## Technical Details

### Data Format
- Raw PNG file data is preserved byte-for-byte
- No compression or conversion of the image itself
- The entire PNG file structure is included in the array
- Compatible with any PNG decoder library

### Memory Usage
- The tool loads the entire file into memory
- For very large images, ensure sufficient RAM is available
- Generated arrays consume the same amount of memory as the original file

### Compatibility
- Works with all PNG variants (grayscale, RGB, RGBA, indexed)
- Compatible with any C compiler
- `PROGMEM` keyword can be removed if not using Arduino/AVR

## Example Workflow

1. **Prepare your image**
   ```bash
   # Start with any PNG file
   my_logo.png
   ```

2. **Convert to header**
   ```bash
   python covert.py
   # Choose option 1, follow prompts
   # Output: my_logo.h
   ```

3. **Include in your project**
   ```c
   #include "my_logo.h"
   // Use my_logo array in your code
   ```

4. **Verify (optional)**
   ```bash
   python covert.py
   # Choose option 2 to convert back and verify
   # Output: verified_logo.png
   ```

## License

This tool is provided as-is for educational and development purposes. Feel free to modify and distribute as needed for your projects.
