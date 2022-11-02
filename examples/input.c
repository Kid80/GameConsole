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
#include "stdbool.h"
#include "stdint.h"

void (*input_d_up)();
void (*input_d_down)();
void (*input_d_left)();
void (*input_d_right)();
void (*input_j_sw)();

void input_placeholder(){}

void input_irq_handler(uint gpio, uint32_t events){
    if(gpio == D_UP){
        (*input_d_up)();
    }
    if(gpio == D_DOWN){
        (*input_d_down)();
    }
    if(gpio == D_LEFT){
        (*input_d_left)();
    }
    if(gpio == D_RIGHT){
        (*input_d_right)();
    }
    if(gpio == J_SW){
        //(*input_d_left)();
        (*input_j_sw)();
    }
}

/* 
Initialises the input module and input gpio.
*/
void input_init(){
    input_d_up = &input_placeholder;
    input_d_down = &input_placeholder;
    input_d_left = &input_placeholder;
    input_d_right = &input_placeholder;

    adc_init();
    adc_gpio_init(J_X);
    adc_gpio_init(J_Y);
    adc_gpio_init(RANDOM);

    gpio_set_irq_enabled_with_callback(D_UP, GPIO_IRQ_EDGE_RISE, true, &input_irq_handler);
    gpio_set_irq_enabled_with_callback(D_DOWN, GPIO_IRQ_EDGE_RISE, true, &input_irq_handler);
    gpio_set_irq_enabled_with_callback(D_LEFT, GPIO_IRQ_EDGE_RISE, true, &input_irq_handler);
    gpio_set_irq_enabled_with_callback(D_RIGHT, GPIO_IRQ_EDGE_RISE, true, &input_irq_handler);
    gpio_set_irq_enabled_with_callback(J_SW, GPIO_IRQ_EDGE_FALL, true, &input_irq_handler);
    gpio_pull_up(J_SW);
}

void input_set_d_up(void (*func)()){
    input_d_up = func;
}

void input_set_d_down(void (*func)()){
    input_d_down = func;
}

void input_set_d_left(void (*func)()){
    input_d_left = func;
}

void input_set_d_right(void (*func)()){
    input_d_right = func;
}

void input_set_j_sw(void (*func)()){
    input_j_sw = func;
}

void input_clear(){
    input_d_up = &input_placeholder;
    input_d_down = &input_placeholder;
    input_d_left = &input_placeholder;
    input_d_right = &input_placeholder;
}

uint16_t input_poll_x(){
    adc_select_input(0);
    return adc_read();
}

uint16_t input_poll_y(){
    adc_select_input(1);
    return adc_read();
}

bool random_bit(){
    adc_select_input(2);
    uint16_t result = adc_read();
    result = result & 0x1;
    return result == 0 ? false : true;
}

uint8_t random_byte(){
    adc_select_input(2);
    uint16_t result = adc_read();
    uint8_t Result = result & 0xff;
    return Result;
}

uint8_t random_range(uint8_t min, uint8_t max){
    adc_select_input(2);
    uint16_t result = adc_read();
    uint8_t Result = result & 0xff;
    while(Result < min || Result > max){
        result = adc_read();
        Result = result & 0xff;
    }
    return Result;
}