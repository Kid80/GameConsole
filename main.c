#include "main.h"   //Examples
#include "LCD_Driver.h"
#include "LCD_Touch.h"
#include "LCD_GUI.h"
#include "LCD_Bmp.h"
#include "DEV_Config.h"
#include "pico/stdio.h"
#include "hardware/watchdog.h"
#include "pico/stdlib.h"
#include "input.h"
#include "hardware/gpio.h"
#include "tetris.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"

void RunProgram(int (*p)()){
    int Code = (*p)();
    if(Code != 0){
        //GUI_Clear(0x001f);
        GUI_DisNum(10, 10, Code, &Font24, 0x001f, 0x0000);
        Driver_Delay_ms(2000);
    }
    else{
        GUI_Clear(0xffff);
    }
}

int main(void){
    stdio_init_all();
   	
	System_Init();
	SD_Init();

	LCD_SCAN_DIR  lcd_scan_dir = SCAN_DIR_DFT;
	LCD_Init(lcd_scan_dir,800);
	TP_Init(lcd_scan_dir);

    RunProgram(&tetris);

    while(1) {
    }
    return 0;
}