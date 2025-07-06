# ScenarioTown02Maker

This tool is built to work with **CARLA 0.9.15** and allows the user to generate custom simulation scenarios on the **Town02** map using a simple JSON configuration file.

## Overview

**ScenarioTown02Maker** is designed to automatically spawn vehicles and pedestrians, assign routes, and define behaviors using a structured and editable JSON file.  
It is inspired by the structure of CARLA’s ScenarioRunner, but provides a more streamlined and user-friendly approach.  

While it integrates seamlessly with the **CARLA VR Chair Addon**, it can also be used independently. When used without the addon, modules such as the pedestrian detection sensor and V2V broadcast sensor can simply be disabled.

> **Note:** This script requires a **CARLA server instance running in Town02**.  
> It does not run inside the Unreal Editor and must be executed as a standalone Python script.

## Features

- Automatic spawning of vehicles and pedestrians at user-defined positions
- Assignment of custom routes to each actor
- Optional configuration of actor behavior (e.g., stop or continue at end of route)
- Integration of V2V modules and pedestrian detection sensors
- Safety distance checks between actors and spectator vehicle
- Modular architecture for easy extension and maintenance

## Structure

The tool is divided into several Python modules for clarity and maintainability:

- `main.py`: Entry point. Connects to the CARLA server, loads the scenario configuration, and launches the simulation loop.
- `scenario_executor.py`: Core logic for spawning actors, assigning behaviors, managing sensors and references.
- `scenario_parser.py`: Parses and validates the JSON file and loads it into Python data structures.
- `utils/`: Contains helper modules:
  - `scenario_utils.py`
  - `walker_route_manager.py`
  - `walker_utils.py`

## Installation

1. Make sure CARLA 0.9.15 is correctly built and running. You should be inside the simulator (Town02 map loaded).

2. Copy the `ScenarioTown02Maker` folder into your CARLA root directory.

3. Run the script using a terminal or command prompt:

   python main.py

## Spawn Point Reference
The directory `carla-vr-chair-addon/ScenarioTown02Maker/Town02SpawnPoints` contains an image showing available vehicle spawn points in the Town02 map.
Pedestrians are spawned on sidewalks near each vehicle’s spawn point. However, not all spawn points are valid for walkers due to collisions with nearby objects.
To determine which points are unsuitable for pedestrians, refer to `carla-vr-chair-addon/ScenarioTown02Maker/utils/walker_utils.py`, where such points are programmatically filtered and documented.

## Metrics
The metrics directory contains visual performance data collected using Unreal Session Frontend.
These include:

- Line charts showing the evolution of performance metrics (Game Thread, Render Thread, and GPU) over time;
- Histograms that represent the frame rate (FPS) distribution in each test scenario.

Each file name reflects the test configuration using the following abbreviations:
SH: Without complementary hardware
CH: With complementary hardware (YawVR, Logitech G923)
V: Number of vehicles
P: Number of pedestrians

For example, CH_3V_3P refers to the test with hardware enabled and a scenario containing 3 vehicles and 3 pedestrians.

## Notes

There are two notable limitations when integrating this script with the CARLA VR Chair Addon:

 - Vehicle possession:
   CARLA typically uses pygame to allow user input on a newly spawned vehicle. In the VR addon, possession is done via Blueprint on BeginPlay, which causes CARLA to treat the controlled vehicle as a spectator. Consequently, the Traffic Manager does not detect it as a valid vehicle.
   To address this, the scenario script includes a configurable safety distance that makes other vehicles stop when approaching the commanded vehicle.

 - Pedestrian detection:
   CARLA’s AI walkers are controlled by a third-party library and follow predefined crossing rules. However, the pedestrian sensor used in the addon cannot detect AI-controlled walkers unless they are walking freely (without crossing zones).
   Although AI behavior is basic and can result in stuck walkers, it also enables full control over their routes, allowing them to cross roads without constraints.

## Example Configuration

An example file is available at `ScenarioTown02Maker/config/sample_scenario.json`.


