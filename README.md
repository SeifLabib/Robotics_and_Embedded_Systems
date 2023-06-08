# MiniProjet : Surveillance Robot
Welcome to the branch containing the source code for our MiniProjet, for the course of Embedded Systems and Robotics MICRO-315 for the spring of 2023.

The main theme is around a military surveillance robot, that performs predefined patterns (in our case a square), to scan a protected zone. The robot detects objects using the ToF sensor, then proceeds to follow the object to get closer and detect its color using the camera. Once its color detected, the robot decides whether to keep following it (eventually destroying it) or leaving it alone because it judged the object safe. The robot also plays an alarm sound when it doees detect an object, as well as it lights its RGB LEDs, meaning it got out of its "camouflage" and is following the objbect to destroy it.

The code made for an [EPuck2](https://www.gctronic.com/doc/index.php/e-puck2) based on its [library](https://www.gctronic.com/doc/index.php?title=e-puck2_robot_side_development), using the RTOS [ChibiOS](https://www.chibios.org/dokuwiki/doku.php). 

## User Manual
Just reset the robot, and it will initialize all the ressources and the peripherals it needs, then proceed to complete the square pattern predefined, scan, detect, follow and destroy.

## Constants to modify
In our code, we have some constants that are modifiable, both are in the module main.h: 
- `GOAL_DISTANCE` that indicates the distance that the EPuck2 keeps with it target object, depends on the "arm" used, but it is set to 10cm by default (100mm).
- `WALK_WAIT_TIME` that indicates the time that the EPuck2 keeps its motors set to the walking speed, changing the length of the sides of the square pattern, depends on the wanted demonstration and the conditions.

---------------------------------------------------------------------------------------------------------------------------------------------------------------------

## Update for the demo
- For the demo we have in mind, we found it necessary to modify a few variables:
- `KP` which adjusts the 'following' speed, modified to be enough for the robot to 'explode' (destroy) the target.
- `GOAL_DISTANCE` was changed to 0 to have the robot making physical contact with the body if not safe.
- The sleep time of the Thread `Leave_safe_object` to get called more frequently so it does not destroy the safe body (aka. green balloon)
