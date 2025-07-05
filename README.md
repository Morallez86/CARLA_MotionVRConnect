# Carla VR Chair Addon

This extension is built on top of **CARLA 0.9.15** and is based on the **developer version** of CARLA.

## Overview

The addon introduces new functionalities and assets to enhance your CARLA simulation experience. It extends the base CARLA framework with additional blueprints, Python APIs, and scenario tools.

> **Note:** This project is intended to run **only within the Unreal Editor**.  
> It is not designed to be **packaged** or **shipped** as a standalone executable.

## Installation

To add these functionalities to your existing CARLA setup:

1. Download the source code for **CARLA 0.9.15** from the official releases page:  
   [https://github.com/carla-simulator/carla/releases](https://github.com/carla-simulator/carla/releases)

2. Follow the official tutorial to build CARLA on **Windows**:  
   [https://carla.readthedocs.io/en/0.9.15/build_windows/](https://carla.readthedocs.io/en/0.9.15/build_windows/)

3. After building, **verify that CARLA 0.9.15 is working properly**:  
   - Ensure that the project compiles without errors  
   - Confirm that the **Unreal Editor** opens successfully and loads the CARLA project

4. Once your CARLA environment is confirmed working, copy the folders from this extension directly into your CARLA installation directory.  
   Specifically, merge or place the following folders:
   - `Unreal`  
   - `PythonAPI`  
   - `LibCarla`  
   - `ScenarioTown02Maker`  

   Make sure to preserve the folder structure when copying.

5. Use the following commands in cmd in your carla folder, to make a new compilation (avoid using **make rebuild**):
   - make clean
   - make libcarla
   - make pythonapi
   - make launch

6. Follow the guidelines presented in the pdf files in this same directory through this order:
   - VR Setup Guide for Carla VR Chair Addon
   - Vehicle Control Setup for Carla VR Chair Addon
   - Chair Setup for Carla VR Chair Addon

   These documents provide step-by-step instructions to configure VR, vehicle controls, and motion chair support.

## Requirements

- CARLA version **0.9.15**
- Developer build (built from source)
- Windows 10 or later

## Notes

- This project has **not been tested on Linux**.
- It is designed specifically for the **Windows developer build** of CARLA 0.9.15.
- Back up your existing CARLA folders before applying this addon, to avoid overwriting important files.
- If you encounter issues related to **CARLA itself** (not this extension), it is recommended to check the official CARLA issue tracker:  
  [https://github.com/carla-simulator/carla/issues]
  CARLA has an active community, and many issues already have fixes or workarounds.

---

Feel free to report issues or contribute improvements to this project!
