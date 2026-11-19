Obstacle Avoiding Robot Car

Project Overview

This project is a two-wheel-drive robot car built around an ESP32. 
It drives forward on its own, measures the distance ahead with an ultrasonic sensor, 
and steers away from obstacles without any human input. 
It combines mechanical design, electronics, and embedded programming in one working system.

Objectives

Build a working autonomous vehicle on an ESP32.
- Detect obstacles in real time with an ultrasonic sensor.
- Control motor direction and speed through a motor driver.
- Integrate a custom CAD-designed chassis, wiring, and firmware.

Team

| Tajrian Rahman | Wrote and tested the firmware |
| Raiyan Parvez | Designed the chassis in CAD |
| Fazle Rabbi | Assembled the car and did the wiring |

Hardware Components

| Component | Purpose |
| ESP32 dev board | Main controller |
| HC-SR04 ultrasonic sensor | Measures distance to obstacles |
| 2× DC motors with wheels | Drive and steering |
| L298N motor driver | Controls motor direction and speed |
| 3× 3.7V Li-ion cells (3S holder) | Power supply |
| Breadboard, jumper wires | Connections |
| Resistors | Voltage divider on the sensor ECHO line |

Mechanical Design

The chassis was designed in CAD to hold the motors, the electronics, the battery holder, and a front-mounted ultrasonic sensor. 
All design files are in the `cad/` folder.

How It Works

1. The ESP32 drives both motors forward and continuously reads the sensor.
2. When an obstacle is closer than 20 cm, the car stops and reverses briefly.
3. It turns right and checks the distance. If clear, it drives on.
4. If blocked, it turns left past the centre and checks again.
5. If both sides are blocked, it turns around and continues.

Features

- Fully autonomous obstacle avoidance
- Reverse-and-turn recovery when blocked on both sides
- Adjustable speed, turn time, and detection distance
- Serial monitor output for testing

Motor and Driver Selection

- DC motors with wheels: Simple, low cost, and give enough torque and speed for a light chassis. 
  Direction and speed are easy to control with PWM.
- L298N driver: The ESP32 pins cannot drive motors directly. The L298N has two H-bridge channels, 
  so it controls both motors' direction and speed independently. 
  It handles about 2A per channel and a 5-35V supply, which suits the Li-ion pack. 
  It is cheap, widely available, and well documented. Its onboard 5V regulator can also power the ESP32.

Wiring

Motor driver to ESP32

| L298N | ESP32 |
| ENA | GPIO 14 |
| IN1 | GPIO 27 |
| IN2 | GPIO 26 |
| IN3 | GPIO 25 |
| IN4 | GPIO 33 |
| ENB | GPIO 32 |

Ultrasonic sensor to ESP32

| HC-SR04 | ESP32 |
| VCC | 5V |
| TRIG | GPIO 5 |
| ECHO | GPIO 18 (through a voltage divider) |
| GND | GND |

Power and motors

- Battery positive to L298N 12V input, battery negative to L298N GND.
- L298N 5V output to ESP32 5V pin. Connect all grounds together.
- Left motor to L298N OUT1/OUT2, right motor to OUT3/OUT4.

Setup Instructions

1. Assemble the chassis from the files in `cad/` and mount the motors and wheels.
2. Fit the sensor at the front of the chassis.
3. Wire everything using the tables above.
4. Install the Arduino IDE and ESP32 board support.
5. Place the car on the floor and switch on the battery.

Change these constants at the top of the code: `SPEED`, `TURN_SPEED`, `SAFE_CM`, `BACK_MS`, `TURN_MS`.

Youtube demonstration link: (https://youtube.com/shorts/SYV-s3gRsK0?si=b3cIg3lfCSRtbNd0)
Future Improvements

- Add a servo to scan left and right with the sensor
- Add side sensors for smoother avoidance
- Add Wi-Fi or Bluetooth manual control
