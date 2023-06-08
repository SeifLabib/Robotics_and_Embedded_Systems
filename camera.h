#ifndef CAMERA_H
#define CAMERA_H

// Specify the 2 consecutive lines used for tracking the black line
// The line number starts from 0 and ending to PO8030_MAX_HEIGHT - 1. Consult camera/po8030.h
// But as 2 lines will be used, the value of the first line can be higher than PO8030_MAX_HEIGHT - 2
#define USED_LINE 200   // Must be inside [0..478], according to the above explanations

/**
 * @return returns the position of the black line detected, to help with rotation 
*/
uint16_t get_line_position(void);

/**
 * @brief initializes and creates the Threads capture_image and process_image
*/
void process_image_start(void);

/**
 * @brief updates the line position of the black line detected, to help with rotation
 * @param buffer the pointer that points on where the version to be manipulated of line of image is stored
*/
void update_line_position(uint8_t *buffer);

/**
 * @return returns the value of the bool green_detected that indicates whether the robot is detecting a green object or not
*/
bool green_detected_getter(void);


/**
 * @return returns the value of the mean of the red color (by default) in the line of the image captured
 * @param buffer the pointer that points on where the version to be manipulated of line of image is stored
*/
uint32_t calculate_mean(uint8_t *buffer);

/**
 * @return returns the value of the mean of the blue color in the line of the image captured
 * @param img_buff_ptr the pointer that points on where the line of image is stored
*/
uint32_t get_blue_mean(uint8_t *img_buff_ptr);

/**
 * @return returns the value of the mean of the green color in the line of the image captured
 * @param img_buff_ptr the pointer that points on where the line of image is stored
*/
uint32_t get_green_mean(uint8_t *img_buff_ptr);

/**
 * @param one_color_image the pointer that points on the array of the line of image after being manipulated to only contain red, green or blue
 * @param img_buff_ptr the pointer that points on where the line of image is stored
*/
void update_green_detected(uint8_t *img_buff_ptr, uint8_t *one_color_image);


/**
 * @return returns the value of the mean of the line of image captured and manipulated without taking into consideration the black line, to detect its color
 * @param buffer the pointer that points on where the version to be manipulated of line of image is stored
*/
uint32_t calculate_mean_no_black(uint8_t *buffer);

#endif /* CAMERA_H */