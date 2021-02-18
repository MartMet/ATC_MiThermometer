# ATC_MiThermometer Supercapacitor Modification


Initial forked from https://github.com/pvvx/ATC_MiThermometer (v2.0)

(Thanks and Credits to @atc1441 and @Victor pvvx for this awesome project)

## Why this fork?
This fork aims to provide a hardware & software modification to run the Xiaomi Thermometer LYWSD03MMC with a supercapacitor and mini solarpanel. So no CR2032 batteries needed anymore.

![preview](/images/preview.jpg)

## Used Hardware
* Coin Cell Supercapacitor 1.5F 5.5V (e.g. Korchip DCL5R5155VF EDLC)
* Germanium Diode (should be prefered over Silicon Diode due to less voltage drop)
* Solar Panel BR-243318C (small version) or BP-376634C (big version) http://solarbotics.net/library/datasheets/sunceram.pdf

## Whats the runtime with the super capacitor?
over 48 hours:

<img src="https://github.com/MartMet/ATC_MiThermometer/blob/master/images/supercap_runtime.jpg" alt="supercap_runtime" width="500"/>

Voltage drop over time:
Charged 1.5F to 3.6V and let the thermometer run with 48MHz. Advertising interval 10s, measurement is done via plotting internal ADC values.

If the capacity is assumed as labeled and the self-discharge is left aside, the power consumption is on average of  [40.58µW](https://www.wolframalpha.com/input/?i=0.5*1.5F*%28%283.6V%29%5E2-%281.9V%29%5E2%29%2F48h%3DUnitSimplify%5Bx%5D&assumption=%22UnitClash%22+-%3E+%7B%22F%22%2C+%7B%22Farads%22%7D%7D&assumption=%7B%22C%22%2C+%22V%22%7D+-%3E+%7B%22Unit%22%7D&assumption=%22UnitClash%22+-%3E+%7B%22h%22%2C+%7B%22Hours%22%7D%7D)


## Hardware Modifications
* very less modifications needed
* no voltage regulator needed (there is already a DC-DC converter integrated, TLSR8251 is rated from 1.8V-3.6V)
* the supercapacitor fits in without soldering at all. (So the Thermometer can be still powered from a CR2032 cell)


### Case
The case needs minor modifications to be able to connect the solar panel with wires.

<img src="https://github.com/MartMet/ATC_MiThermometer/blob/master/images/case1.jpg" alt="case1" width="250"/>
<img src="https://github.com/MartMet/ATC_MiThermometer/blob/master/images/case2.jpg" alt="case2" width="250"/>

Note: When viewing the LCD the wires should be exit the case on the left side. The BLE antenna is on the right and i expierenced a worse RSSI while the first attempt to exit the wires on the right side.

### Soldering
Just solder GND and Voltage wires a shown. Dont forget to place in a diode, this is necessary that the solar panel does not discharge the Supercap overnight.

<img src="https://github.com/MartMet/ATC_MiThermometer/blob/master/images/solder.jpg" alt="solder" width="400"/>


(This was the first attempt, with wires right over the BLE antenna. Be sure to use the other side!)

### Supercapacitor
Remove the "-" pin from the supercapacitor. And carefully bend the "+" pin down. The capacitor fits into the slot of the CR2032.

<img src="https://github.com/MartMet/ATC_MiThermometer/blob/master/images/cap1.jpg" alt="cap1" width="250"/>
<img src="https://github.com/MartMet/ATC_MiThermometer/blob/master/images/cap2.jpg" alt="cap2" width="250"/>

## Software Modifications
Only minor modifications are done:

* voltages below 1.9V will send the device to cyclic 2min deepsleep, over 1.9V the device will wakeup
* "poor mans" overvoltage protection: as long as the voltage is over 3.6V a 100ms power waste loop will be called
	* the smaller solar panel has only 5 cells which will be 0.7V * 5 - 0.3V(Ge-Diode) = max. 3.2V in bright sunlight, so no protection needed
	* the bigger solar panel has 8 cells (5.3V) but cant power the MCU in active mode in bright sunlight, so the voltage will drop
	* be sure not to update the device in bright sunlight
	* simple HW overvoltage protection means like Z-Diodes or LEDs wont work as they draw several µA even in working voltage range
* changed battery level indicator via LUT to take the non-linearity of the supercapacitor into account
* the MCU clock is changed from 24MHz to 48MHz to achieve more current consumption in the power waste loop (interestingly the higher MCU clock also reduced overall power consumption)

<img src="https://github.com/MartMet/ATC_MiThermometer/blob/master/images/voltage_comparsion.jpg" alt="voltage_comparsion" width="500"/>
Voltage drop over time:
Charged 1.5F to 3.6V and let the thermometer run with 24MHz/48MHz. Advertising interval 10s, measurement is done via plotting internal ADC values. 

## OTA and Custom Setup
Flash the file "ATC_Thermometer20_mod.bin" with:
[TelinkMiFlasher.html](https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html) - OTA and customize, auto-download files new firmware


## Node-RED 
read advertisements with Node-RED. [Node-RED](https://github.com/MartMet/ATC_MiThermometer/blob/master/node-RED/nodered.md)