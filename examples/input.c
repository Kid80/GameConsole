#include "LCD_Driver.h"
#include "LCD_Touch.h"
#include "LCD_GUI.h"
#include "LCD_Bmp.h"
#include "DEV_Config.h"
#include "pico/stdio.h"
#include "hardware/watchdog.h"
#include "pico/stdlib.h"
#include "input.h"

void (*input_d_up)();
void (*input_d_down)();
void (*input_d_left)();
void (*input_d_right)();

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
}

/* 
Initialises the input module and input gpio.
*/
void input_init(){
    input_d_up = &input_placeholder;
    input_d_down = &input_placeholder;
    input_d_left = &input_placeholder;
    input_d_right = &input_placeholder;

    gpio_set_irq_enabled_with_callback(D_UP, GPIO_IRQ_EDGE_RISE, true, &input_irq_handler);
    gpio_set_irq_enabled_with_callback(D_DOWN, GPIO_IRQ_EDGE_RISE, true, &input_irq_handler);
    gpio_set_irq_enabled_with_callback(D_LEFT, GPIO_IRQ_EDGE_RISE, true, &input_irq_handler);
    gpio_set_irq_enabled_with_callback(D_RIGHT, GPIO_IRQ_EDGE_RISE, true, &input_irq_handler);
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

void input_clear(){
    input_d_up = &input_placeholder;
    input_d_down = &input_placeholder;
    input_d_left = &input_placeholder;
    input_d_right = &input_placeholder;
}