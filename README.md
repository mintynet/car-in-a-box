# car-in-a-box
This repository is for the hardware and software parts that make my 'Car in a box' PD0 think that it is a fully working Car
The 'Car in a box' has most of the ECUs from a 2014 Peugeot 208 1.0 3 Cylinder.

## Ardustim 
The original ardustim software comes from [Speeduino Ardustim project](https://github.com/speeduino/Ardu-Stim) The sketch in the ardustim folder has been modified to contain the Crank & 2 Cam profiles for the 1.0 3 Cylinder Engine.
The PCB allows for either surface mount or through hole 2N3904 transistors and 1k base resistors. he 1st channel can also use a TIP120 transistor to allow for use with bigger load devices. An Arduino Nano is used with a potentiometer (0v,Signal,5v) is connected to analog 0 input to allow the changing of the RPMs of the engine.
The inputs to the board connect to pins 8, 9 & 10 on the Nano. The outputs are connected to the Crank and two Cam sensor inputs on the Engine ECU. A common ground is also required between the MCU and the ECU.
<br>
![Ardustim Schematic](/Hardware/ardustim-sch.png)
![Ardustim PCB](/Hardware/ardustim.jpg)

## O2 Sensor simulator
The O2 sensor simulator uses a 556 timer which is a dual 555 timer, the circuit used is shown below. Two sensors are required because one is pre and one is post the catalitic convertor. There is also a requirement for the heater circuit to be simulated by using a 50W 8ohm wirewound resistor connected across where the heater circuit would be connected.
<br>
![O2 Sim Schematic](/Hardware/O2Sim-sch.jpg)
![O2 Sim Front](/Hardware/O2Sim-front.jpg)
![O2 Sim Rear](/Hardware/O2Sim-rear.jpg)
![O2 Sim Scope](/Hardware/O2Sim-scope.jpg)

## ABS Sensor Simulator
The ABS/Wheel Speed Sensor Simulator does not use a digital signal to make the car think that the wheels are turning. It uses a [Nano-can](https://github.com/mintynet/nano-can) with an addition 20x4 LCD, a potentiometer to control the required speed of the PWM controlled motor, this is driven using an ardustim PCB but using the TIP120 transistor to allow for the greater current of the motor. The CAN bus is connected to the cluster LS CAN BUS (125kbps) to allow the simulator to read the speed from the vehicle. The sketch is in the ABS-Sim folder.

## Fuel and Engine Temperature gauge
These are just potentiometers across the lines on the respective wiring in the vehicle loom.
