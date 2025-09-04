# VESC MPH Dashboard

A professional ESP32-based dashboard for VESC motor controllers featuring MPH speed display, comprehensive telemetry monitoring, and custom typography designed for electric scooters, skateboards, and other personal electric vehicles.

![Dashboard Preview]([docs/dashboard_preview.jpg](https://github.com/Mast3rShak3/FlipSky_VESCDisplay_V2/blob/353afb5542e0324df9a31ebdf481d0cc4482ff85/imgs/background.png))
*Dashboard displaying real-time speed, battery status, and motor telemetry*

## ✨ Features

### 📊 **Real-Time Telemetry**
- **Speed Display**: Large, easy-to-read MPH readout with Jersey M54 sport font
- **Battery Monitoring**: Voltage display with color-coded status indicators
- **Temperature Monitoring**: Separate VESC controller and motor temperature readings
- **Power Consumption**: Real-time wattage calculation and display
- **Current Monitoring**: Both motor phase and battery input current

### 🎯 **Smart Monitoring**
- **Automatic Brightness**: Light sensor adjusts screen brightness based on ambient conditions
- **Warning System**: Color-coded alerts for high speed, temperature, and battery conditions
- **Trip Odometer**: Miles tracking with EEPROM storage for persistent data
- **Error Display**: Real-time VESC fault code monitoring

### 🎨 **Professional Interface**
- **Custom Typography**: Jersey M54 sport fonts for authentic racing aesthetic
- **Background Graphics**: Customizable PNG background and startup logo support
- **Color-Coded Status**: Intuitive green/yellow/red warning system
- **Flicker-Free Updates**: Smooth display updates without screen artifacts

### 🔧 **Developer Features**
- **Static Test Mode**: Built-in test data for development and debugging
- **Dual Unit Support**: Easy toggle between static test and live VESC data
- **Modular Code**: Clean, well-documented code structure for easy modification
- **Touch Ready**: Foundation prepared for future touch screen implementation

## 🛠️ Hardware Requirements

### **Required Components**
- **ESP32-2432S028R** display module (320x240 TFT with touch)
- **VESC-compatible motor controller** (Flipsky, Trampa, etc.)
- **Connecting wires** for UART communication

### **Supported VESC Controllers**
- Flipsky 4.12, 4.20, 6.6
- Trampa VESC 100/250, 100/500
- Benjamin Vedder original VESC
- Any VESC-compatible ESC with UART communication

### **Display Specifications**
- **Resolution**: 320x240 pixels
- **Controller**: ILI9341
- **Interface**: SPI
- **Touch**: Resistive (XPT2046)
- **Backlight**: PWM controlled via LDR

## 📋 Pin Configuration

### **UART Communication (ESP32 ↔ VESC)**
```
ESP32 Pin 22 (RX) → VESC TX
ESP32 Pin 27 (TX) → VESC RX
GND → GND
```

### **Display Interface (Built-in on ESP32-2432S028R)**
```
TFT_MISO: Pin 12
TFT_MOSI: Pin 13  
TFT_SCLK: Pin 14
TFT_CS: Pin 15
TFT_DC: Pin 2
TFT_RST: -1 (shared with ESP32 reset)
```

### **Sensors & Controls**
```
LDR_PIN: Pin 34 (Light sensor)
LCD_BACKLIGHT: Pin 21 (PWM brightness control)
TOUCH_CS: Pin 21 (Touch controller)
```

## 🚀 Installation

### **1. Arduino IDE Setup**
```bash
# Install Arduino IDE (latest version)
# Add ESP32 board package URL in preferences:
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

# Install ESP32 boards via Board Manager
# Select: ESP32 Dev Module
```

### **2. Required Libraries**
Install these libraries via Arduino Library Manager:
```
TFT_eSPI - Hardware-specific TFT library
FlickerFreePrint - Smooth text updates  
ComEVesc - VESC communication protocol
PNGdec - PNG image decoder
EEPROMAnything - Easy EEPROM storage
```

### **3. TFT_eSPI Configuration**
Configure `User_Setup.h` in TFT_eSPI library:
```cpp
#define ILI9341_DRIVER
#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST   -1
#define TOUCH_CS 21
#define SPI_FREQUENCY  27000000
```

### **4. Font Installation**
1. Download Jersey M54 font from [dafont.com](https://www.dafont.com/jersey-m54.font)
2. Convert to GFX format using [truetype2gfx](https://rop.nl/truetype2gfx/)
3. Generate these files:
   - `JerseyM54_82pt7b.h` (main speed display)
   - `JerseyM54_18pt7b.h` (data values)
   - `JerseyM54_14pt7b.h` (smaller data)
   - `Blockletter8pt7b.h` (labels)

### **5. Upload Process**
```bash
# 1. Clone this repository
git clone https://github.com/yourusername/VESC-MPH-Dashboard.git

# 2. Open main.ino in Arduino IDE
# 3. Select board: ESP32 Dev Module
# 4. Select correct COM port
# 5. Upload code
```

## ⚙️ Configuration

### **Motor Parameters**
Adjust these values in `main.ino` for your specific setup:
```cpp
#define MOTOR_POLES 30              // Motor pole count
#define WHEEL_DIAMETER_MM 246       // Wheel diameter in millimeters
#define GEAR_RATIO 1.0              // Gear reduction ratio
```

### **Warning Thresholds**
Customize warning levels for your system:
```cpp
#define HIGH_SPEED_WARNING_MPH 37   // Speed warning (MPH)
#define VESC_TEMP_WARNING1 50       // VESC temp warning (°C)
#define VESC_TEMP_WARNING2 80       // VESC temp critical (°C)
#define MOTOR_TEMP_WARNING1 80      // Motor temp warning (°C)
#define BATTERY_WARNING_HIGH 67.2   // High voltage warning
#define BATTERY_WARNING_LOW 54.4    // Low voltage warning
```

### **Test Mode**
For development and testing:
```cpp
bool useStaticData = true;  // Enable test mode
// Change to false for live VESC connection
```

## 📊 Display Layout

```
┌─────────────────────────────────────┐
│ 🔋 42.3V              359W ⚡      │
│ Battery               Watts          │
│                                     │
│            25                       │
│           MPH                       │
│        (Large Speed)                │
│                                     │
│ 45°C  65°C  12A   8A   1,247       │
│ VescT MotorT PHASE BATT  MILES      │
└─────────────────────────────────────┘
```

### **Color Coding**
- **🟢 Green**: Normal operation, good battery level
- **🟡 Yellow**: Caution - moderate temperature, low battery
- **🔴 Red**: Warning - high temperature, critical battery, over speed
- **⚪ White**: Standard data display

## 🔧 Troubleshooting

### **Common Issues**

#### **Black Screen**
```
Problem: Display shows backlight but no content
Solution: Check TFT_eSPI User_Setup.h configuration
         Verify pin connections match ESP32-2432S028R
```

#### **Font Errors**
```
Problem: 'JerseyM54_82pt7b' was not declared
Solution: Ensure all .h font files are in project directory
         Check font variable names match #define statements
```

#### **VESC Communication Timeout**
```
Problem: "Package to send: 2 1 4 64 132 3 66 Timeout"
Solution: Check UART wiring (TX↔RX, RX↔TX)
         Verify VESC UART settings (115200 baud)
         Enable UART in VESC Tool
```

#### **Compilation Errors**
```
Problem: background_image.h syntax error
Solution: Change 'unsigned byte' to 'unsigned char'
         Or comment out background image includes
```

### **Debug Mode**
Enable debug output via USB serial:
```cpp
#define DEBUG_MODE  // Uncomment for serial debugging
```

## 🔮 Future Features

### **Planned Enhancements**
- **Touch Navigation**: Multi-page interface with swipe gestures
- **Data Logging**: SD card trip data recording
- **Wireless Connectivity**: WiFi/Bluetooth data sharing
- **Advanced Metrics**: Efficiency calculations, range estimation
- **Customizable Layouts**: User-selectable dashboard themes

### **Expandable Data Display**
The VESC provides additional telemetry that can be added:
- Duty cycle percentage
- Amp-hours consumed/regenerated  
- Watt-hours consumed/regenerated
- Individual MOSFET temperatures
- Motor position and encoder data

## 🤝 Contributing

We welcome contributions! Please see our contributing guidelines:

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/AmazingFeature`)
3. **Commit** your changes (`git commit -m 'Add some AmazingFeature'`)
4. **Push** to the branch (`git push origin feature/AmazingFeature`)
5. **Open** a Pull Request

### **Development Setup**
```bash
# Clone your fork
git clone https://github.com/yourusername/VESC-MPH-Dashboard.git

# Create development branch
git checkout -b feature/your-feature-name

# Enable test mode for development
# Set useStaticData = true in main.ino
```

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **Benjamin Vedder** - Creator of VESC firmware and communication protocol
- **VESC Project** - Open-source motor controller ecosystem
- **TFT_eSPI Library** - Bodmer's excellent TFT display library
- **Jersey M54 Font** - justme54s for the sport-style typography
- **Arduino Community** - For extensive ESP32 development resources

## 📞 Support

- **Issues**: Report bugs via [GitHub Issues](https://github.com/yourusername/VESC-MPH-Dashboard/issues)
- **Discussions**: Join discussions in [GitHub Discussions](https://github.com/yourusername/VESC-MPH-Dashboard/discussions)
- **VESC Community**: [VESC Project Forums](https://vesc-project.com/forums)

## 📈 Project Status

**Current Version**: 1.0.0
**Status**: Stable Release
**Last Updated**: December 2024

### **Version History**
- **v1.0.0**: Initial release with MPH display and comprehensive telemetry
- **v0.9.0**: Beta testing with static data support
- **v0.8.0**: Alpha release with basic VESC communication

---

**⚡ Built for the electric vehicle community - Ride safe, ride smart! ⚡**
