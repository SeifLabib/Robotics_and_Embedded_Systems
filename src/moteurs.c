#include <ch.h>
#include <math.h>
#include <motors.h>

#include "moteurs.h"
#include "main.h"
#include "camera.h"
#include "tof_detection.h"

static bool walk_tof_on = TRUE;				//indicates whether to keep using the ToF to
										   //momentarily "deactivate it" if detecting a green object

//simple PI regulator implementation
int16_t pi_regulator(int16_t distance){

	int16_t error = 0;
	int16_t speed = 0;

	static int16_t sum_error = 0;

	error = distance - GOAL_DISTANCE;

	//disables the PI regulator if the error is to small
	//this avoids to always move as we cannot exactly be where we want and 
	//the ToF sensor is a bit noisy
	if(fabs(error) < ERROR_THRESHOLD){
		return 0;
	}

	sum_error += error;

	//we set a maximum and a minimum for the sum to avoid an uncontrolled growth
	if(sum_error > MAX_SUM_ERROR){
		sum_error = MAX_SUM_ERROR;
	}else if(sum_error < -MAX_SUM_ERROR){
		sum_error = -MAX_SUM_ERROR;
	}

	speed = KP * error;// + KI * sum_error;

    return speed;
}

static THD_WORKING_AREA(waPiRegulator, 128);
static THD_FUNCTION(PiRegulator, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    systime_t time;

    int16_t speed = 0;
    int16_t speed_correction = 0;

    while(1){
		if (object_detected_getter()){
			//computes the speed to give to the motors
			//get_distance is modified by the tof_detection thread
			speed = pi_regulator(get_distance());
			//computes a correction factor to let the robot rotate to be in front of the line
			speed_correction = (get_line_position() - (IMAGE_BUFFER_SIZE/2));

			//if the line is nearly in front of the camera, don't rotate
			//if line not detected don't rotate
			if((abs(speed_correction) < ROTATION_THRESHOLD) || !get_line_position() ){
				speed_correction = 0;
			}

			//if line not detected don't rotate
			if (!get_line_position()){
				speed_correction = 0;
			}

			right_motor_set_speed(speed - ROTATION_COEFF * speed_correction);
			left_motor_set_speed(speed + ROTATION_COEFF * speed_correction);

			time = chVTGetSystemTime();

			//100Hz
			chThdSleepUntilWindowed(time, time + MS2ST(10)); 
		}
		//if the bool stop_pi_motors is FALSE then stop the motors to reset their speeds to the pattern speed (walk + turn)
		if (stop_pi_motors_getter()){
			stop();
		}
	}
}


static THD_WORKING_AREA(waWalkPattern, 64);
static THD_FUNCTION(WalkPattern, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    while(1){
		//if the robot isn't detecting any object and walk_tof_on is TRUE then complete the designed pattern (walk + turn)
		if (!object_detected_getter() && walk_tof_on){
			walk();
		}
		chThdSleepMilliseconds(WALK_WAIT_TIME); 
		if (!object_detected_getter() && walk_tof_on){
			turn();
		} 
		chThdSleepMilliseconds(TURN_WAIT_TIME90*NB_QUADRANTS_CORNERS);  
	}
}

static THD_WORKING_AREA(waleavesafeobject, 128);
static THD_FUNCTION(LeaveSafeObject, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    while(1){
		//if there is an object detected AND the object is green (aka. safe) then leave the object (turn and momentarily "deactivate" the ToF sensor)
		if (green_detected_getter() && object_detected_getter()){ 
			turn();
			object_detected_setter(FALSE);
			walk_tof_on = FALSE; 
			chThdSleepMilliseconds(TURN_WAIT_TIME90*NB_QUADRANTS_SAFE);
			stop();
			walk_tof_on = TRUE; 
		}
		chThdSleepSeconds(1); 
	}
}


void pi_regulator_start(void) {
	chThdCreateStatic(waPiRegulator, sizeof(waPiRegulator),NORMALPRIO, PiRegulator, NULL);
}


void walk_pattern_start(void) {
	chThdCreateStatic(waWalkPattern,sizeof(waWalkPattern),NORMALPRIO, WalkPattern, NULL);
}


void leave_safe_object_start(void) {
	chThdCreateStatic(waleavesafeobject,sizeof(waleavesafeobject),NORMALPRIO, LeaveSafeObject, NULL);
}


void walk(void){
	right_motor_set_speed(WALK_SPEED);
	left_motor_set_speed(WALK_SPEED);
}


void turn(void){
	right_motor_set_speed(TURN_SPEED);
	left_motor_set_speed(-TURN_SPEED);
}


void stop(void){    
	right_motor_set_speed(0);
	left_motor_set_speed(0);
}


bool walk_tof_on_getter(void){
    return walk_tof_on;
}