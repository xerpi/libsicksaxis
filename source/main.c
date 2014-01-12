#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include "sicksaxis.h"


#define HEAP_SIZE 4096

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;
void init_video();

int main(int argc, char **argv)
{
	IOS_ReloadIOS(58);
	usleep(100 * 1000);
	USB_Initialize();
	init_video();
	WPAD_Init();
	printf("sicksaxis v2\n");
	
	ss_init();
	struct ss_device *dev, *dev2;

	while(1) {
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		
		if (pressed & WPAD_BUTTON_A) dev = ss_open();
		if (pressed & WPAD_BUTTON_PLUS) dev2 = ss_open();
		if (pressed & WPAD_BUTTON_B) printf("\x1b[2;0H");


		if (pressed & WPAD_BUTTON_HOME) exit(0);
		VIDEO_WaitVSync();
	}

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
