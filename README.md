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

# PCB Design
I went with an 80mm by 120mm rectangular 4-layer with 5mm radius filleted edges, with the following designations:
* Layer 1: Signals and USB power processing
* Layer 2: Solid ground plane (with some unfilled areas for antenna clearance)
* Layer 3: Bulk power distribution
* Layer 4: Extra signals

The subsystems are separated and clustered with their respective components to shorten routes and minimize noise interference, and some of these areas are demarcated with stitching vias. The display sits at the head of the board with the Ebyte module, I²C sensors, and a logo immediately underneath. Near the equator and to the left of the board sits the ESP32, with the antenna facing the board's edge and ~9mm of copper-free clearance on both sides of the antenna. On the right is a 25x25mm copper-free clearance zone to (hopefully) mount the Quectel YCGO011AA. In the bottom left is the MAX M10S with a U.FL coax connector, and in the bottom right is the USB-C receptacle and the power systems.

# Bill of Materials

|Reference|Qty|Value|DNP|Exclude from BOM|Exclude from Board|Footprint|Datasheet|
|---|---|---|---|---|---|---|---|
|AE1|1|Antenna| | | | | |
|C6,C9,C12,C15,C19,C23,C25,C27,C29 | 9 | 0.1uF | | | |Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder|
|C7,C13,C22,C26,C28,C30 | 6 | 10uF | | | |Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder| |
|C10,C11 | 2 | 22uF | | | | Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder | |
|C14 | 1 | 2.2uF | | | | Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder | |
|C16,C18,C20 | 3 | 1uF | | | |Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder | |
|C21 | 1 | 47pF | | | | Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder | |
|C24 | 1 | 4.7uF | | | | Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder | |
| D1 | 1 | D_TVS | | | | Diode_SMD:D_SOD-123F| |
|F1 | 1 | Polyfuse | | | | Fuse:Fuse_2920_7451Metric_Pad2.10x5.45mm_HandSolder | |
|FB1 | 1 | FerriteBead | | | | Ferrite_SMD:BEADC1608X95N | |
|IC1 | 1 | E22-900M22S | | | | ebyte:E22-900M22S | |
| J1 | 1 | USB_C_Receptacle_USB2.0_14P | | | | Connector_USB:USB_C_Receptacle_GCT_USB4105-xxA_16P_TopMnt_Horizontal | https://www.usb.org/sites/default/files/documents/usb_type-c.zip|
| J2 | 1 | Conn_Coaxial | | | | Connector_Coaxial:U.FL_Hirose_U.FL-R-SMT-1_Vertical | |
| L2 | 1 | 3.9uH | | | | Inductor_SMD:L_Sunlord_MWSA0402S | |
| L3 | 1 | 27nH | | | | Inductor_SMD:L_0603_1608Metric_Pad1.05x0.95mm_HandSolder | |
| R1,R2 | 2 | 5.1k | | | | Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder | |
| R3,R4 | 2 | 4.7k | | | | Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder | |
| R5 | 1 | 10k | | | | Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder | |
| SW1 | 1 | SW_Push | | | | Tactile_Switch:SW_TS04-66-55-BK-160-SMT | |
| U1 | 1 | ESP32-S3-WROOM-1 | | | | ESP32-S3-WROOM-1-N16R8:ESP32S3WROOM1N8R2 | https://www.espressif.com/sites/default/files/documentation/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf|
| U2 | 1 | TPD2EUSB30A | | | | USB_ESD:SOTFL35P100X50-3N |http://www.ti.com/lit/ds/symlink/tpd2eusb30a.pdf|
| U3 | 1 | AP63203WU | | | | Buck_Converter:SOIC_05WU-7_DIO-M |https://www.diodes.com/assets/Datasheets/AP63200-AP63201-AP63203-AP63205.pdf|
| U5 | 1 | MAX-M10S | | | | U-BLOX MAX-M10S:XCVR_MAX-M10S-00B | https://content.u-blox.com/sites/default/files/MAX-M10S_DataSheet_UBX-20035208.pdf|
| U6 | 1 | MMC5983MA | | | | MMC5983MA:MMC5983MA_MEM | |
| U7 | 1 | BME688 | | | | BME688:IC_BME688 | |
| U8 | 1 | TLV75533PDRV | | | | LDO:SOT95P280X145-5N |http://www.ti.com/lit/ds/symlink/tlv755p.pdf|
| U9 | 1 | TPD1E05U06DPY | | | | GPS_ESD:DIO_TPD1E05U06DPYR | https://www.ti.com/lit/ds/symlink/tpd1e05u06.pdf|
| U10 | 1 | CR2013-MI2120 | | | | ILI9341:CR2013-MI2120 | http://pan.baidu.com/s/11Y990|





<img width="564" height="833" alt="Screenshot 2026-07-08 at 11 11 59 PM" src="https://github.com/user-attachments/assets/cd43f9ff-9a8a-48c6-91d7-b162855d03d8" />

<img width="1161" height="787" alt="Screenshot 2026-07-11 at 3 25 43 PM" src="https://github.com/user-attachments/assets/5b153f4d-4d80-4b17-8f36-12e4084f73e6" />

<img width="428" height="625" alt="Screenshot 2026-07-09 at 12 09 27 AM" src="https://github.com/user-attachments/assets/cbba426d-6cf8-4c08-8a19-a40d461bbf24" />

