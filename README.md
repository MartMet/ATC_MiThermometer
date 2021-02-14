# ATC_MiThermometer Supercapacitor Modification


Initial forked from https://github.com/pvvx/ATC_MiThermometer (v2.0)

(Thanks and Credits to @atc1441 and @Victor pvvx for this awesome project)

## Why this fork?
This fork aims to provide a hardware & software modification to run the ATC-MiThermometer with a supercapacitor and mini solarpanel. So no CR2032 batteries needed anymore.

![preview](/images/preview.jpg)

## Used Hardware
* Coin Cell Supercapacitor 1.5F 5.5V (e.g. Korchip DCL5R5155VF EDLC)
* Germanium Diode (should be prefered over Silicon Diode due to less voltage drop)
* Solar Panel BR-243318C (small version) or BP-376634C (big version) http://solarbotics.net/library/datasheets/sunceram.pdf


## Hardware Modifications
* very less modifications needed
* no voltage regulator needed
* the supercapacitor fits in without soldering at all. (So the Thermometer can be still powered from a CR2032 cell)


### Case
The case needs to minor modifications to be able to connect the solar panel with wires.
![case1](/images/case1.jpg | width=250)
![case2](/images/case2.jpg | width=250)

Note: When viewing the LCD the wires should be exit the case on the left side. The BLE antenna is on the right and i expierenced a worse RSSI while the first attempt to exit the wires on the right side.

### Soldering
Just solder GND and Voltage wires a shown. Dont forget to place in a diode, this is necessary that the solar panel does not discharge the Supercap overnight.
![solder](/images/solder.jpg | width=400)

(This was the first attempt, with wires right over the BLE antenna. Be sure to use the other side!)

### Supercapacitor
Remove the "-" pin from the supercapacitor. And carefully bend the "+" pin down. The capacitor fits into the slot of the CR2032.

![cap1](/images/cap1.jpg | width=250)
![cap2](/images/cap2.jpg | width=250)

## Software Modifications

