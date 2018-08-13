# PID Controller
---

A program to control a car in a simulator using PID controller. This program also has automatic parameter tuning for the PID controller. To enable parameter tuning, set `tune_params = true`


## Note: 
1. This project uses a simulator. You can download it from [here](https://github.com/udacity/self-driving-car-sim/releases)
2. The program communicates with the simulator using uWebSocket which can be set up by running
   * `install-mac.sh` on macOS
   * `install-ubuntu.sh` on ubuntu


## Basic Build Instructions

1. Clone this repo.
2. Make a build directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./pid`. 
