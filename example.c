#include <stdio.h>
#include <stdlib.h>
#include "LPH91572.h"

int main (void)
{
	LCD_init();

	unsigned short myScreenShot[23232];
	
	//Забивка скриншота 132x176
	
	FILE* f_scr = fopen("test2.bmp", "r");
	fseek(f_scr, 0x36, SEEK_SET); // skip bmp header
	fread(&myScreenShot, 1, 176 * 132 * 2/*16bit*/, f_scr);
	fclose(f_scr);
	
	for (int x = 0; x < 23232; x += 1) {
//		myScreenShot[x] = (myScreenShot[x] >> 8 | myScreenShot[x] << 8);
	}
	
	LCD_FillScreen(myScreenShot);
	
	return 0;
}

