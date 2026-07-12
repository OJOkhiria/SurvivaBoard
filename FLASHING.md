# Flashing SurvivaBoard

The project targets an ESP32-S3-WROOM-1-N16R8 with 16 MB of flash. Firmware
logs use USB Serial/JTAG so UART0 remains available to the MAX-M10S GNSS module.

## Build

```sh
pio run
```

The resulting artifacts are written to:

```text
.pio/build/esp32-s3-devkitc-1-n16r8v/
```

## Flash

Connect the board's USB-C port, then run:

```sh
pio run --target upload
```

If PlatformIO cannot select the serial device automatically, pass the port:

```sh
pio run --target upload --upload-port /dev/cu.usbmodemXXXX
```

On first bring-up, hold the ESP32 boot strap low while resetting the module if
the ROM bootloader does not enter download mode automatically.

The current schematic does not expose GPIO0 as a dedicated BOOT button or test
pad. If the first upload does not start automatically, temporarily pull the
ESP32-S3 module's GPIO0 pad to GND while resetting EN. Add an accessible BOOT
test pad or button in the next PCB revision.

## Monitor

```sh
pio device monitor --baud 115200
```

Expected initial logs report the shared bus bring-up and the availability of
the ILI9341 interface, E22 radio interface, MAX-M10S UART, BME688, and
MMC5983MA.
