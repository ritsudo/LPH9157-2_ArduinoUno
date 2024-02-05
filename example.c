#include "LPH91572.h"

int main (void)
{

	LCD_init();
	
	for (;;)
	{
		LCD_FillScreen (0x000F);
		LCD_FillScreen (0x00F0);
	}
	return 0;
}

