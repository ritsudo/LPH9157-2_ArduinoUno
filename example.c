#include "LPH91572.h"

int main (void)
{

	LCD_init();
	
	for (;;)
	{
		LCD_FillScreen (0x0F);
		LCD_FillScreen (0xF0);
	}
	return 0;
}

