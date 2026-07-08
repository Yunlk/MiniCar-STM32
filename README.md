# MiniCar-STM32

STM32 firmware for the miniature car side of the CarTwinViewer digital-twin demo.

This project runs on an STM32F103C8 board. It controls basic car peripherals, sends the car position to the PC viewer over UART, and receives obstacle position data from the PC viewer.

## Project Role

- STM32 lower-computer firmware
- Keil MDK project
- Uses STM32F10x standard peripheral library style APIs
- Sends local car coordinates to the PC host application
- Receives obstacle coordinates from the PC host application

The matching host project is `CarTwinViewer-PC`.

## Directory Layout

```text
HAL/        Peripheral drivers such as UART, motor, LED, key, and buzzer
SYS/        System and delay support code
USER/       Application-level car logic and main loop
MDK/        Keil MDK project files
OBJ/        Build output directory
```

## Target

- MCU: STM32F103C8
- Toolchain: Keil MDK
- UART: USART2
- UART pins: PA2 TX, PA3 RX
- Baud rate: 9600

## Main Behavior

- Initializes delay, buzzer, keys, LEDs, motors, UART, and car state.
- Supports manual mode and auto mode flags.
- In manual mode, key input changes the local lane state and sends car position to the PC viewer.
- In auto mode, the firmware is intended to use received obstacle data for avoidance logic.

## Serial Frame

The current protocol follows the original teaching project format:

```text
%xxx\0yyy$
```

The firmware sends the car position in this format and parses obstacle position data received from the PC viewer.

## Current Limitations

- Automatic obstacle avoidance is not fully implemented.
- Some lane-change helper logic is still incomplete.
- UART receive parsing assumes well-formed frames.
- Generated Keil/J-Link files should be kept out of normal source commits unless intentionally needed.
- This README documents the current project state only; no firmware behavior has been changed as part of this README pass.
