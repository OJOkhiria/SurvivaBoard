# SurvivaBoard
A custom LoRa board equipped with an ESP32-S3-WROOM-1-N16R8, Ebyte E22-900M22S SX1262 radio, U.FL antenna, U-BLOX MAX M10S 00B GPS module, MMC5983MA magnetometer, BME688 environmental sensor, and a CR2013-MI2120 display driven by an ILI9341 controller. 

# Power
Because the board uses a variety of sensors, a GPS, a radio, and other noisy components, I created two separate 3.3V power rails for devices with different noise tolerances. 5V power first comes in from the USB-C receptacle and is stepped down with a fixed-output 3.3V buck converter (ap63203wu-7). To avoid surging, there are TVS diodes on the USB power lines and the D+/D- data lines, and a polyfuse on the power line immediately before the buck converter. The raw output from the buck converter will be used for the ESP32, Ebyte E22-900M22S, and ILI9341. This output also goes through a ferrite bead and an LDO (tlv75533pdrvr) converter to further reduce noise for sensors and other peripherals.

# Subsystems
**ESP32-S3-WROOM-1-N16R8:** Serves as the controller for the entire board. Has a built-in antenna for WiFi and Bluetooth interfacing and communicates with the other various subsystems with UART, SPI, and I²C. It's equipped with an external reset button and plenty of bulk decoupling using 0.1μF and 10μF capacitors.

**U-Blox MAX M10S 00B:** A GPS/GNSS module that will be used to gather data on the board's location and communicate with the ESP32 via UART. Equipped with a TPD1E05U06DPY ESD protection diode; 10μF, 1μF, and 0.1μF decoupling capacitors; and an external coax connector for an active patch antenna (the Quectel YCGO011AA). 

**Ebyte E22-900M22S:** A LoRa module built on the SX1262 transceiver. Originally, I was going to use a bare SX1262, but I chose to go with the Ebyte module to avoid having to build an entire RF front end from scratch. Has an onboard coax connector for an antenna, which will be connected to the FXP290.07.0100A. Communicates with the ESP32 via SPI, and may be used for Meshtastic integration.

**MMC5983MA:** The magnetometer of the board. It will provide heading information to provide the board with a built-in compass. Communicates with the ESP32 using I²C, and has 1μF and 10μF decoupling capacitors.

**BME688:** An environmental sensor that measures temperature, humidity, barometric pressure, and VOC gases. It can generate IAQ (indoor air quality) and also has integrated artificial intelligence, which will be trained to recognize certain odors/gas signatures. Has 0.1μF and 1μF decoupling capacitors and communicates with the ESP32 using I²C.

**ILI9341:** This will be used to display all the information the board gathers, and potentially for users to use Meshtastic. Has an SD card interface for any fonts or other things, and a PWM backlight control. Has 0.1μF and 10μF decoupling capacitors and communicates with the ESP32 using SPI.


