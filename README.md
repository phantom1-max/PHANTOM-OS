# PHANTOM OS Starter (Build 0.1)

A small cyberdeck-style interface for the ZEVRA ESP32-S3 N16R8 development board. The ESP32 runs the firmware and serves the interface to a phone or computer browser over its own Wi-Fi network.

## Current features
- Wi-Fi access point named `PHANTOM-OS`
- Browser interface at `http://192.168.4.1`
- Terminal commands: `help`, `status`, `memory`, `uptime`, `wifi`, and `clear`
- Live status panel for uptime, free heap, chip/cores, and connected Wi-Fi clients
- Cyberpunk-inspired dark green interface with Terminal, System Monitor, File Explorer placeholder, and Settings

## Hardware
- Target: ZEVRA ESP32-S3 N16R8 (16 MB flash, 8 MB PSRAM)
- USB-C data cable for programming and power
- Phone or computer browser for the interface
- No TFT screen required for this first version
- microSD reader is not yet used by this firmware

## Install with Arduino IDE
1. Install Arduino IDE: https://www.arduino.cc/en/software
2. Open Arduino IDE Settings/Preferences.
3. Add this URL under **Additional boards manager URLs**:
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
4. Open Boards Manager and install **esp32 by Espressif Systems**.
5. Download or open `PHANTOM_OS.ino`.
6. Select **ESP32S3 Dev Module** under Tools > Board.
7. Connect the ESP32-S3 board over USB-C and choose its port.
8. Upload the sketch. If uploading fails, try holding the board's BOOT button while starting upload, then release it when writing begins.
9. Open Serial Monitor at **115200 baud** for startup details.

## Connect to PHANTOM OS
1. From your phone or computer, connect to Wi-Fi network `PHANTOM-OS`.
2. Prototype password: `phantom123`.
3. If the device says the network has no internet, choose to stay connected.
4. Open a browser and visit **http://192.168.4.1**.
5. Open Terminal and try `help`, `status`, `memory`, `uptime`, or `wifi`.

## Security and limitations
- Change `AP_PASSWORD` in the sketch before using this outside a private prototype.
- This is an experimental embedded interface, not Linux or a general-purpose desktop OS.
- The phone/computer renders the web interface; the ESP32 runs the firmware and command handlers.
- File Explorer is currently a placeholder. microSD file listing/read/write is not implemented.
- Bluetooth keyboard and USB mouse support are not implemented in this starter.
- Do not wire the microSD module until the exact GPIO pinout for your board is confirmed.

## Project status
Build 0.1 starter firmware. Hardware upload and testing are still required; this repository does not claim that the firmware has been tested on the physical board.
