#include "LPH91572.h"

int main (void)
{

	LCD_init();
	
	for (;;)
	{
		LCD_FillScreen (0xFFFF);
		LCD_Puts_Shadow("Hello", 6, 20, 0x0000,  4, 4, 90);
		LCD_Puts_Shadow("Hello", 4, 18, 0xFFFF,  4, 4, 90);
		LCD_Puts_Shadow("Hello", 4, 60, 0xFFFF,  2, 2, 90);
		LCD_Puts_Shadow("Hello", 4, 90, 0x0000,  1, 1, 90);
		delay(2000);
	}
	return 0;
}

