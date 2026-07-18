
In this session, I worked on the full power system for SurvivaBoard. Because the board uses a variety of sensors, a GPS, a radio, and other noisy components, I created two separate 3.3V power rails for devices with different noise tolerances. 5V power first comes in from the USB-C receptacle and is stepped down with a fixed-output 3.3V buck converter. The raw output from the buck converter will be used for the ESP32, SX1262, and TFT display. This output also goes through a ferrite bead and an LDO converter to further reduce noise for sensors and other peripherals.

<img width="1046" height="900" alt="image" src="https://github.com/user-attachments/assets/223d5e0d-f029-4d16-98a3-c5a2ac85299f" />


