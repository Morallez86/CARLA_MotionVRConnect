# CARLA_MotionVRConnect

This extension is built on top of **CARLA 0.9.15** and is based on the **developer version** of CARLA.

## Overview

CARLA_MotionVRConnect is an extension of the CARLA 0.9.15 simulator designed to support immersive, human-in-the-loop simulations using virtual reality, a motion platform (YawVR), and physical driving hardware such as the Logitech G923 racing wheel and pedals.
The addon introduces new Unreal Engine Blueprints, C++ classes, and Python scripts to enable:
- VR camera integration with seated view and head tracking
- Real-time control of a motion chair (yaw, pitch, roll) based on vehicle dynamics
- Manual driving with input from steering wheel and pedals
- Custom pedestrian detection and V2V communication sensors
- Adaptive sound cue systems to enhance spatial awareness
- A 2D minimap and speedometer displayed in the vehicle HUD
- Modular scenario creation via the ScenarioTown02Maker tool

> **Note:** This project is intended to run **only within the Unreal Editor**.  
> It is not designed to be **packaged** or **shipped** as a standalone executable.

### Demo Video

[![Watch the demo](https://img.youtube.com/vi/VEfPHTE/0.jpg)](https://www.youtube.com/watch?v=VEfPHTE_M3I)

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
   - make LibCarla
   - make PythonAPI
   - make launch

6. Follow the guidelines presented in the pdf files in this same directory through this order:
   - CARLA_MotionVRConnect_VR_Setup_Guide.pdf
   - CARLA_MotionVRConnect_Vehicle_Control_Setup.pdf
   - CARLA_MotionVRConnect_Chair_Setup.pdf

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
