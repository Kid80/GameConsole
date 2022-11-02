#ifndef __INPUT_H
#define __INPUT_H

#include "stdbool.h"
#include "stdint.h"

#define D_UP    0
#define D_DOWN  1
// Pin 2 dead
#define D_LEFT  6
#define D_RIGHT 3

#define J_SW 4
#define J_X 27
#define J_Y 26
#define RANDOM 28



void input_placeholder();
void input_irq_handler(uint gpio, uint32_t events);

void input_init();

// Sets the function for GPIO 0 - D_UP
void input_set_d_up(void(*func)());
// Sets the function for GPIO 1 - D_DOWN
void input_set_d_down(void(*func)());
// Sets the function for GPIO 2 - D_LEFT
void input_set_d_left(void(*func)());
// Sets the function for GPIO 3 - D_RIGHT
void input_set_d_right(void(*func)());
// Sets the function for GPIO 4 - J_SW
void input_set_j_sw(void(*func)());

void input_clear();

uint16_t input_poll_x();
uint16_t input_poll_y();

bool random_bit();
uint8_t random_byte();

#endif