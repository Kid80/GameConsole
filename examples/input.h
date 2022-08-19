#ifndef __INPUT_H
#define __INPUT_H

#define D_UP    0
#define D_DOWN  1
#define D_LEFT  2
#define D_RIGHT 3

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

void input_clear();

#endif