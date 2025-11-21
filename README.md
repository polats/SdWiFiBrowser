# SD WiFi Pro

ESP32/ESP8266 firmware for wireless SD card management on 3D printers. Access and manage your printer's SD card files over WiFi through a web interface.

## Features

- **Wireless File Management**: Upload, download, and delete files via web browser
- **Dual WiFi Modes**: Station (connect to existing network) or Access Point
- **Smart SD Sharing**: Safely shares SD card access between printer and ESP32
- **Web Interface**: Clean, responsive file manager
- **REST API**: Programmatic access for automation
- **Configuration Options**: EEPROM, INI file, or web-based setup

## Quick Start

### First Time Setup

1. **Power on the device**
   - Device creates WiFi access point on first boot
   - SSID: `SD-WIFI-PRO`
   - IP: `192.168.4.1`

2. **Connect to the AP**
   - Connect your device to the `SD-WIFI-PRO` network
   - Open browser to `http://192.168.4.1`

3. **Configure WiFi** (optional)
   - Click "SETTING" to access WiFi configuration
   - Enter your network SSID and password
   - Click "Connect"
   - Device will connect and display its new IP address

4. **Manage Files**
   - Browse files on SD card
   - Upload G-code files from your computer
   - Download or delete existing files

### Alternative Configuration

Create `data/SETUP.INI` file before uploading to SPIFFS:

```ini
SSID=YourNetworkName
PASSWORD=YourPassword
```

## Hardware Requirements

### Components

- ESP32 or ESP8266 development board
- SD WiFi Pro module (or compatible)
- SD WiFi Pro dev board (for programming)
- SD card (FAT32 formatted, up to 32GB recommended)
- SD card slot with multiplexer circuit
- USB Type-C cable

### Pin Configuration

**ESP32 Standard:**

- SD Card: CS=13, MISO=2, MOSI=15, SCLK=14
- SD Switch: GPIO 26 (multiplexer control)
- SD Power: GPIO 27 (optional)
- CS Sense: GPIO 32 (printer SD access detection)

**ESP8266:**

- SD Card: CS=4, MISO=12, MOSI=13, SCLK=14
- CS Sense: GPIO 5

See `pins.h` for other board variants (BEAM, etc.)

## Building from Source

### ⚠️ Important: ESP32 Core Version

This firmware requires **ESP32 Arduino Core 2.0.x**. If you're using Core 3.x, you'll encounter compilation errors.

**Quick Fix**: 
1. Downgrade to ESP32 Core 2.0.17 in Arduino IDE Boards Manager
2. Fix AsyncTCP library (see [QUICK_FIX.md](QUICK_FIX.md))

See **[QUICK_FIX.md](QUICK_FIX.md)** for step-by-step instructions or **[COMPILATION_GUIDE.md](COMPILATION_GUIDE.md)** for detailed troubleshooting.

### Requirements

**Arduino IDE:**

- ESP32 Core **2.0.17** (or any 2.0.x version) / ESP8266 Core 3.x
- Required libraries:
  - ESPAsyncWebServer
  - AsyncTCP (ESP32) or ESPAsyncTCP (ESP8266)
  - ArduinoJson
  - SD
  - SPIFFS

**PlatformIO:**

```bash
# Interactive menu
./upload.sh

# Quick upload (no prompts)
./quick-upload.sh

# Manual commands
platformio run --target uploadfs  # Upload SPIFFS
platformio run --target upload    # Upload firmware
```

### Compilation Steps

1. Install ESP32 Core 2.0.x or ESP8266 board support
2. Install required libraries via Library Manager
3. Select your board (ESP32 Dev Module or ESP8266)
4. Set partition scheme to include SPIFFS
5. Upload sketch
6. Upload SPIFFS data (web interface files in `data/` folder)

## Flashing Pre-built Binary

Binary firmware is located in `tools/Flash/binary/` directory.

### Hardware Setup

1. **Connect SD WiFi Pro to Dev Board**
   - Insert SD WiFi Pro module into dev board socket
   - Connect USB Type-C cable to dev board
   - Plug cable into PC USB port

2. **Configure DIP Switches**
   - Switch 1: **OFF** (bootloader mode)
   - Switch 2: **ON** (enable programming)

3. **Verify Connection**
   - Check Device Manager (Windows) for "USB-SERIAL CH340 (COMx)"
   - Note the COM port number
   - If not detected, install CH340 driver

### Flashing (Windows)

```bash
cd tools/Flash
install-all-8M.bat
```

Follow the prompts to select your COM port.

### Flashing (Manual)

```bash
cd tools/Flash
win64/esptool.exe --chip esp32 --port COM3 --baud 921600 \
  --before default_reset --after hard_reset write_flash \
  -z --flash_mode dio --flash_freq 80m --flash_size 8MB \
  0x1000 binary/bootloader.bin \
  0x8000 binary/partitions.bin \
  0x10000 binary/firmware.bin \
  0x310000 binary/spiffs.bin
```

Replace `COM3` with your actual COM port.

### After Flashing

1. **Reset DIP Switches** (for normal operation)
   - Switch 1: **ON**
   - Switch 2: **OFF**

2. **Power cycle the device**
3. **Look for SD-WIFI-PRO access point**

## Web Interface

### Main Page (`/` or `/index.htm`)

- **File List**: View all files on SD card with size information
- **Upload**: Select and upload files from your computer
- **Download**: Download files to your computer
- **Delete**: Remove files from SD card
- **Update List**: Refresh file listing

### WiFi Settings (`/wifi.htm`)

- **Connect**: Join existing WiFi network
- **AP Mode**: Switch to access point mode
- **Status**: View connection status and IP address

## API Reference

### File Operations

```bash
# List files
GET /list?dir=/

# Download file
GET /download?path=/filename.gcode

# Upload file
POST /upload
Content-Type: multipart/form-data

# Delete file
GET /delete?path=/filename.gcode

# Release SD card control
GET /relinquish
```

### WiFi Operations

```bash
# Scan networks
GET /wifiscan

# Get scan results
GET /wifilist

# Connect to network
POST /wificonnect?ssid=NetworkName&password=Password

# Check status
GET /wifistatus

# Enable AP mode
POST /wifiap
```

## Documentation

- **[QUICK_FIX.md](QUICK_FIX.md)**: ⚡ Fast compilation fix guide (start here!)
- **[SCRIPTS.md](SCRIPTS.md)**: 🚀 Upload scripts guide (PlatformIO)
- **[USER_GUIDE.md](USER_GUIDE.md)**: Complete user documentation with troubleshooting
- **[DEVELOPER_GUIDE.md](DEVELOPER_GUIDE.md)**: Technical architecture and development guide
- **[COMPILATION_GUIDE.md](COMPILATION_GUIDE.md)**: Detailed ESP32 core compatibility guide

## Troubleshooting

**Cannot compile - ESP32 Core 3.x errors:**

- See [COMPILATION_GUIDE.md](COMPILATION_GUIDE.md)
- Quick fix: Downgrade to ESP32 Core 2.0.17

**Cannot connect to WiFi:**

- Ensure 2.4GHz network (5GHz not supported)
- Verify SSID and password are correct
- Check serial output at 115200 baud for errors

**"SD card is busy" errors:**

- Printer is currently accessing SD card
- Wait 10 seconds after printer activity
- Device automatically manages SD card sharing

**Web interface not loading:**

- Verify SPIFFS data was uploaded
- Check device IP address (serial output or router)
- Try accessing via AP mode (192.168.4.1)

## Debug Mode

Enable detailed logging by editing `serial.h`:

```cpp
//#define RELEASE  // Comment out this line
```

Recompile and monitor serial output at 115200 baud.

## Dependencies

- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- [AsyncTCP](https://github.com/me-no-dev/AsyncTCP) (ESP32)
- [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP) (ESP8266)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)

## License

GNU General Public License v3.0 or later.

## Credits

Developed by FYSETC  
Serial communication code based on Marlin firmware

## Support

- Website: [https://www.fysetc.com/](https://www.fysetc.com/)
- For detailed documentation, see USER_GUIDE.md and DEVELOPER_GUIDE.md
- For compilation issues, see COMPILATION_GUIDE.md
