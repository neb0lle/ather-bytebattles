# Changelog

## Release v0.1.0 - 2024-05-09

<!--
### Unreleased

* Integration of Unity framework for unit test cases.
* Integration of coding style guide check.
* External EEPROM abstraction in middleware.
-->

### Added

* Following platforms have been added.

    |Platform|Arch.|CPU Core|FPU|
    |--------|----|--------|------|
    |CYT2B75_M0PLUS|Armv6-M |Cortex-M0+ |No |
    |CYT2B75_M4    |Armv7-M |Cortex-M4  |Yes|

* Following Driver Abstraction Layer (DAL) for CYT2B75 have been added.
    - ADC
        - MCU PIN level abstraction.
        - Non-blocking APIs .
        - Interrupt control with priority.
        - ADC group conversion.
        - DAL managed:
            - MCU PIN level callback on interrupt.
            - ADC clock settings.
            - ADC pin settings.
        - Todo:
            - [ ] Blocking APIs.
    - CAN
        - Channel level abstraction.
        - Classic CAN with 11-bit (standard) CAN identifier.
        - Non-blocking implementation only.
        - Interrupt control with priority.
        - Hardware FIFO (RxFIFO) support to receive messages.
            - Accept all.
            - Accepting messages with matching IDs.
        - DAL managed:
            - Channel level callback on interrupt.
            - CAN clock settings.
            - CAN pin settings.
            - Fixed pre-scaler to ensure time quanta (tq) is constant with different baudrates.
        - Todo:
            - [ ] Classic CAN with 29-bit (extended) CAN identifier.
            - [ ] Designated mailboxes for transmission and reception.
            - [ ] CAN with flexible datarate (CANFD).
        > **Note:**
        > - Use Mailbox #0 for all transmissions.
        > - Use callback function to read the received CAN message. Function `asdk_can_read` is not implemented because it must handle both RxFIFO and designated mailbox configuration for reading the message.
    - Clock
        - Supports source selection between internal or external crystal.
        - PLL configuration is derived based on the output & input frequency to the PLL block.
        - Clock enable API to enable peripheral clocks within DAL.
        > **Note:**
        > - Trim settings for external crystal uses `xtal_trim_config_t` data structure. It is specific to CYT2B75 platform and is not generic & is subjected to change.
        > - The `asdk_clock_enable` function may share clock dividers between peripherals if the divider value is same.
    - GPIO
        - MCU Pin level abstraction.
        - Interrupt control with priority.
        - Designated APIs for input and output pin functions.
        - DAL managed:
            - GPIO pin settings.
            - Global level callback on interrupt.
    - I2C
        - Channel level abstraction.
        - Interrupt control with priority.
        - Non-blocking APIs.
        - Supports master mode.
        - DAL managed:
            - I2C pin settings.
            - I2C clock settings.
            - Channel level callback on interrupt.
            - SCB block as I2C subjected to availability.
        - Todo:
            - [ ] Blocking APIs.
            - [ ] Slave mode.
            - [ ] Master-Slave mode.
        > **Note:** High speed mode (ASDK_I2C_TRANSFER_RATE_3_4mbps) is not handled yet.
    - PWM
        - Channel level abstraction.
        - Supports Left, Center aligned PWM.
        - Interrupt control with priority.
        - User configurable PWM peripheral clock frequency.
        - DAL managed:
            - PWM pin settings.
            - Channel level callback on interrupt.
        - Todo:
            - [ ] Right Edge aligned PWM yet to be validated.
    - SPI
        - Channel level abstraction.
        - Interrupt control with priority.
        - Non-blocking APIs.
        - Supports master mode.
        - DAL managed:
            - SPI pin settings.
            - SPI clock settings.
            - Channel level callback on interrupt.
            - SCB block as SPI subjected to availability.
        - Todo:
            - [ ] Blocking APIs.
            - [ ] Slave mode.
        > **Note**: The callback is made on transfer done event only.
    - System
        - Software reset.
        - Enable & disable interrupts globally.
        - Read reset reason.
        - Read core clock frequency.
        - Todo:
            - [ ] System timer support with user configurable timer.
            - [ ] Get system time in milliseconds based on `SYS_TIMER` or `USE_RTOS` option.
            - [ ] Abstract reset reasons.
    - Timers
        - Channel level abstraction.
        - Mode based timer abstraction (Timer, Compare & Capture).
        - User configurable timer peripheral clock frequency.
        - Interrupt control with priority.
        - DAL managed:
            - Capture pin settings.
            - Channel level callback on interrupt.
            - Timer type callback for both Timer & Compare mode.
            - Capture type callback for Capture mode.
    - UART
        - Channel level abstraction.
        - Interrupt control with priority.
        - Supports standard baudrates.
        - Non-blocking APIs.
        - DAL managed:
            - UART pin settings.
            - UART clock settings.
            - Channel level callback on interrupt.
            - SCB block as UART subjected to availability.
        - Todo:
            - [ ] Blocking APIs.
            - [ ] Support for CTS & RTS pins.
    - Watchdog
        - Supports independent and window watchdog.
        - Supports reading the current watchdog time in ms.
        - Supports warn action when the warn timeout limit is reached.
        > **Note:**
        > - Use WDT no. #0 for WDT operations.

* Following Middleware modules have been added.

    - CAN Services
        - CAN Services is carried over from ASDK.
        > **Note:** Length of queue is fixed to 40, planning to make it user configurable.
    - RTOS
        - uC-OS3 support carried over from ASDK.
        > `USE_RTOS` cmake option to enable/disable RTOS from build.
    - UDS stack (including CANIL)
        - Carried over from ASDK.
        > `USE_UDS` cmake option to enable/disable UDS from build.

* Following user guides have been added.
    - GPIO
    - CAN
    - Timer
    - ADC
    - PWM
    - Application configuration
    - ASDK Configuration

* Following developer guides have been added.
    - Build system
    - Setup script

<!--
### Changed
### Deprecated
### Removed
### Fixed
### Security
-->