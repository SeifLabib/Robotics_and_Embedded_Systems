#ifndef TOF_DETECTION_H
#define TOF_DETECTION_H

#define RGB_LED_MILITARY    69, 75, 0
#define DETECTION_RANGE     250
#define NOTHING_DETECTED    0

/**
*   @brief initializes the Tof and creates its Thread, present in VL53L0X.c, as well as the Thread Detection
*/
void detection_start(void);


/**
*   @brief sets the variable object_detected with a value to chose a mode in moteurs.c whether following or searching
    @param value the value to set object_detected
*/
void object_detected_setter(bool value);


/**
*   @return returns the value of object_detected to chose a mode whether following or searching
*/
bool object_detected_getter(void);


/**
*   @return returns the distance detected by the ToF in mm
*/
uint16_t get_distance(void);

/**
*   @brief activates the visual + auditory alert (called when object detected)
*/
void alert_vis_aud(void);


/**
*   @return returns the value of the bool stop_pi_motors to reset the speed of the motors to the searching speed
*/
bool stop_pi_motors_getter(void);


#endif /*TOF_DETECTION_H*/