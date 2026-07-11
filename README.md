# SurvivaBoard
A custom LoRa board equipped with an ESP32-S3-WROOM-1-N16R8, Ebyte E22-900M22S SX1262 radio, U.FL antenna, U-BLOX MAX M10S 00B GPS module, MMC5983MA magnetometer, BME688 environmental sensor, and a CR2013-MI2120 display driven by an ILI9341 controller. 

# Power
Because the board uses a variety of sensors, a GPS, a radio, and other noisy components, I created two separate 3.3V power rails for devices with different noise tolerances. 5V power first comes in from the USB-C receptacle and is stepped down with a fixed-output 3.3V buck converter (ap63203wu-7). To avoid surging, there are TVS diodes on the USB power lines and the D+/D- data lines, and a polyfuse on the power line immediately before the buck converter. The raw output from the buck converter will be used for the ESP32, Ebyte E22-900M22S, and ILI9341. This output also goes through a ferrite bead and an LDO (tlv75533pdrvr) converter to further reduce noise for sensors and other peripherals.

# Subsystems
