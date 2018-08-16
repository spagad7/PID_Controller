# PID Controller
---

A program to control a car in a simulator using PID controller. This program also has automatic parameter tuning for the PID controller. To enable parameter tuning, set `tune_params = true`

## Working of PID Controller

Imagine that we are driving a car and we want to follow a certain line. If we are on the left of the line then we steer the car towards the right, and if we are on the right of the line then we steer left. The amount of steering input we give is proportional to the distance from the line we want to follow. PID controller is designed to do the exact same job. PID stands for Proportional Integral and Differential. The working of these 3 components is described below

The P(proportional) component causes the car to steer proportional to the error (distance from the desired trajectory, also called `cross track error (cte)`), that is, the car turns harder the further away it is from the desired trajectory. Therefore `steering angle = -P * cte`, where P is the proportinal gain. However, proportional controller doens't work that well on it's own; with proportional controller, the car can be crooked when it enter the desired trajectory causing it to repeatedly overshoot the desired path and follow a wavy path along the desired trajectory.

To correct this overshooting problem we use the D(differential) component. The differential component considers error rate, that is, how fast or slow the car is moving towards the desired trajectory. If the car is perfectly following the desired trajectory, the cross track error will be zero, as the change of error with respect to time is zero. The differential component is implemented by calculating the differnce between the cte of the previous and the current time frame and scaling it with differential gain D, that is, `steering angle = -D * delta_cte`. Conceptually we can think that increasing the proportional gain P will increase the pull the car feels towards the desired trajectory, whereas increasing the differential gain D increases the ressistance the car will feel in moving towards the trajectory. When the propertional and differential gains are perfectly balanced the car can smoothly move towards the desired trajectory without overshooting. However, there is still one drawback with using only propertional and differential components. So far, we have assumed that the car is in perfect/ideal working condition, however, this is not the case in reality. There may be a slight offset in the steering angle because of some mechanical fault, which causes the car to build up a lane offset from the desired trajectory.

To counteract on the lane offset problem caused by imperfect mechanical conditions of real cars, we use I(integral) component. This component considers the sum of cross track errors the car has seen so far and scale it with integral gain I, that is, `steering angle = -I * sum_cte`. So if the car is spending a long time on the one side of lane, then the sum of cross track error increases, and the integral term will act on this error and causes the car to move towards the desired trajectory.

The total effect of the all the three components is captured in the below equation

`steering angle = (-P * cte) + (-D * delta_cte) + (-I * sum_cte)`

The P, I and D gains should be perfectly tuned for the car to follow the desired trajectory.


## Final Parameters

Tuning the P, I and D gains can be taxing task. This can be either done manually by tuning each gains and independently and then choosing a combination which works the best, or by automating this process using Twiddle algorihtm. I chose to automate the tuninng process with a little initial manual tuning. In short, the twiddle algorithm (auto-tune) tunes the `P`, `I` and `D params` by increasing or decreasing the parameters by potential changes `delta_params` to reduce the sum of squared cross-track-error. If the error decreases either by increasing or decreasing the parameters then `delta_params` get scaled up. On the other hand if the error does not decrease either by increasing or decreasing the parameters by `delta_params` then the `delta_params` is scaled down. And this cycle is repeated till the desired low error rate is reached. The implementation of this algorihtm can be seen in `twiddle.cpp`. Finally, the best combination of the P, I and D parameters which worked for me are: `P = 0.220256`, `I = 0.000119051`, `D = 3.93977`.



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
