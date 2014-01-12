#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include "sicksaxis.h"


#define HEAP_SIZE 4096

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;
static int heap_id = -1;
void init_video();
void usb_list();

int main(int argc, char **argv)
{
	IOS_ReloadIOS(58);
	usleep(50 * 1000);
	USB_Initialize();
	init_video();
	WPAD_Init();
	printf("sicksaxis v2\n");
	
	heap_id = iosCreateHeap(heap_id);
	
	usb_list();

	while(1) {
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		
		if (pressed & WPAD_BUTTON_A) usb_list();
		if (pressed & WPAD_BUTTON_B) printf("\x1b[2;0H");


		if (pressed & WPAD_BUTTON_HOME) exit(0);
		VIDEO_WaitVSync();
	}

	return 0;
}


void usb_list()
{
	usb_device_entry dev_entry[8];
	u8 dev_count;
	if (USB_GetDeviceList(dev_entry, 8, USB_CLASS_HID, &dev_count) < 0) {
		printf("Could not get USB device list.\n");
		return;
	}
	printf("Found %d USB devices.\n\n", dev_count);
	if (dev_count == 0) return;
	
	/*
	Device 0:
		Device ID: 1966113
		Token: 2228225
		
	Device 1:
		Device ID: 2031650
		Token: 2162689		
	
	*/
	
	int i;
	for (i = 0; i < dev_count; ++i) {
		printf("Device %d:\n", i);
		printf("\tDevice ID: %d\n", dev_entry[i].device_id);
		//printf("\tVID: 0x%X\n", dev_entry[i].vid);
		//printf("\tPID: 0x%X\n", dev_entry[i].pid);
		printf("\tToken: %d\n\n", dev_entry[i].token);
	}
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
