# WALLE-firmware

Arduino Mega firmware for the **W.A.L.L.E.**

Part of the *Wireless Algorithmic Listen & Look Explorer* capstone project (Toronto Metropolitan University, ECE, 2025–2026). The emotion recognition software that drives this firmware lives in [WALLE-va-recognition](https://github.com/marpbone/WALLE-va-recognition); the project overview and photos are in [WALLE](https://github.com/marpbone/WALLE).

---

## What it does

A Raspberry Pi classifies the user's emotional state and sends a mood packet over the serial communication channel. This firmware receives it, pushes it into a FIFO queue, and plays an animation across seven servos and 2 motors controlling head, neck, eyes, arms, and wheels. Motion runs through a physics-based controller with velocity and acceleration limits so movement reads as smooth rather than mechanical. When no mood arrives for long enough, the robot falls into idle behaviours on its own. The robot has capability to be remotely controlled with an xbox controller using flask.

## Serial protocol

Four raw bytes at **115200 baud**, in order:

| Byte | Field | Values |
|---|---|---|
| 0 | `mood` | 0 no read, 1 angry, 2 disgust, 3 fear, 4 happy, 5 sad, 6 surprised, 7 neutral 
| 1 | `source` | 00 no source, 01 visual input only, 02 audio input only, 03 both 
| 2 | `confidence` | fusion confidence, compared against `CONF_THRESHOLD` which can be adjusted as per accuracy requirements |
| 3 | `landmark` | whether or not a facial landmark is detected (if haar cascade returns true) |

Moods land in a 10-deep queue (`MOOD_QUEUE_SIZE`); the animation queue holds 40 frames (`QUEUE_LENGTH`).

## Pin mapping

| Pin | Function |
|---|---|
| 12 / 13 | Motor direction, left / right |
| 3 / 11 | Motor PWM speed, left / right |
| 9 / 8 | Motor brake, left / right |
| 10 | PCA9685 servo shield output enable |
| A2 | Battery level, through a 100 kOhm / 47 kOhm divider (scaling 0.3197) |
| 23, 37 | Source indicator LEDs |
| 27, 29, 31, 33 | Mood indicator LEDs |
| 7 | Speaker amplifier enable |
| SDA / SCL | PCA9685 driver and SH1106 OLED (I2C) |

Battery range is configured for a 3S pack: 12.6 V full, 10.2 V empty.

## Servo calibration

Servo positions are normalized 0-100 in the animation code and mapped to PCA9685 pulse counts:

| Index | Joint | Low, High | Range |
|---|---|---|---|
| 0 | Head pan | 180, 500 | 70* - 140* |
| 1 | Upper neck | 300, 450 | 50* - 145* |
| 2 | Lower neck | 183, 458 | 30* - 160* |
| 3 | Right eye (MG90D) | 390, 490 | 40* - 120* |
| 4 | Left eye (MG90D) | 390, 220 | 125* - 60*, reversed |
| 5 | Left arm | 500, 350  | 75* - 135* |
| 6 | Right arm | 300, 550 | 135* - 75* |

The values are specific to this chassis and servo horns; wrong limits will drive a servo into a mechanical stop and burn it out. Several servos were lost this way during development.

The PCA9685 must run at **50 Hz**, not the library default of 60 Hz; a mismatch shifts every servo away from its intended angle.

## Files

| File | Purpose |
|---|---|
| `wall-e.ino` | Main loop, serial parsing, mood evaluation, servo and motor control |
| `animations.ino` | Animation sequences (`playAnimation(int)`), one per mood plus idles |
| `display.ino` | SH1106 OLED battery and status rendering |
| `MotorController.hpp` | Velocity- and acceleration-limited motion controller |
| `Queue.hpp` | Fixed-size FIFO used for the mood and animation queues |
