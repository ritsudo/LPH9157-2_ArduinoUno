//--------Библиотека дисплея Siemens C75, МЕ75---------
//           Зеленый текстолит LPH9157-2
//              132х176 пикселей
//          ==== Кизим Игорь ====
//              Port to Arduino UNO by ritsudo
//              Using hardware SPI ang GPIO (wiringPi)
//-----------------------------------------------------

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <string.h>

#define _GEOMETRICAL // Использование функций вывода геометрических фигур

//===============================================================
//		            Назначение выводов порта
//===============================================================

#define LCD_RS 21	 // CD - тип передаваемых данных - СЕРЫЙ
#define LCD_RESET 22 // Сброс - КОРИЧНЕВЫЙ
#define LCD_CS 23	 // Выбор чипа - ЗЕЛЁНЫЙ - ИСПОЛЬЗУЕМ SPIDEV0.0 PIN 24
#define LCD_CLK 24	 // Синхронизация - СИНИЙ - ИСПОЛЬЗУЕМ SPIDEV0.0 PIN 23
#define LCD_DATA 25	 // Данные - БЕЛЫЙ  - ИСПОЛЬЗУЕМ SPIDEV0.0 PIN 19

//*************************************************************
// Команда/Данные
#define CMD 0
#define DAT 1
char RS_old;

//*************************************************************
// ПРОТОТИПЫ ФУНКЦИЙ
void LCD_init(void);
void Send_to_lcd(unsigned char RS, unsigned char data);
void paint();
void SetArea(char x1, char x2, char y1, char y2);
void LCD_FillScreen(unsigned short *inputImg);

//*************************************************************
// ЭКРАННЫЙ БУФЕР ДЛЯ ОТПРАВКИ ПО SPI
unsigned short myScreenBuffer[23232];

//===============================================================
//                        ИНИЦИАЛИЗАЦИЯ
//===============================================================
void LCD_init(void)
{
	wiringPiSetup();
	// SPI BEGIN SPI.beginTransaction(SPISettings(30000000L, MSBFIRST, SPI_MODE0));

	int fd = wiringPiSPISetup(0, 32000000);
	if (fd < 0)
	{
		printf("Open the SPI device failed!\n");
		;
	}

	pinMode(LCD_CS, OUTPUT);
	pinMode(LCD_RESET, OUTPUT);
	pinMode(LCD_RS, OUTPUT);
	pinMode(LCD_CLK, OUTPUT);
	pinMode(LCD_DATA, OUTPUT);

	digitalWrite(LCD_RESET, 0);
	delay(500);
	digitalWrite(LCD_RESET, 1);
	delay(500);
	Send_to_lcd(CMD, 0x01); // Программный сброс
	Send_to_lcd(CMD, 0x36); // Memory Access Control (Направление заполнения области дисплея (памяти): 0bVHRXXXXX, V - заполнение по вертикали (0 - сверху-вниз, 1 - снизу-вверх),
	Send_to_lcd(DAT, 0x00);
	Send_to_lcd(CMD, 0x11); // Выход из спящего режима
	delay(20);
	Send_to_lcd(CMD, 0x3a); // Установка цветовой палитры
	Send_to_lcd(DAT, 0x05); // Два байта на пиксель 65536 цветов
	delay(20);
	Send_to_lcd(CMD, 0x29); // Включение дисплея
}

//===============================================================
// Функция записи команды/данных в LCD (RS==0 - команда, RS==1 - данные)
//===============================================================
void Send_to_lcd(unsigned char RS, unsigned char data)
{
	static unsigned char old_RS = 0;
	if ((old_RS != RS) || (!RS && !old_RS))
	{
		digitalWrite(LCD_RS, RS);
	}

	wiringPiSPIDataRW(0, &data, sizeof(data));
}

//===============================================================
// Функция записи экранного буфера в LCD
//===============================================================

void paint()
{
	unsigned char RS = 1;

	static unsigned char old_RS = 0;
	if ((old_RS != RS) || (!RS && !old_RS))
	{
		digitalWrite(LCD_RS, RS);
	}

	unsigned short mySendBuffer[2048];

	unsigned char *dataPointer = (char *)mySendBuffer;

	for (unsigned char i = 0; i < 11; i += 1)
	{
		unsigned short *myPointer = &myScreenBuffer[(i * 2048)];
		memcpy(mySendBuffer, myPointer, 4096);
		wiringPiSPIDataRW(0, dataPointer, 4096);
	}

	unsigned short *myPointer = &myScreenBuffer[(11 * 2048)];
	memcpy(mySendBuffer, myPointer, 1408);
	wiringPiSPIDataRW(0, dataPointer, 1408);
}

//===============================================================
//              Задание прямоугольной области экрана
//===============================================================
void SetArea(char x1, char x2, char y1, char y2)
{
	Send_to_lcd(CMD, 0x2A); // задаем область по X
	Send_to_lcd(DAT, x1);	// начальная
	Send_to_lcd(DAT, x2);	// конечная

	Send_to_lcd(CMD, 0x2B); // задаем область по Y
	Send_to_lcd(DAT, y1);	// начальная
	Send_to_lcd(DAT, y2);	// конечная

	Send_to_lcd(CMD, 0x2C); // отправляем команду на начало записи в память и начинаем посылать данные
}

//===============================================================
//                  ЗАЛИВКА ЭКРАНА ЦВЕТОМ
//===============================================================
void LCD_FillScreen(unsigned short *inputImg)
{
	SetArea(0, 131, 0, 175); // Область всего экрана
	digitalWrite(LCD_RS, 1);

	memcpy(myScreenBuffer, inputImg, 46464);

	paint();

	// delay(16);
}
