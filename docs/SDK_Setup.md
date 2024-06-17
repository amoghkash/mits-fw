# ESP-IDF SDK Setup

This document goes over how to setup your development IDE and make sure all settings remain the same through releases. Any changes made in menuconfig to the application must be recorded here

## How to setup ESP-IDF

1. Make sure you have VSCode Installed
2. Download the ESP-IDF extension from Espressif Systems
3. Setup the build environment on your system, following the instructions. You might have to install some other tools like ninja and make during setup. Use the latest stable IDF version.
4. When you open an ESP project, it should setup and work automatically.

## Build System Configurations + Changes

### How to make changes to configurations

To make changes to the build system configurations, use the menuconfig GUI. This can be accessed by using the settings button.

### Build System Configurations

1. USB Setup
    1. Enable TinyUSB CDC feature
2. Partition Setup
    1. Select the "Custom partition table CSV" option in the "Partition Table" Field
