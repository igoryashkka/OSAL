# BSP (Board Support Package)

This folder contains hardware abstraction layer and board-specific drivers.

## Folder Structure

### `/drivers/`
Low-level drivers for:
- ADC (Analog-to-Digital Converter)
- UART (Serial communication)
- SPI (Serial Peripheral Interface)
- I2C (Inter-Integrated Circuit)
- Timers
- Interrupt handlers

Each driver abstracts the MCU-specific implementation and provides a clean API.

### `/config/`
Board-specific configuration:
- Pin definitions
- Clock frequencies
- Peripheral configurations
- MCU selection (STM32F103, STM32H750)

## Usage

BSP drivers are used by:
- Platform API layer (wraps BSP drivers)
- Application modules
- Task implementations

## Example: Adding a New Driver

1. Create driver in `/drivers/` with generic API
2. Implement MCU-specific code for both STM32F103 and STM32H750
3. Use Platform API as the abstraction layer
