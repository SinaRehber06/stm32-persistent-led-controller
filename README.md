# STM32 Persistent LED Controller

A simple embedded systems project developed using an STM32 microcontroller and the STM32 HAL library. The project controls the number of LED blinks using a push button and stores the selected value permanently in the microcontroller's internal Flash memory.

## Overview

This project demonstrates several fundamental embedded systems concepts, including GPIO control, timer interrupts, button input handling, long and short button press detection, and non-volatile data storage using internal Flash memory.

The user can configure the LED blink count between 4 and 7 using a push button. The selected value is stored in Flash memory, allowing the configuration to persist even after the microcontroller is restarted or powered off.

## Features

- Configurable LED blink count
- Short button press detection
- Long button press detection
- Timer-based LED control
- Timer interrupt implementation
- Internal Flash memory storage
- Persistent configuration after restart
- Startup reset functionality
- LED feedback for successful reset
- STM32 HAL-based implementation

## How It Works

When the microcontroller starts, the previously stored blink count is read from the internal Flash memory.

The blink count can have one of the following values:

```text
4 → 5 → 6 → 7 → 4

The exact project structure may vary depending on the STM32CubeIDE and STM32CubeMX project configuration.

Technologies
C
STM32
STM32 HAL
STM32CubeIDE
STM32CubeMX
GPIO
TIM2
Timer Interrupts
Internal Flash Memory
Embedded Systems
Concepts Demonstrated

This project provides practical experience with:

Embedded C programming
STM32 microcontroller programming
GPIO configuration
Digital input and output
Timer configuration
Interrupt-driven programming
Button press detection
Short and long press handling
LED control
Flash memory operations
Non-volatile data storage
Basic state-based control
Example

If the current blink count is 4, the LED performs the configured blinking sequence.

After pressing the button once:
Blink Count = 5
After pressing it again:

Blink Count = 6

Another press:

Blink Count = 7

The next press returns the value to:

Blink Count = 4
The selected value is stored in Flash memory after each change.
Reset Behavior
The system provides two ways to reset the blink count:
Hold the button for approximately 3 seconds during normal operation.
Hold the button for approximately 3 seconds during startup.
In both cases, the blink count is restored to:
4
The LED remains ON for approximately one second after a successful startup reset to indicate that the operation was completed.
Purpose
The project was developed as a practical embedded systems study to demonstrate how a microcontroller can combine user input,
timer interrupts, LED control, and persistent memory storage in a single application.

License

This project is intended for educational and development purposes.
