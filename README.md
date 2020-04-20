# Zedboard_Etch_A_Sketch
Using the Zedboard FPGA development board with Digilent PMODS for a rotary encoder and RGB OLED Display, allows the user to draw on the screen based on which button is pressed while moving the encoder.

Repository contains the Vivado Block design to setup the Zynq processing system and connect peripherals, and C code from SDK to operate the Encoder and OLED display as an Etch-a-Sketch

To generate the bitstream in Vivado, get the PMOD library from https://github.com/Digilent/vivado-library/releases and add it to the project IP repository. Create HDL wrapper for design_1.bd and the bitsream should generate.
