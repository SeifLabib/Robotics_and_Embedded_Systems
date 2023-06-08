#ifndef MOTEURS
#define MOTEURS


/**
*   @brief initializes the PI regulator and creates its Thread
*/
void pi_regulator_start(void);

/**
*   @brief initializes and creates the Thread of Walk_pattern, to start scanning for an object
*/
void walk_pattern_start(void);

/**
*   @brief initializes and creates the Thread of Leave_Safe_Object, to temporarily "deactivate" the ToF and turn and continue searching 
*/
void leave_safe_object_start(void);


/**
*   @brief sets the motors to the walking speed (searching mode)
*/
void walk(void);


/**
*   @brief sets the motors to the turning speed (searching mode)
*/
void turn(void);


/**
*   @brief stops the motors when the EPuck2 detects a green object or when it stops detecting an object (object disappears)
*/
void stop(void);


/**
*   @return returns the value of the bool walk_tof to stop the ToF while turning away from a green object (to not stay in an infinite loop)
*/
bool walk_tof_on_getter(void);

/**
 * @return returns the value of the speed determined by the PI regulator, depending on the detected distance and the predefined
 * @param distance the distance detected in [mm], modified by the tof_detection module 
*/
int16_t pi_regulator(int16_t distance);

#endif /* MOTEURS */