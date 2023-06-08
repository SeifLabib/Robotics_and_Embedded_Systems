#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <camera/dcmi_camera.h>
#include <msgbus/messagebus.h>
#include <parameter/parameter.h>


//constants for the differents parts of the project
#define IMAGE_BUFFER_SIZE		640     //size of the buffer of the captured and saved image
#define WIDTH_SLOPE				5       //minimal width of the falling slope used to calculate the mean of the image
#define MIN_LINE_WIDTH			40      //minimal width of the line in pixels to be considered a black line
#define ROTATION_THRESHOLD		14
#define ROTATION_COEFF			2
#define GOAL_DISTANCE 			100     //[mm] => 10cm
#define ERROR_THRESHOLD			10	    //[mm] => 1cm
#define KP						23      //pour une distance en [mm], determined experimentally
#define KI 						1	    //must not be zero, pour une distance en [mm]
#define MAX_SUM_ERROR 			(MOTOR_SPEED_LIMIT/KI)
#define WALK_SPEED              900     //walking speed 900 steps/s => 11.7 cm/s
#define TURN_SPEED              600     //turning speed 600 steps/s => decided experimentally, not too slow, nor too fast
#define TURN_WAIT_TIME90        537     //time needed to turn 90 degrees to scan
#define WALK_WAIT_TIME          2137    //time needed to walk 2.137s to have the side of the square = 25cm
#define NB_QUADRANTS_CORNERS    5       //5 quadrants pour faire un tour de 360+90 degre
#define NB_QUADRANTS_SAFE       2       //2 quadrants pour faire un tour de 180 degre

/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

#ifdef __cplusplus
}
#endif

#endif
