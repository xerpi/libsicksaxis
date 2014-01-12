#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <sicksaxis.h>

static int run = 1;
static void *xfb = NULL;
static GXRModeObj *rmode = NULL;
void init_video();
void print_ss_data(const struct ss_device *ss);

int main(int argc, char **argv)
{
	IOS_ReloadIOS(58);
	usleep(100 * 1000);
	USB_Initialize();
	init_video();
	WPAD_Init();
	
	ss_init();
	struct ss_device dev, dev2;
	ss_initialize(&dev);

	while(run) {
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		
		
		if (pressed & WPAD_BUTTON_A) {if (ss_open(&dev)>0) ss_start_reading(&dev);}
		if (pressed & WPAD_BUTTON_PLUS) {if (ss_open(&dev2)>0) ss_start_reading(&dev2);}
		if (pressed & WPAD_BUTTON_1) ss_set_led(&dev, rand()%8);
		if (pressed & WPAD_BUTTON_2) ss_set_rumble(&dev, rand()%0xFF, rand()%0xFF, rand()%0xFF, rand()%0xFF);

		printf("\x1b[2;0H");
		if (dev.connected)
			print_ss_data(&dev);
			
		if (dev2.connected)
			print_ss_data(&dev2);
		
		if (pressed & WPAD_BUTTON_HOME) run = 0;
		VIDEO_WaitVSync();
	}
	ss_close(&dev);
	ss_close(&dev2);
	USB_Deinitialize();
	exit(0);
	return 0;
}

void init_video()
{
	// Initialise the video system
	VIDEO_Init();

	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	
	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);
	
	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);
	
	// Make the display visible
	VIDEO_SetBlack(FALSE);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
	
	printf("\x1b[2;0H");
}


void print_ss_data(const struct ss_device *ss)
{
    printf("\n\nPS: %i   START: %i   SELECT: %i   /\\: %i   []: %i   O: %i   X: %i   L3: %i   R3: %i\n", \
            ss->pad.buttons.PS, ss->pad.buttons.start, ss->pad.buttons.select, ss->pad.buttons.triangle, \
            ss->pad.buttons.square, ss->pad.buttons.circle, ss->pad.buttons.cross, ss->pad.buttons.L3, ss->pad.buttons.R3);

    printf("L1: %i   L2: %i   R1: %i   R2: %i   UP: %i   DOWN: %i   RIGHT: %i   LEFT: %i\n", \
            ss->pad.buttons.L1, ss->pad.buttons.L2, ss->pad.buttons.R1, ss->pad.buttons.R2, \
            ss->pad.buttons.up, ss->pad.buttons.down, ss->pad.buttons.right, ss->pad.buttons.left);

    printf("LX: %i   LY: %i   RX: %i   RY: %i\n", \
            ss->pad.left_analog.x, ss->pad.left_analog.y, ss->pad.right_analog.x, ss->pad.right_analog.y);

    printf("aX: %i   aY: %i   aZ: %i   Zgyro: %i\n", \
            ss->pad.motion.acc_x, ss->pad.motion.acc_y, ss->pad.motion.acc_z, ss->pad.motion.z_gyro);

    printf("L1 press: %i   L2 press: %i   R1 press: %i   R2 press: %i\n", \
            ss->pad.shoulder_sens.L1, ss->pad.shoulder_sens.L2, ss->pad.shoulder_sens.R1, ss->pad.shoulder_sens.R2);

    printf("/\\ press: %i   [] press: %i   O press: %i   X press: %i\n",
            ss->pad.button_sens.triangle, ss->pad.button_sens.square, ss->pad.button_sens.circle, ss->pad.button_sens.cross);

    printf("UP: %i   DOWN: %i   RIGHT: %i   LEFT: %i\n", \
            ss->pad.dpad_sens.up, ss->pad.dpad_sens.down, ss->pad.dpad_sens.right, ss->pad.dpad_sens.left);
}
