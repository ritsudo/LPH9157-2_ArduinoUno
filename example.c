#include <stdio.h>
#include <stdlib.h>
#include "LPH91572.h"

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define SCREEN_WIDTH 132
#define SCREEN_HEIGHT 176

#define FB_WIDTH 320// 176
#define FB_HEIGHT 240// 144

int main (void)
{
	LCD_init();

	uint16_t screen[SCREEN_HEIGHT][SCREEN_WIDTH];
	uint8_t inputScreen[SCREEN_HEIGHT * SCREEN_WIDTH * 3];
	
	//Забивка скриншота 132x176
	
	FILE* f_scr = fopen("cache.bmp", "r");
	fseek(f_scr, 0x36, SEEK_SET); // skip bmp header
	fread(&inputScreen, 1, SCREEN_HEIGHT * SCREEN_WIDTH * 3, f_scr); // 2 means 16 bit, USE R5G6B5 palette
	fclose(f_scr);
	
	int cell = 0;
	int bufferEnd = sizeof(inputScreen) - 1;
	for(int x = 0; x < SCREEN_WIDTH; x++)
	{
		for(int y = 0; y < SCREEN_HEIGHT; y++)
		{
			cell = bufferEnd - (y*SCREEN_WIDTH + (SCREEN_WIDTH-x-1))*3;
			
			uint16_t newColorByte = (0b1111100);
			screen[y][x] = newColorByte;
		}
	}
	
	
	/*
	
    uint16_t old_fb[FB_HEIGHT * FB_WIDTH];

    int fd_scr = open("/dev/fb0", O_RDONLY);
    int scr_sz = FB_HEIGHT * FB_WIDTH * 2;
    uint16_t* fb_screenshot = mmap(0, scr_sz, PROT_READ, MAP_PRIVATE, fd_scr, 0);

    // scaling
    float scale_X = FB_HEIGHT / (float)SCREEN_WIDTH;
    float scale_Y = FB_WIDTH / (float)SCREEN_HEIGHT;

    int frame_cnt = 0;
    struct timespec ts1, ts2;
    clock_gettime(CLOCK_MONOTONIC, &ts1);

    for(;;) // forever cycle
    {

        for(int x = 0; x < SCREEN_WIDTH; x++)
            for(int y = 0; y < SCREEN_HEIGHT; y++)
            {
//                int fb_x = y * scale_X;
//                int fb_y = x * scale_Y;

                uint16_t px = fb_screenshot[x*FB_HEIGHT + y];

                screen[y][SCREEN_WIDTH - 1 - x] = px;
            }

        LCD_FillScreen((unsigned short*)&screen[0][0]);

        /// calc fps
        frame_cnt++;
        if(frame_cnt >= 100)
        {
            clock_gettime(CLOCK_MONOTONIC, &ts2);

            float allsec = (ts2.tv_sec - ts1.tv_sec) + (ts2.tv_nsec - ts1.tv_nsec) / 1000000000.0;
            float fps = frame_cnt / allsec;

            printf("%f FPS\n", fps);

            frame_cnt = 0;
            clock_gettime(CLOCK_MONOTONIC, &ts1);
        }

        usleep(1000);
    }

    munmap(fb_screenshot, scr_sz);
    close(fd_scr);

    printf("fin\n"); 

	*/
	LCD_FillScreen((unsigned short*)&screen[0][0]);
	
	return 0;
}

