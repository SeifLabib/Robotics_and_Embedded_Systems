#include <ch.h>
#include <audio/play_melody.h>
#include <sensors/VL53L0X/VL53L0X.h>

#include "tof_detection.h"
#include "moteurs.h"
#include "leds.h"

static uint16_t dist_mm = 0;                    //16 bits car c'est possible qu'elle soit >255 mm (25 cm)
static bool object_detected = FALSE;            //indicates whether there is an object detected or not
static bool previous_object_detected = FALSE;   //indicates the last value of object_detected
static bool stop_pi_motors = FALSE;             //indicates whether to stop the pi_motors or not
                                                //to reset the speed of the motors to walking speed

void object_detected_setter(bool value){
    object_detected = value;
}

bool object_detected_getter(void){
    return object_detected;
}

bool stop_pi_motors_getter(void){
    return stop_pi_motors;
}

uint16_t get_distance(void){
    return dist_mm;
}

static THD_WORKING_AREA(waDetection, 258);
static THD_FUNCTION(Detection, arg) {

	chRegSetThreadName(__FUNCTION__);
    (void)arg;

    while(1){
        previous_object_detected = object_detected;
        dist_mm = VL53L0X_get_dist_mm();

        if((dist_mm <= DETECTION_RANGE) && (dist_mm > NOTHING_DETECTED) && (walk_tof_on_getter()) ){      
            alert_vis_aud();
            object_detected = TRUE;       
            }
        else{
            clear_leds();
            object_detected = FALSE;         
        }   
        if (previous_object_detected && !object_detected){
            stop_pi_motors=TRUE;
        }
        else{
            stop_pi_motors=FALSE;
        }
        //10Hz
        chThdSleepMilliseconds(100);
    }
}

void detection_start(void){
    VL53L0X_start();
    chThdCreateStatic(waDetection, sizeof(waDetection), NORMALPRIO, Detection, NULL);
}

void alert_vis_aud(void){
    set_rgb_led(LED2, RGB_LED_MILITARY);
    set_rgb_led(LED4, RGB_LED_MILITARY);
    set_rgb_led(LED6, RGB_LED_MILITARY);
    set_rgb_led(LED8, RGB_LED_MILITARY);        
    playNote(600, 40);
}
