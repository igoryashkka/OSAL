# OSAL Project Structure

## Directory Organization

```
OSAL/
├── app/                          # Application layer
│   ├── main.c                    # Entry point
│   ├── config/                   # Application configuration
│   ├── init/                     # Initialization routines
│   │   ├── application.h/c       # App init and task creation
│   │   └── README.md
│   ├── tasks/                    # Task implementations
│   │   ├── tasks_peripheral.c/h  # LED, battery monitoring
│   │   ├── tasks_comms.c/h       # GPS, Ethernet, Radio
│   │   ├── tasks_stats.c/h       # Statistics and monitoring
│   │   └── README.md
│   └── README.md
│
├── BSP/                          # Board Support Package (Hardware Abstraction)
│   ├── drivers/                  # Low-level drivers
│   ├── config/                   # Board-specific configuration
│   └── README.md
│
├── Platform_API/                 # MCU-agnostic platform APIs
│   ├── Platform_GPIO/
│   ├── Platform_UART/
│   ├── Platform_Clock/
│   ├── Platform_Flash/
│   └── (extends across STM32F1 and STM32H7)
│
├── modules/                      # Application modules
│   ├── blink/                    # LED control
│   ├── system/                   # System initialization
│   ├── scheduler/                # Task scheduler
│   ├── gps_nmea/                 # GPS NMEA parser
│   └── flash/                    # NVM/EEPROM operations
│
├── platform/                     # Platform-specific implementations
│   ├── stm32/
│   │   ├── common/               # Shared STM32 code
│   │   ├── impl/
│   │   │   ├── stm32f1/          # STM32F103 specific
│   │   │   └── stm32h7/          # STM32H750 specific
│   │   ├── ld/                   # Linker scripts
│   │   └── startup/              # Startup code
│   └── stm8/                     # STM8L151 scaffold (startup/linker)
│
├── cmake/                        # Build configuration
│   ├── mcu/                      # MCU-specific CMake
│   └── toolchains/               # Compiler/toolchain setup
│
├── vendors/                      # Third-party libraries
│   └── README.md
│
├── include/                      # Shared project headers
│   └── README.md
│
├── tools/                        # Build and utility scripts
│   ├── build.sh
│   └── clean.sh
│
├── CMakeLists.txt               # Main build configuration
└── README.md                    # Project documentation
```

## Key Features

### Application Layer (`app/`)
- **main.c** - Clean entry point calling initialization and scheduler
- **config/** - Configuration constants and parameters
- **init/** - Platform and subsystem initialization
- **tasks/** - Task definitions organized by function:
  - Peripheral tasks (LED, battery)
  - Communication tasks (GPS, Ethernet, Radio)
  - Statistics & monitoring tasks

### Abstraction Layers

1. **Platform_API/** - MCU-agnostic APIs
   - Used by application tasks
   - Implemented for each MCU variant

2. **BSP/** - Board Support Package
   - Hardware drivers
   - Pin/clock configuration
   - MCU-specific implementations

3. **modules/** - Reusable components
   - Task Scheduler - Non-preemptive, priority-based
   - GPS NMEA - NEO-6M GPS protocol
   - System - Clock and platform initialization
   - Blink - LED control

## Build Process

1. **CMake** selects MCU (STM32F103 or STM32H750)
2. **Toolchain** configures compiler (arm-none-eabi-gcc)
3. **Platform implementations** selected based on MCU
4. **Build** produces firmware for target MCU

## Adding New Features

### New Task
1. Implement task function in appropriate file under `app/tasks/`
2. Add function declaration to corresponding header
3. Register in `app_start_tasks()` (app/init/application.c)

### New Module
1. Create folder under `modules/`
2. Implement module API
3. Use Platform_API for MCU operations (not direct HAL)

### New Driver
1. Create generic driver in `BSP/drivers/`
2. Implement for STM32F1 and STM32H7
3. Wrap with Platform_API if used by application

## Code Flow

```
main() 
  ↓
app_init() 
  ├─ init_system()
  ├─ blink_init()
  ├─ SCHED_Init()
  └─ GPS_Init()
  ↓
app_start_tasks()
  ├─ Create task_led_blink (500ms, NORMAL)
  ├─ Create task_gps_process (100ms, HIGH)
  ├─ Create task_battery_monitor (5s, NORMAL)
  └─ Create task_statistics (10s, LOW)
  ↓
SCHED_Run()
  └─ Loop forever, running tasks based on priority and time
```

## Project Conventions

- **Task priorities**: CRITICAL (3), HIGH (2), NORMAL (1), LOW (0)
- **MCU agnostic**: Use Platform_API, not direct HAL
- **Return values**: Tasks return next execution interval in ms
- **Documentation**: Each module has README with API and usage
- **Headers**: Public APIs in `.h` files, implementation in `.c` files

## Build Commands

```bash
# Build for STM32F103
./tools/build.sh f103

# Build for STM32H750
./tools/build.sh h750

# Clean build artifacts
./tools/clean.sh
```
