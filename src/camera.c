#include <ch.h>
#include <camera/po8030.h>
#include "camera.h"
#include "main.h"

static uint16_t line_position = 0; //IMAGE_BUFFER_SIZE/2;		//indicates the position of the black line detected, initialized to 0
static bool green_detected = FALSE;							//indicates whether the robot is detecting a green object or not
static uint16_t begin = 0;									//indicates the position of the beginning of the black line detected
															//used to calculate its width	
static uint16_t end = 0;									//indicates the position of the end of the black line detected
															//used to calculate its width

//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);

/*
 *  Returns the line's width extracted from the image buffer given
 *  Returns 0 if line not found
 */
void update_line_position(uint8_t *buffer){

	uint16_t i = 0;
	uint8_t stop = 0, wrong_line = 0, line_not_found = 0;
	uint32_t mean = 0;

	//performs an average
	mean = calculate_mean(buffer);
	do{
		wrong_line = 0;
		//search for a begin
		while(stop == 0 && i < (IMAGE_BUFFER_SIZE - WIDTH_SLOPE))
		{ 
			//the slope must at least be WIDTH_SLOPE wide and is compared
		    //to the mean of the image
		    if(buffer[i] > mean && buffer[i+WIDTH_SLOPE] < mean)
		    {
		        begin = i;
		        stop = 1;
		    }
		    i++;
		}
		//if a begin was found, search for an end
		if (i < (IMAGE_BUFFER_SIZE - WIDTH_SLOPE) && begin)
		{
		    stop = 0;
		    
		    while(stop == 0 && i < IMAGE_BUFFER_SIZE)
		    {
		        if(buffer[i] > mean && buffer[i-WIDTH_SLOPE] < mean)
		        {
		            end = i;
		            stop = 1;
		        }
		        i++;
		    }
		    //if an end was not found
		    if (i > IMAGE_BUFFER_SIZE || !end)
		    {
		        line_not_found = 1;
		    }
		}
		else//if no begin was found
		{
		    line_not_found = 1;
		}

		//if a line too small has been detected, continues the search
		if(!line_not_found && (end-begin) < MIN_LINE_WIDTH){
			i = end;
			begin = 0;
			end = 0;
			stop = 0;
			wrong_line = 1;
		}
	}while(wrong_line);

	if(line_not_found){
		begin = 0;
		end = 0;
	}else{
		line_position = (begin + end)/2; //gives the line position.
	}

}

static THD_WORKING_AREA(waCaptureImage, 256);
static THD_FUNCTION(CaptureImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	//Takes pixels 0 to IMAGE_BUFFER_SIZE of the lines USED_LINE and USED_LINE + 1 (minimum 2 lines because reasons)
	po8030_advanced_config(FORMAT_RGB565, 0, USED_LINE, IMAGE_BUFFER_SIZE, 2, SUBSAMPLING_X1, SUBSAMPLING_X1);
	dcmi_enable_double_buffering();
	dcmi_set_capture_mode(CAPTURE_ONE_SHOT);
	dcmi_prepare();

    while(1){
        //starts a capture
		dcmi_capture_start();
		//waits for the capture to be done
		wait_image_ready();
		//signals an image has been captured
		chBSemSignal(&image_ready_sem);
    }
}

static THD_WORKING_AREA(waProcessImage, 2048);
static THD_FUNCTION(ProcessImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	uint8_t *img_buff_ptr;
	uint8_t one_color_image[IMAGE_BUFFER_SIZE] = {0};

    while(1){
    	//waits until an image has been captured
        chBSemWait(&image_ready_sem);
		//gets the pointer to the array filled with the last image in RGB565    
		img_buff_ptr = dcmi_get_last_image_ptr();

		//Extracts only the red pixels
		for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2){
			//extracts 5 MSbits of the MSbyte (First byte in big-endian format)
			//takes nothing from the second byte
			one_color_image[i/2] = (uint8_t)img_buff_ptr[i] & 0xF8;
		}

		//search for a line in the image and gets its width in pixels
		update_line_position(one_color_image);
		//updates the bool green_detected depending on whether the robot is detecting a green object or not
		update_green_detected(img_buff_ptr, one_color_image);
    }
}


uint16_t get_line_position(void){
	return line_position;
}

void process_image_start(void){
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO, ProcessImage, NULL);
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO, CaptureImage, NULL);
}


bool green_detected_getter(void){
    return green_detected;
}

uint32_t calculate_mean(uint8_t *buffer){
	uint32_t mean = 0;
	for(uint16_t i = 0 ; i < IMAGE_BUFFER_SIZE ; i++){
		mean += buffer[i];
	}
	mean /= IMAGE_BUFFER_SIZE;
	return mean;
}

uint32_t calculate_mean_no_black(uint8_t *buffer){
	uint32_t mean = 0;
	for(uint16_t i = 0 ; i < IMAGE_BUFFER_SIZE ; i++){
		if(i<begin || i>end){
			mean += buffer[i];
		}
	}
	mean /= (IMAGE_BUFFER_SIZE-(end-begin));
	return mean;
}



uint32_t get_green_mean(uint8_t *img_buff_ptr){
	uint8_t image[IMAGE_BUFFER_SIZE] = {0};
	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2){
		//extracts 3 LSbits of the first byte and the 3 MSbits of second byte
		image[i/2] = (((uint8_t)img_buff_ptr[i] & 0x07) << 5 )
				   + (((uint8_t)img_buff_ptr[i+1] & 0xE0) >> 3);
	}
	return calculate_mean_no_black(image);
}

uint32_t get_blue_mean(uint8_t *img_buff_ptr){
	uint8_t image[IMAGE_BUFFER_SIZE] = {0};
	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2){
		//extracts 5 LSbits of the LSByte (Second byte in big-endian format)
		//and rescale to 8 bits
		//takes nothing from the first byte
		image[i/2] = ((uint8_t)img_buff_ptr[i+1] & 0x1F) << 3;
	}
	return calculate_mean_no_black(image);
}

void update_green_detected(uint8_t *img_buff_ptr, uint8_t *image){
	uint32_t green_mean=0;
	uint32_t blue_mean=0;
	uint32_t red_mean=0;

	red_mean = calculate_mean_no_black(image);
	green_mean = get_green_mean(img_buff_ptr); 
	blue_mean  = get_blue_mean(img_buff_ptr);

	if((green_mean > blue_mean) && (green_mean > red_mean)){
		green_detected = TRUE;
	}
	else{
		green_detected = FALSE;
	}
}
