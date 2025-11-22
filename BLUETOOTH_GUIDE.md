# Bluetooth Implementation Guide

## Overview
This project now includes Bluetooth Low Energy (BLE) support using the NimBLE library. NimBLE is a lightweight, efficient BLE stack that uses less memory than the standard ESP32 Bluetooth library.

## Features
- **BLE UART Service**: Implements Nordic UART Service (NUS) for serial communication
- **Status Monitoring**: Web interface shows real-time Bluetooth connection status
- **Low Memory Footprint**: NimBLE uses ~50% less RAM than classic Bluetooth
- **Auto-reconnect**: Automatically starts advertising when client disconnects

## Device Name
The Bluetooth device advertises as: **ESP32-SD-WiFi**

## Service UUIDs
- **Service UUID**: `6E400001-B5A3-F393-E0A9-E50E24DCCA9E` (Nordic UART Service)
- **RX Characteristic**: `6E400002-B5A3-F393-E0A9-E50E24DCCA9E` (Write to ESP32)
- **TX Characteristic**: `6E400003-B5A3-F393-E0A9-E50E24DCCA9E` (Notify from ESP32)

## Connecting to the Device

### Using Mobile Apps

#### iOS
1. Download **nRF Connect** or **LightBlue** from App Store
2. Scan for devices
3. Connect to "ESP32-SD-WiFi"
4. Navigate to the UART service
5. Enable notifications on TX characteristic
6. Write to RX characteristic to send data

#### Android
1. Download **nRF Connect** or **Serial Bluetooth Terminal** from Play Store
2. Scan for BLE devices
3. Connect to "ESP32-SD-WiFi"
4. Navigate to the UART service
5. Enable notifications on TX characteristic
6. Write to RX characteristic to send data

### Using Python (Desktop)
```python
import asyncio
from bleak import BleakClient, BleakScanner

SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
RX_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
TX_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

async def main():
    # Scan for device
    device = await BleakScanner.find_device_by_name("ESP32-SD-WiFi")
    
    if device:
        async with BleakClient(device) as client:
            # Enable notifications
            def notification_handler(sender, data):
                print(f"Received: {data.decode()}")
            
            await client.start_notify(TX_UUID, notification_handler)
            
            # Send data
            await client.write_gatt_char(RX_UUID, b"Hello ESP32!\n")
            
            await asyncio.sleep(5)

asyncio.run(main())
```

## Web Interface Status
The web interface at `http://[device-ip]/` shows:
- **Bluetooth: Connected** (green) - Client is connected
- **Bluetooth: Ready (No client)** (blue) - Advertising, waiting for connection
- **Bluetooth: Disabled** (gray) - Bluetooth not initialized

## API Usage

### In Your Code
```cpp
#include "bluetooth.h"

// Check if connected
if (BT.isConnected()) {
    // Send data
    BT.write("Hello from ESP32\n");
}

// Read data
if (BT.available()) {
    String data = BT.readString();
    Serial.println(data);
}
```

### Available Methods
- `BT.begin(deviceName)` - Initialize Bluetooth with device name
- `BT.end()` - Stop Bluetooth
- `BT.isConnected()` - Check if client is connected
- `BT.isEnabled()` - Check if Bluetooth is enabled
- `BT.write(data)` - Send data to connected client
- `BT.available()` - Check if data is available to read
- `BT.read()` - Read single byte
- `BT.readString()` - Read all available data as string
- `BT.flush()` - Clear receive buffer

## Memory Usage
NimBLE is optimized for low memory usage:
- **RAM**: ~20-30KB (vs 50-60KB for classic Bluetooth)
- **Flash**: ~200KB

## Build Flags
The following flags are set in `platformio.ini` to reduce memory:
```ini
-DCONFIG_BT_NIMBLE_ROLE_PERIPHERAL_DISABLED
-DCONFIG_BT_NIMBLE_ROLE_BROADCASTER_DISABLED
```

These disable unused BLE roles (we only need Central role for UART service).

## Troubleshooting

### Device Not Appearing in Scan
- Ensure Bluetooth is enabled on your phone/computer
- Check that the ESP32 has booted successfully (check serial monitor)
- Try restarting the ESP32

### Cannot Connect
- Make sure no other device is already connected
- Restart the ESP32
- Clear Bluetooth cache on your phone (Settings > Apps > Bluetooth > Clear Cache)

### Connection Drops
- Check power supply - weak power can cause disconnections
- Reduce distance between devices
- Check for WiFi interference (both use 2.4GHz)

## Disabling Bluetooth
To disable Bluetooth and save memory, comment out the initialization in `SdWiFiBrowser.ino`:
```cpp
// BT.begin("ESP32-SD-WiFi");
```

## Future Enhancements
Possible additions:
- File transfer over Bluetooth
- Remote command execution
- Bluetooth-based configuration
- Multiple simultaneous connections
