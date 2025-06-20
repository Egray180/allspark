# Background

For this project I wanted to design a modular device that could be used to measure forces on wall mounted products. It works by clamping a piece of 3/4" ply in all corners, with 4 sensors in each corner. You can choose the size of the ply according to the product you are trying to test, and screw your product into the ply. The original goal was to measure how much force it takes to rip wall mounted products off the wall, but it could be used for pretty much anything. The configuration of the sensors allows AllSpark to measure forces normal to the wood (1 axis), shear forces (2 more axes), and torque in all three axes.

# Sensors

The [Uneo GHF10](https://www.uneotech.com/uploads/product_download/tw/GHF10-500N%20ENG.pdf) is a thin, flexible, piezoresistive force sensor designed to measure applied force from 0 to 500 N. It operates by converting mechanical pressure into a change in electrical resistance using a proprietary conductive polymer film. The sensor offers high linearity (≥99% R²), fast response (~0.1 ms), and a long lifespan (up to 10 million actuations). It’s ideal for embedded, robotic, and wearable applications requiring accurate, analog force sensing.

The sensor consists of multiple layers: a top electrode, a spacer with a sensing window, and a bottom electrode printed with piezoresistive ink. When pressed, the top and bottom electrodes make contact through the window. In this way, the applied pressure modulates the contact area and density of conductive paths, reducing overall resistance. With no applied force, the resistance is in the MΩ range, and with the full 500N force applied, the resistance is less than 1 kΩ. 

# PCB

I designed a custom 4 layer PCB for this project around an ESP32-C3-MINI-1-N4. AllSpark runs on x2 [L91](https://data.energizer.com/pdfs/l91.pdf) batteries under normal operating circumstances, but can also be powered over micro USB, which is also the primary flashing method. 

### Schematic

![](images/schematic_colour.png)

The circuit is sectioned as follows: 
- Green - battery circuit
- Orange - USB circuit
- Red - dual ideal diode ORing circuit to use micro USB power when both sources are provided
- Purple - ESP32, EN, BOOT, peripherals
- Blue - analog circuitry to feed into ADCs

### Layers, layout, routing

The board stackup is signal - ground - power - signal. 

![](images/full_layout.png)
![](images/PCB_Top_3D.jpg)
![](images/PCB_Bot_3D.jpg)

The Altium project and manufacturing files are attached in the project folders. 

### Rework

The TVS diode that I used casued issues when using battery power becasue it backfed voltage to the USB VBUS net through the USB differential pair, which toggled the boost converter enable. To fix it, I repopulated it with [this](https://www.ti.com/lit/ds/symlink/tpd2e2u06.pdf?HQS=dis-dk-null-digikeymode-dsf-pf-null-wwe&ts=1750318743440&ref_url=https%253A%252F%252Fwww.ti.com%252Fgeneral%252Fdocs%252Fsuppproductinfo.tsp%253FdistId%253D10%2526gotoUrl%253Dhttps%253A%252F%252Fwww.ti.com%252Flit%252Fgpn%252Ftpd2e2u06) component. Note that the project files do not reflect this update.

# Mechanical

# Software 

Make sure "USB CDC On Boot" is enabled for serial to work. 

# Capabilities
