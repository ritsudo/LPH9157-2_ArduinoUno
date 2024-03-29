//--------Библиотека дисплея Siemens C75, МЕ75---------
//           Зеленый текстолит LPH9157-2
//              132х176 пикселей
//          ==== Кизим Игорь ====
//              Port to Arduino UNO by ritsudo
//              Using program SPI emulator
//-----------------------------------------------------

#include "Symbols.h"

//#define _8_BIT_COLOR  //Если закомментировано - 16-ти битный цвет
#define _GEOMETRICAL  //Использование функций вывода геометрических фигур

//===============================================================
//		            Назначение выводов порта 
//===============================================================
#define LCD_CS 		4 //Выбор чипа
#define LCD_RESET 	3 //Сброс
#define LCD_RS 		2	//CD - тип передаваемых данных
#define LCD_CLK 	5 //Синхронизация
#define LCD_DATA 	6 //Данные

//*************************************************************
//Команда/Данные
#define CMD 0
#define DAT 1
char RS_old;


//*************************************************************
//ПРОТОТИПЫ ФУНКЦИЙ
void LCD_init (void);
void Send_to_lcd (unsigned char RS, unsigned char data);
void SetArea (char x1, char x2, char y1, char y2);
void Put_Pixel (char x, char y, unsigned int color);
void Send_Symbol (unsigned char symbol, char x, char y, int t_color, int b_color, char zoom_width, char zoom_height, int rot);  
void LCD_Putchar (char symbol, char x, char y, int t_color, int b_color, char zoom_width, char zoom_height, int rot);    
void LCD_Puts(char *str, int x, int y,  int t_color, int b_color, char zoom_width, char zoom_height, int rot);
void LCD_Putsf(char *str, int x, int y,  int t_color, int b_color, char zoom_width, char zoom_height, int rot);
void Send_Symbol_Shadow (unsigned char symbol, char x, char y, int t_color, char zoom_width, char zoom_height, int rot);
void LCD_Putchar_Shadow (char symbol, char x, char y, int t_color, char zoom_width, char zoom_height, int rot);
void LCD_Puts_Shadow (char *str, int x, int y,  int t_color, char zoom_width, char zoom_height, int rot);
void LCD_Putsf_Shadow (char *str, int x, int y,  int t_color, char zoom_width, char zoom_height, int rot);
void LCD_FillScreen (unsigned int color);

//===============================================================
//                        ИНИЦИАЛИЗАЦИЯ
//===============================================================
void LCD_init(void)
{
pinMode(LCD_CS, OUTPUT);
pinMode(LCD_RESET, OUTPUT);
pinMode(LCD_RS, OUTPUT);
pinMode(LCD_CLK, OUTPUT);
pinMode(LCD_DATA, OUTPUT);

 digitalWrite(LCD_RESET, LOW);
 delay(500);
 digitalWrite(LCD_RESET, HIGH);
 delay(500);
 Send_to_lcd(CMD, 0x01); //Программный сброс
 Send_to_lcd(CMD, 0x36); //Memory Access Control (Направление заполнения области дисплея (памяти): 0bVHRXXXXX, V - заполнение по вертикали (0 - сверху-вниз, 1 - снизу-вверх), 
 Send_to_lcd(DAT, 0x00);
 Send_to_lcd(CMD, 0x11); //Выход из спящего режима
 delay(20);
 Send_to_lcd(CMD, 0x3a); //Установка цветовой палитры
 #ifdef _8_BIT_COLOR
 Send_to_lcd(DAT, 0x02); //Байт на пиксель 256 цветов
 #else
 Send_to_lcd(DAT, 0x05); //Два байта на пиксель 65536 цветов
 #endif
 delay(20);
 Send_to_lcd(CMD, 0x29); //Включение дисплея
}

//===============================================================
//Функция записи команды/данных в LCD (RS==0 - команда, RS==1 - данные)
//===============================================================
void Send_to_lcd (unsigned char RS, unsigned char data)
{
 //unsigned char count;  
 digitalWrite(LCD_CLK, LOW);
 digitalWrite(LCD_DATA, LOW);
 if ((RS_old != RS) || (!RS_old && !RS)) //проверяем старое значение RS (если поступают одни команды то дергаем CS)
 { 
  digitalWrite(LCD_CS, HIGH);	// Установка CS 
  digitalWrite(LCD_RS, RS);	
  digitalWrite(LCD_CS, LOW);	// Сброс CS 
 }
 //******************************************************************************
 //Такой цикл обеспечивает более компактный код при записи байта в дисплей 
 //******************************************************************************
 /*
 for (count = 0; count < 8; count++) //Цикл передачи данных
 { 
  if(data&0x80)		LCD_DATA=1;
  else			    LCD_DATA=0;
  digitalWrite(LCD_CLK, HIGH);  
  data <<= 1;
  digitalWrite(LCD_CLK, LOW);    
 } 
 */
 //******************************************************************************
 //Такой прямой код (без цикла) обеспечивает более быструю запись байта в дисплей 
 //******************************************************************************
 digitalWrite(LCD_DATA, LOW);
 if ((data & 128) == 128)  digitalWrite(LCD_DATA, HIGH); 
 digitalWrite(LCD_CLK, HIGH);
 digitalWrite(LCD_CLK, LOW);
 digitalWrite(LCD_DATA, LOW);
 if ((data & 64) == 64)  digitalWrite(LCD_DATA, HIGH); 
 digitalWrite(LCD_CLK, HIGH);
 digitalWrite(LCD_CLK, LOW);
 digitalWrite(LCD_DATA, LOW);
 if ((data & 32) == 32)  digitalWrite(LCD_DATA, HIGH); 
 digitalWrite(LCD_CLK, HIGH);
 digitalWrite(LCD_CLK, LOW);
 digitalWrite(LCD_DATA, LOW);
 if ((data & 16) ==16)  digitalWrite(LCD_DATA, HIGH); 
 digitalWrite(LCD_CLK, HIGH);
 digitalWrite(LCD_CLK, LOW);
 digitalWrite(LCD_DATA, LOW);
 if ((data & 8) == 8)  digitalWrite(LCD_DATA, HIGH); 
 digitalWrite(LCD_CLK, HIGH);
 digitalWrite(LCD_CLK, LOW);
 digitalWrite(LCD_DATA, LOW);
 if ((data & 4) == 4)  digitalWrite(LCD_DATA, HIGH); 
 digitalWrite(LCD_CLK, HIGH);
 digitalWrite(LCD_CLK, LOW);
 digitalWrite(LCD_DATA, LOW);
 if ((data & 2) == 2)  digitalWrite(LCD_DATA, HIGH); 
 digitalWrite(LCD_CLK, HIGH);
 digitalWrite(LCD_CLK, LOW);
 digitalWrite(LCD_DATA, LOW);
 if ((data & 1) == 1)  digitalWrite(LCD_DATA, HIGH); 
 digitalWrite(LCD_CLK, HIGH);
 digitalWrite(LCD_CLK, LOW);
 
 RS_old=RS;  //запоминаю значение RS	
 digitalWrite(LCD_DATA, LOW);
}

//===============================================================
//              Задание прямоугольной области экрана
//===============================================================
void SetArea(char x1, char x2, char y1, char y2)
{
 Send_to_lcd( CMD, 0x2A );  //задаем область по X
 Send_to_lcd( DAT, x1 );    //начальная
 Send_to_lcd( DAT, x2 );    //конечная

 Send_to_lcd( CMD, 0x2B );  //задаем область по Y
 Send_to_lcd( DAT, y1 );    //начальная 
 Send_to_lcd( DAT, y2 );    //конечная

 Send_to_lcd( CMD, 0x2C );  //отправляем команду на начало записи в память и начинаем посылать данные
}                 

//===============================================================
//                          Рисуем точку 
//=============================================================== 
void Put_Pixel (char x, char y, unsigned int color) 
{
 signed char i;
 SetArea( x, x, y, y ); 
 digitalWrite(LCD_RS, HIGH);    
 
 #ifdef _8_BIT_COLOR	//(8-ми битовая цветовая палитра (256 цветов))
 Send_to_lcd( DAT, color ); //Данные - задаём цвет пикселя 
 #else			        //(16-ти битовая цветовая палитра (65536 цветов))
 Send_to_lcd( DAT, (color >> 8) ); 
 Send_to_lcd( DAT, color );
 #endif
}  
                   
//===============================================================
//           Функция прорисовки символа на дисплее
//===============================================================
void Send_Symbol (unsigned char symbol, char x, char y, int t_color, int b_color, char zoom_width, char zoom_height, int rot) 
{
 unsigned char temp_symbol, a, b, zw, zh, mask; 
  
 if (symbol>127) symbol-=64;    //Убираем отсутствующую часть таблицы ASCII
 for ( a = 0; a < 5; a++) //Перебираю 5 байт, составляющих символ
 {
  temp_symbol = font_5x8[symbol-32][a];
  zw = 0; 
  while(zw != zoom_width) //Вывод байта выполняется zw раз 
  {    
   mask=0x01;  
   switch(rot)
   {
    case 0: case 180: SetArea( x+zw, x+zw, y, y+(zoom_height*8)-1 ); break;
    case 90: case 270: SetArea( x, x+(zoom_height*8)-1, y+zw, y+zw ); break;                  
   } 
   digitalWrite(LCD_RS, HIGH); //Передаются данные          
   for ( b = 0; b < 8; b++ ) //Цикл перебирания 8 бит байта
   {         
    zh = zoom_height; //в zoom_height раз увеличится высота символа
    while(zh != 0) //Вывод пикселя выполняется z раз
    {
     if (temp_symbol&mask) 
     {
      #ifdef _8_BIT_COLOR	//(8-ми битовая цветовая палитра (256 цветов))
      Send_to_lcd( DAT, t_color ); //Данные - задаём цвет пикселя 
      #else			        //(16-ти битовая цветовая палитра (65536 цветов))
      Send_to_lcd( DAT, (t_color >> 8) ); Send_to_lcd( DAT, t_color );
      #endif
     }
     else 
     {
      #ifdef _8_BIT_COLOR	//(8-ми битовая цветовая палитра (256 цветов))
      Send_to_lcd( DAT, b_color ); //Данные - задаём цвет пикселя 
      #else			        //(16-ти битовая цветовая палитра (65536 цветов))
      Send_to_lcd( DAT, (b_color >> 8) ); Send_to_lcd( DAT, b_color );
      #endif
     }
     zh--;
    }
    mask<<=1; //Смещаю содержимое mask на 1 бит влево;    
   }
   zw++;
  }
  switch(rot)
  {
   case 0: case 180: x=x+zoom_width;  break; //Получить адрес начального пикселя по оси x для вывода очередного байта
   case 90: case 270: y=y+zoom_width; break; //Получить адрес начального пикселя по оси y для вывода очередного байта            
  }                  
 }
}  

//===============================================================
// Функция вывода одного символа ASCII-кода (из файла Symbols.h)
//===============================================================
void LCD_Putchar(char symbol, char x, char y, int t_color, int b_color, char zoom_width, char zoom_height, int rot)
{
 unsigned char m;
 if(zoom_width == 0)   zoom_width = 1;
 if(zoom_height == 0)  zoom_height = 1;
 switch (rot)
 {
  case 0:  //Начальный адрес осей Х и У - левый верхний угол дисплея   
  Send_Symbol( symbol, x, y, t_color, b_color, zoom_width, zoom_height, rot);
  break;
  //================================
  case 90:
  m=y; y=x; x=m;
  Send_to_lcd(CMD, 0x36); 
  Send_to_lcd(DAT, 0x40); //Начальный адрес осей Х и У - правый верхний угол дисплея 
  Send_Symbol( symbol, x, y, t_color, b_color, zoom_width, zoom_height, rot);
  Send_to_lcd(CMD, 0x36); 
  Send_to_lcd(DAT, 0x00);
  break;
  //================================
  case 180:       
  Send_to_lcd(CMD, 0x36); 
  Send_to_lcd(DAT, 0xC0); //Начальный адрес осей Х и У - правый нижний угол дисплея 
  Send_Symbol( symbol, x, y, t_color, b_color, zoom_width, zoom_height, rot);
  Send_to_lcd(CMD, 0x36); 
  Send_to_lcd(DAT, 0x00);
  break;
  //================================
  case 270:
  m=y; y=x; x=m;
  Send_to_lcd(CMD, 0x36); 
  Send_to_lcd(DAT, 0x80); //Начальный адрес осей Х и У - левый нижний угол дисплея  
  Send_Symbol( symbol, x, y, t_color, b_color, zoom_width, zoom_height, rot);
  Send_to_lcd(CMD, 0x36); 
  Send_to_lcd(DAT, 0x00);
  break;
  //================================
  default:
  Send_to_lcd(CMD, 0x36); 
  Send_to_lcd(DAT, 0x00); //Начальный адрес осей Х и У - левый верхний угол дисплея  
  Send_Symbol( symbol, x, y, t_color, b_color, zoom_width, zoom_height, rot);
  Send_to_lcd(CMD, 0x36); 
  Send_to_lcd(DAT, 0x00);
  //=================================    
 };  
}

//===============================================================
//          Функция вывода строки, расположенной в ram 
//===============================================================
void LCD_Puts(char *str, int x, int y,  int t_color, int b_color, char zoom_width, char zoom_height, int rot)
{
 unsigned char i=0;
              
 if(zoom_width == 0)   zoom_width = 1;
 if(zoom_height == 0)  zoom_height = 1;
   
 while (str[i]) //x и y - адрес пикселя начальной позиции; с увеличением переменной i адрес вывода очередного символа смещается на i*6 (чем организуются столбцы дисплея)  
 {      
  LCD_Putchar(str[i], x+(i*6*zoom_width), y, t_color, b_color, zoom_width, zoom_height, rot);
  i++;
 }  
}

//===============================================================
//          Функция вывода строки, расположенной во flash
//===============================================================
void LCD_Putsf(char *str, int x, int y,  int t_color, int b_color, char zoom_width, char zoom_height, int rot)
{
 unsigned char i=0;
        
 if(zoom_width == 0)   zoom_width = 1;
 if(zoom_height == 0)  zoom_height = 1;
   
 while (str[i]) //x и y - адрес пикселя начальной позиции; с увеличением переменной i адрес вывода очередного символа смещается на i*6 (чем организуются столбцы дисплея)  
 {      
  LCD_Putchar(str[i], x+(i*6*zoom_width), y, t_color, b_color, zoom_width, zoom_height, rot);
  i++;
 }  
}    

//===============================================================
//     Функция прорисовки символа на дисплее без цвета фона
//===============================================================
void Send_Symbol_Shadow (unsigned char symbol, char x, char y, int t_color, char zoom_width, char zoom_height, int rot) 
{
 unsigned char temp_symbol, a, b, zw, zh, mask; 
 char m, n;
 m=x; 
 n=y;
 if (symbol>127) symbol-=64;    //Убираем отсутствующую часть таблицы ASCII
 for ( a = 0; a < 5; a++) //Перебираю 5 байт, составляющих символ
 {  
  temp_symbol = font_5x8[symbol-32][a];
  zw = 0; 
  while(zw != zoom_width) //Вывод байта выполняется zw раз 
  {    
   switch(rot)
   {
    case 0: case 180: n=y; break;
    case 90: case 270: m=x; break;
   } 
   mask=0x01;     
   for ( b = 0; b < 8; b++ ) //Цикл перебирания 8 бит байта
   {         
    zh = 0; //в zoom_height раз увеличится высота символа
    while(zh != zoom_height) //Вывод пикселя выполняется z раз
    {
     switch(rot)
     {
      case 0: case 180:  
      if (temp_symbol&mask) 
      {
       Put_Pixel (m+zw, n+zh, t_color);
      }
      break; 
      case 90: case 270: 
      if (temp_symbol&mask) 
      {
       Put_Pixel (m+zh, n+zw, t_color);
      }
      break; //Получить адрес начального пикселя по оси y для вывода очередного байта            
     }            
     zh++;
    }
    mask<<=1; //Смещаю содержимое mask на 1 бит влево;
    switch(rot)
    {
     case 0: case 180: n=n+zoom_height; break;
     case 90: case 270: m=m+zoom_height; break;
    }          
   }
   zw++;   
  }
  switch(rot)
  {
   case 0: case 180: m=m+zoom_width; break;
   case 90: case 270: n=n+zoom_width; break;
  }           
 }
} 

//===============================================================
// Функция вывода одного символа ASCII-кода без цвета фона
//===============================================================
void LCD_Putchar_Shadow (char symbol, char x, char y, int t_color, char zoom_width, char zoom_height, int rot)
{
 unsigned char m;
 if(zoom_width == 0)   zoom_width = 1;
 if(zoom_height == 0)  zoom_height = 1;
 switch (rot)
 {
  case 0:  //Начальный адрес осей Х и У - левый верхний угол дисплея   
  Send_Symbol_Shadow( symbol, x, y, t_color, zoom_width, zoom_height, rot);
  break;
  //================================
  case 90:
  m=y; y=x; x=m;
  Send_to_lcd(CMD, 0x36); 
  Send_to_lcd(DAT, 0x40); //Начальный адрес осей Х и У - правый верхний угол дисплея 
  Send_Symbol_Shadow( symbol, x, y, t_color, zoom_width, zoom_height, rot);
  Send_to_lcd(CMD, 0x36); 
  Send_to_lcd(DAT, 0x00);
  break;
  //================================
  case 180:       
  Send_to_lcd(CMD, 0x36); 
  Send_to_lcd(DAT, 0xC0); //Начальный адрес осей Х и У - правый нижний угол дисплея 
  Send_Symbol_Shadow( symbol, x, y, t_color, zoom_width, zoom_height, rot);
  Send_to_lcd(CMD, 0x36); 
  Send_to_lcd(DAT, 0x00);
  break;
  //================================
  case 270:
  m=y; y=x; x=m;
  Send_to_lcd(CMD, 0x36); 
  Send_to_lcd(DAT, 0x80); //Начальный адрес осей Х и У - левый нижний угол дисплея  
  Send_Symbol_Shadow( symbol, x, y, t_color, zoom_width, zoom_height, rot);
  Send_to_lcd(CMD, 0x36); 
  Send_to_lcd(DAT, 0x00);
  break;
  //================================
  default:
  Send_to_lcd(CMD, 0x36); 
  Send_to_lcd(DAT, 0x00); //Начальный адрес осей Х и У - левый верхний угол дисплея  
  Send_Symbol_Shadow( symbol, x, y, t_color, zoom_width, zoom_height, rot);
  Send_to_lcd(CMD, 0x36); 
  Send_to_lcd(DAT, 0x00);
  //=================================    
 };  
}

//===============================================================
//   Функция вывода строки, расположенной в ram без цвета фона 
//===============================================================
void LCD_Puts_Shadow(char *str, int x, int y,  int t_color, char zoom_width, char zoom_height, int rot)
{
 unsigned char i=0;
                    
 if(zoom_width == 0)   zoom_width = 1;
 if(zoom_height == 0)  zoom_height = 1;
   
 while (str[i]) //x и y - адрес пикселя начальной позиции; с увеличением переменной i адрес вывода очередного символа смещается на i*6 (чем организуются столбцы дисплея)  
 {      
  LCD_Putchar_Shadow(str[i], x+(i*6*zoom_width), y, t_color, zoom_width, zoom_height, rot);
  i++;
 }  
}
   
//===============================================================
//                  ЗАЛИВКА ЭКРАНА ЦВЕТОМ 
//===============================================================
void LCD_FillScreen (unsigned int color)
{ 
 unsigned int x; 
 signed char i;
 SetArea( 0, 131, 0, 175 );   //Область всего экрана 
 digitalWrite(LCD_RS, HIGH);    
 
 //Данные - задаём цвет пикселя
 for (x = 0; x < 23232; x++)  // 23232 - это 132 * 176
 {   
  #ifdef _8_BIT_COLOR	//(8-ми битовая цветовая палитра (256 цветов))
  Send_to_lcd( DAT, color ); //Данные - задаём цвет пикселя 
  #else			//(16-ти битовая цветовая палитра (65536 цветов))
  Send_to_lcd( DAT, (color >> 8) ); Send_to_lcd( DAT, color );
  #endif
 }                 
} 


    
