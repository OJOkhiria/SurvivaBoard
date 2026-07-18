# 1
In this session, I worked on the full power system for SurvivaBoard. Because the board uses a variety of sensors, a GPS, a radio, and other noisy components, I created two separate 3.3V power rails for devices with different noise tolerances. 5V power first comes in from the USB-C receptacle and is stepped down with a fixed-output 3.3V buck converter. The raw output from the buck converter will be used for the ESP32, SX1262, and TFT display. This output also goes through a ferrite bead and an LDO converter to further reduce noise for sensors and other peripherals.

<img width="1046" height="900" alt="image" src="https://github.com/user-attachments/assets/223d5e0d-f029-4d16-98a3-c5a2ac85299f" />

# 2
In this session, I wired all the subsystems for SurvivaBoard and tweaked my power system a bit. In summary, I:

- Added a polyfuse and TVS diode to my USB power system to avoid surging
- Added an ESD to the D+ and D- pins before connecting to the ESP32
- Switched from a bare SX1262 to the Ebyte E22-900M22S to avoid having to create an entire RF front end from scratch
- Changed to a simpler TFT module to keep the board hierarchy centered around the ESP32
- Selected and wired which GPIO pins to use for peripherals (after researching for half an hour which ones I could and couldn’t use)
- Added a reset button to the ESP32
- Added proper local decoupling to each IC

<img width="1431" height="900" alt="image" src="https://github.com/user-attachments/assets/35c071f5-e822-4724-b2b5-8e5d12c52496" />


# 3
In this session, I researched the proper footprints for all my components and assigned them before moving on to PCB routing.

<img width="1042" height="900" alt="image" src="https://github.com/user-attachments/assets/18d776e7-0355-4627-bbfa-f4a07c6a8266" />


# 4
In this session, I plotted all the footprints into the PCB editor and got to routing. I went with a 80mmx120mm 4 layer board. The first and bottom layers are for general signal connections and decoupling, the second layer is a solid ground plane, and the third is for bulk power distribution. The different components are also grouped in a certain way to minimize noise and interference while avoiding winding/convoluted traces. 

<img width="625" height="900" alt="image" src="https://github.com/user-attachments/assets/c76cdc74-19b3-4b1f-9cc3-6061114a2482" />

# 5
In this session, I fixed all the errors on the PCB, made the routing a little more organized, added clearance around the various antennae, added stitching vias, made and added a logo, and changed a few components.

For the environmental sensor, I switched from the BME680 to the BME688 for its AI capabilities, and for the magnetometer, I switched from the LIS2MDLTR to the MMC5983MA because the LIS2MDL was way too hard to source. I also tweaked the inductance of the buck converter to 3.9uH instead of 2.2, and I adjusted the track thickness by component to account for power needs.

<img width="617" height="900" alt="image" src="https://github.com/user-attachments/assets/539b9db1-f6f7-4750-8bc7-41f874d0d2d6" />

# 6
**FINALLY DONE 🥹**

In this session, I wrote the firmware for flashing onto the ESP32. This was honestly the most confusing part of the project. I built it in VS Code with the PlatformIO and ESP-IDF extensions for project management and framework. I also used FreeRTOS with modular drivers to make future edits easier to make. The architecture is based on multiple layers of hardware abstraction and interconnection. For example, programs for sensors will never interface directly with GPIO pins and i2c protocol, instead just sending a read/write request. In addition to the component and communication buses, I also built a display protocol for initializing the board, scanning for sensor presence, gathering data, and updating it in real-time on the screen about twice every second. All of this was wrapped into a flashable bin, meaning **SURVIVABOARD IS READY TO GO!**

<img width="1297" height="900" alt="image" src="https://github.com/user-attachments/assets/f5acc4d7-fd74-4c23-8012-fdb35edf32fa" />
