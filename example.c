#include "LPH91572.h"

int main (void)
{

	LCD_init();
	
	for (;;)
	{
		LCD_FillScreen (0x000F);
		LCD_FillScreen (0xF000);
	}
	return 0;
}

