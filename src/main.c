#include <ch.h>
#include <hal.h>
#include <memory_protection.h>
#include <usbcfg.h>
#include <motors.h>
#include <camera/po8030.h>
#include <audio/audio_thread.h>

// In order to be able to use the RGB LEDs
#include <spi_comm.h>

#include "main.h"
#include "moteurs.h"
#include "camera.h"
#include "tof_detection.h"

static void serial_start(void)
{
	static SerialConfig ser_cfg = {
	    115200,
	    0,
	    0,
	    0,
	};

	sdStart(&SD3, &ser_cfg); // UART3.
}

 int main(void){
    halInit();
    chSysInit();
    mpu_init();

    //starts the serial communication
    serial_start();

    //start the USB communication
    usb_start();

    //starts the camera
    dcmi_start();
	po8030_start();

	//inits the motors
	motors_init();

	//starts RGB LEDs
	spi_comm_start();

	//stars the threads for the pi regulator
	pi_regulator_start();

	//starts the threads for the capture and the processing of the camera
	process_image_start();

	//starts the walking pattern thread to start completing the sqaure pattern
	walk_pattern_start();

	//starts the detecting thread, as well as the ToF, to start scanning searching for an object
	detection_start();

	//starts the DAC to use the speaker
	dac_start();

	//starts the leave_safe_object thread, to configure the motors to leave
	//the safe object (aka. green) when detected, while momentarily "deactivating" the ToF using a boolean called walk_tof_on
	leave_safe_object_start();
	
    while (1) {
		chThdSleepSeconds(100);
    }
	
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
