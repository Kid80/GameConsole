#include "random.h"
#include "stdbool.h"
#include "LCD_Driver.h"
#include "LCD_Touch.h"
#include "LCD_GUI.h"
#include "LCD_Bmp.h"
#include "DEV_Config.h"
#include "pico/stdio.h"
#include "hardware/watchdog.h"
#include "pico/stdlib.h"
#include "input.h"
#include "hardware/adc.h"
#include "tetris.h"

bool random_bit(){
    uint16_t result = adc_read();
    result = result & 0x1;
    return result == 0 ? false : true;
}

uint8_t random_byte(){
    uint16_t result = adc_read();
    uint8_t Result = result & 0xff;
    return Result;
}

uint8_t random_range(uint8_t min, uint8_t max){
    uint16_t result = adc_read();
    uint8_t Result = result & 0xff;
    while(Result < min || Result > max){
        result = adc_read();
        Result = result & 0xff;
    }
    return Result;
}