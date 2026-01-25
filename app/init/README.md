# Application Initialization

This folder contains initialization and setup code for the application.

## Files

- `application.h` - Application initialization API
- `application.c` - Platform and subsystem initialization, task creation

## Functions

- `app_init()` - Initialize all platform subsystems (platform, scheduler, GPS, etc.)
- `app_start_tasks()` - Create and start all application tasks

The initialization flow is clearly defined and can be extended with new subsystems.
