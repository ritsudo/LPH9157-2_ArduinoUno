#include "LPH91572.h"

int main (void)
{
	LCD_init();

	unsigned short myScreenShot[23232];
	
	//Забивка скриншота 132x176
	
	for (int x = 0; x < 23232; x++) 
	{   		
		if (x % 2 == 0) {
		myScreenShot[x] = 0x0F;
		}
		else
		{
			myScreenShot[x] = 0xF0;
		}
	}  
	
	
	LCD_FillScreen(myScreenShot);
	
	return 0;
}

