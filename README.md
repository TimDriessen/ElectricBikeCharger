# NiMH Electric Bike Charger

![banner](https://user-images.githubusercontent.com/45215498/78689553-bf993c00-78f6-11ea-81bd-9e47edc682dd.jpg)

Controller that overviews charging process of a Sparta ION electric bike and automatically switches off charger when certain conditions are met.

 In this project a charged is created for the Sparta ION electric bike. This bike contains a 24V 10000 mAh NiMH battery pack consisting of 20 cells. To make the charger as safe as possible, the battery is charged at a C rate of C/10 where C is the capacity of the battery. For a full charge, the charger should be connected for around 12 hours. This translates to essentially overnight charging. 

The battery-cells will warm up during charge. To prevent damage to the battery on overcharge, modern NiMH cells have an oxygen recycling catalyst built in. With the chosen C-rate, this recycling should be able to keep up and not result in dangerous situations where the battery cells become too hot.

A number of safety measurements were incorporated. The charger will automatically turn off when:
-	Turned off by user
-	Max battery temperature reached
-	Max heatsink temperature reached
-	Battery temperature rising too fast
-	Negative battery voltage drop exceed threshold
-	Max battery voltage reached

## Why this project?

This charger is a small milestone in a larger project of gaining full control of a Sparta ION electric bike. A number of dead battery cells caused the electric bike to not function anymore. Just a simple replacement of these dead battery cells wouldn’t be possible since special Sparta software would be required to re-enable the repaired battery, which only licensed Sparta dealers have access to.  

The original Sparta charger consists of the charger itself and a PCB built into the electric bike. This built-in PCB controls the charger (turning it off when full, trickle charge etc..) and also the motor of the electric bike. This meant that in order to get rid of the Sparta software troubles I had to create an independent charger that is able to charge the 20 NiMH cells. That is exactly what is done in this project.

## What components did I use?

With this built I tried to maximally re-use the components of the charger and electric bike. Because of this I don’t have exact names and links of all parts used. But all them are definitely not hard to come by. A complete list follows below:

| Component | Quantity |
| --- | --- |
| ESP8266 ESP-12E | 1x |
| LM317 Adjustable Voltage Regulator | 1x |
| LM317 Adjustable Voltage Regulator | 1x |
| Step Down Voltage Regulator (2A 4-36V to 12V) | 1x |
| Step Down Voltage Regulator (2A 4-36V to 3.3V) | 1x |
| SN74HC4051 Multiplexer | 1x |
| BS170 MOSFET | 2x |
| 1N4004 Diode | 8x |
| 12V Fan | 1x |
| Small push button  | 2x |
| Rocker ON/OFF Toggle Mini Switch | 1x |
| Small LED | 2x |
| YP-05 (for flashing) | 1x |
|  |  |
| ***Resistors*** |  |
| 1.2 Ohm | 4x |
| 200 Ohm | 1x |
| 1K Ohm | 1x |
| 4.7K Ohm | 1x |
| 5.1K Ohm | 2x |
| 10K Ohm | 6x |
| 10K Ohm Thermal Resistor NTC | 2x |
| 47K Ohm | 1x |
| 51K Ohm | 1x |
| 56K Ohm | 1x |
| 100K Ohm | 4x |
|  |  |
| ***Capacitors*** |  |
| 35V 470μF (Electrolytic) | 2x |
| 50V 1μF (Electrolytic) | 2x |
| 0μ1 (Ceramic) | 2x |
|  |  |
| ***Not visible in drawing*** |  |
| 19V Laptop Charger (min 1A) | 2x |
| PCB Protoboard (10x15cm)| 1x |
| Power Supply DC Female Jack 5.5 · 2.5mm | 2x |
| Power Supply DC Male Jack 5.5 · 2.5mm | 2x |
| Aluminum U-profile for heatsink | 1x |
| 2mm diameter copper wire | 1x |
| Wires (for connecting components) | 1x |

Some of these parts were chosen just by the convenience of having them laying around. For example I’ve used the original plastic case of the charger in which I built in my own PCB Prototype board. Also instead of the two 19V chargers it would perhaps be better to have one larger power supply. The main purpose of this project is to inspire people to find their own best solution using perhaps some inspiration from this project.

## How did I built it?

![Schematic of the charger](https://user-images.githubusercontent.com/45215498/78663929-4cca9980-78d3-11ea-8fae-b95fbf24dfcd.png)

***Charger control***  
The circuit starts of by connecting the laptop chargers in series. Our input voltage then becomes 19*2 = 38V. Directly after this the two Step Down Voltage Regulators are placed (IC2 and IC3). The 3.3V Step Down powers the ESP and the 12V Step Down powers the fan. Since the regulators have a maximum input voltage of 36V the input voltage should be lowered. This was done by connecting six diodes directly after the laptop chargers.

Controlling the LM317 to supply a constant 1A of current was done in the following way. In short, the LM317 adjusts it’s output voltage until the voltage difference between the adjustment terminal and the output terminal is exactly 1.25V. If we now put a resistor R in series between the output terminal and the adjustment terminal, then we have a potential difference of 1.25V over the resistor, which means that we have a current flow of 1.25/R amps (through resistor R1 in the drawing flows a negligible amount of current). By now taking R to be 1.2 Ohms, we have a current flow of 1.25/1.2 = 1.042 A ≈ 1A. The dissipated power for the resistor would be 1.25 * 1 = 1.25W, which is quite a lot. Therefore four resistors were used so that the power dissipation could be better divided.

To turn off the charger we’ve used a MOSFET from which the gate is controlled by the ESP. By making the gate positive, current can flow between drain and source. The adjust pin is now pulled to ground (0V) and therefore we must have that the output pin is exactly 1.25V. With this voltage it is impossible to charge the battery (± 24V). To prevent current to flow from the battery to this 1.25V output pin, diode D1 was placed.

When the charger is on, a voltage of around 30 volts can be measured at the output pin. If resistor R1 wouldn’t be placed there and one would turn off the charger, the 30 volts would be directly connected to ground through the MOSFET. This would break the MOSFET as a maximum of 500 mA can flow through it. Therefore resistor R1 is needed.

Capacitors C1, C2, C3 and resistor R2 are placed there to reduce the noise on the wires.

***Sensory inputs***  
To know when exactly we want to turn on and off the charger, four sensors were incorporated into the design (two temperature and two voltage sensors). 
To be able to measure the voltage before and after the diode, voltage dividers were used. The resistors were chosen in such a way that at 40V across the complete divider, ± 1V would be measured across the 5.1K resistor. If the charger is turned off, the voltage divider after the diode (R13, R14, R15) can be used to measure the voltage across the battery.

To measure the temperature, two NTC resistors were used. These were retrieved from the removed PCB of the electronic bike. Because of this I don’t know the exact specifications, but after some testing with a thermometer (and icy and boiling water) I found resistors which would allow measurements from around 3 degrees Celsius to about 100 degrees Celsius. 
One NTC resistor is used to measure the temperature of the heatsink to which the LM317 is fixed. The other NTC resistor is used to measure the battery temperature. Measuring the battery temperature was possible because the XLR plug which is used to connect the charger to the battery contains three pins. One plus, one ground leaving the third pin available to attach to it, the NTC resistor.

The ESP has only one available input for analog signals, namely GPIO5. But we have four sensory inputs. To solve this problem, the SN74HC405 multiplexer was used. The digital input pins A, B and C are used to select to which of the eight option pins the COM_OUT/IN pin is connected. Since we only use four option pins, we only need A and B.

***Other***  
A fan is used to cooldown the LM317. When the temperature is too high, the fan is turned on and remains on until the temperature reaches a certain threshold. This is implemented using a MOSFET. A flyback diode was also used to prevent a sudden voltage spike of breaking the fan when the fan is turned off.

Other than that we have incorporated a reset and flash button to facilitate flashing the ESP (pull-up). Input switch (S3) is used to be able to turn off the charger and LED D3 is used to indicate the state of charging (waiting, charging or stopped). LED D2 emits light only when the charger is turned on. 
The YP-05 chip is connected using female header connector pins. The chip can be removed once finished flashing the ESP.

## How does the Software work?

![State Diagram of the charger software](https://user-images.githubusercontent.com/45215498/78668616-42140280-78db-11ea-912a-23a9f3629552.png)

The charger can be in three different states (0, 1, 2). Either ‘Waiting’, ‘Charging’ or ‘Charge stopped’ as can be seen in the following state diagram:

The software resolves around the stateMachineRun() method. Using the Ticker library we have created a state ticker which ticks every 0.1 seconds. Once such a tick occurs we update the time-variables based on the current state of the charger. For example in state 0, at every state tick we will update the timer variable using the timeUpdate() method. If a certain time threshold has passed we will call the checkStart() method which was supplied as input to the timeUpdate() method.

When we are busy charging (state 1), we will call the checkStop() method every 0.4 seconds. In this method we check if one of the following stop conditions are met:
-	Turned off by user (by means of switch S3)
-	Max battery temperature reached
-	Max heatsink temperature reached
-	Battery temperature rising too fast
-	Negative battery voltage drop exceed threshold
-	Max battery voltage reached

Besides these conditions also a timer was added. After 12 hours of charging, the charger will be automatically turned off.

In special circumstances it might occur that the fan isn’t able to cooldown the LM317. To make sure the charger can still continue, a pulsating mode was added. When the temperature reaches a certain threshold, the charger will be turned off for a small part of each cycle. The higher the temperature the longer the turned-off part of the cycle will be (similar to PWM). In normal circumstances the calculatePulsatePause() method will return 100% as the charger can be remained on for the full duration of the cycle. Once the charger heats up too much this percentage will go down and ultimately end up at 0%.

Looking back at the software I see room for improvement. The project started of as a very simple charger but gradually more components were added to make it better. Because of this structure-wise improvements can definitely be made.

## Pictures of the charger
On the first picture the main components of the charger are indicated.
![layoutcharger](https://user-images.githubusercontent.com/45215498/78689577-c58f1d00-78f6-11ea-984b-e2da20e5bd73.jpg)

The other pictures are here to give a better idea of the shape of the device.
![20200407_161808](https://user-images.githubusercontent.com/45215498/78689527-b90ac480-78f6-11ea-8fdb-655c74a88f56.jpg)
And the back:
![backcharger](https://user-images.githubusercontent.com/45215498/78691123-937eba80-78f8-11ea-901c-377634548c46.jpg)
