#include "LPH91572.h"

int main (void)
{

	LCD_init();
	
	for (;;)
	{
		LCD_FillScreen (0xF000);
		LCD_FillScreen (0x0F00);
	}
	return 0;
}

