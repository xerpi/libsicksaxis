/* SickSaxis lib by xerpi */

#include <gccore.h>
#include <wiiuse/wpad.h>
#include <gcutil.h>
#include <ogcsys.h>
#include <ogc/ipc.h>
#include <ogc/usb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiiuse/wpad.h>
#include <sicksaxis.h>

#define cls() printf("\x1b[2;0H")

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;
u32 pressed, cnt = 0, dir = 1, i = 0;

void initStuff();
void print_ss_data(ss_instance_t ss);
void moveLeds();

//SickSaxis
	ss_instance_t ss;

int main(int argc, char **argv)
{
	IOS_ReloadIOS(58);
	usleep(500 * 1000);
	USB_Initialize();
	
	initStuff();

	printf("SickSaxis lib v1 made by xerpi\n\n");
	
	//SickSaxis
		ss_init();
		ss_open(&ss);
		ss_start_reading(&ss);
	
	while(1)
	{
		printf("\x1b[8;0H"); //Clear console
		WPAD_ScanPads();
		pressed = WPAD_ButtonsDown(0);
		
		//SickSaxis	
			if(ss.connected)
			{
				moveLeds();
			}
			else
			{
				cls();
				usleep(100 * 1000); //100mS
				if(ss_open(&ss)) ss_start_reading(&ss);
			}

		print_ss_data(ss);
		printf("\nLED: %i\n", i);

		
		if (pressed & WPAD_BUTTON_HOME) break;
		if(ss.gamepad.buttons.PS && ss.gamepad.buttons.start) break;
		VIDEO_Flush();
		VIDEO_WaitVSync();
	}
	ss_close(&ss);
	ss_end();
	USB_Deinitialize();
	exit(0);
	return 0;
}

void moveLeds()
{
	cnt++;
	if(cnt > 8)
	{
		if(dir == 1)
		{
			i++;
			cnt = 0;
			if(i > 4)
			{
				i = 4;
				dir = 0;
			}
			ss_set_led(&ss, i);
		}
		else
		{
			i--;
			cnt = 0;
			if(i < 1)
			{
				i = 1;
				dir = 1;
			}
			ss_set_led(&ss, i);				
		}
	}
}

void initStuff()
{
	VIDEO_Init();
	WPAD_Init();
	
	rmode = VIDEO_GetPreferredMode(NULL);
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();
	cls();
}

void print_ss_data(ss_instance_t ss)
{
    printf("\n\nPS: %i   START: %i   SELECT: %i   /\\: %i   []: %i   O: %i   X: %i   L3: %i   R3: %i\n", \
            ss.gamepad.buttons.PS, ss.gamepad.buttons.start, ss.gamepad.buttons.select, ss.gamepad.buttons.triangle, \
            ss.gamepad.buttons.square, ss.gamepad.buttons.circle, ss.gamepad.buttons.cross, ss.gamepad.buttons.L3, ss.gamepad.buttons.R3);

    printf("L1: %i   L2: %i   R1: %i   R2: %i   UP: %i   DOWN: %i   RIGHT: %i   LEFT: %i\n", \
            ss.gamepad.buttons.L1, ss.gamepad.buttons.L2, ss.gamepad.buttons.R1, ss.gamepad.buttons.R2, \
            ss.gamepad.buttons.up, ss.gamepad.buttons.down, ss.gamepad.buttons.right, ss.gamepad.buttons.left);

    printf("LX: %i   LY: %i   RX: %i   RY: %i\n", \
            ss.gamepad.leftAnalog.x, ss.gamepad.leftAnalog.y, ss.gamepad.rightAnalog.x, ss.gamepad.rightAnalog.y);

    printf("aX: %i   aY: %i   aZ: %i   Zgyro: %i\n", \
            ss.gamepad.motion.accX, ss.gamepad.motion.accY, ss.gamepad.motion.accZ, ss.gamepad.motion.Zgyro);

    printf("L1 press: %i   L2 press: %i   R1 press: %i   R2 press: %i\n", \
            ss.gamepad.shoulder_sens.L1, ss.gamepad.shoulder_sens.L2, ss.gamepad.shoulder_sens.R1, ss.gamepad.shoulder_sens.R2);

    printf("/\\ press: %i   [] press: %i   O press: %i   X press: %i\n",
            ss.gamepad.button_sens.triangle, ss.gamepad.button_sens.square, ss.gamepad.button_sens.circle, ss.gamepad.button_sens.cross);

    printf("UP: %i   DOWN: %i   RIGHT: %i   LEFT: %i\n", \
            ss.gamepad.dpad_sens.up, ss.gamepad.dpad_sens.down, ss.gamepad.dpad_sens.right, ss.gamepad.dpad_sens.left);
}
