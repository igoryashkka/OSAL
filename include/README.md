# Shared Include Files

This folder contains common header files shared across the entire project.

## Purpose

- Common type definitions
- Project-wide constants
- Macros and utilities
- Interface definitions

## Usage

Files in this folder should be included with:
```c
#include "filename.h"
```

Avoid deep nesting - keep the hierarchy flat and simple.

## Guidelines

- Only project-wide definitions belong here
- Module-specific headers stay in their modules
- Keep include files focused and minimal
