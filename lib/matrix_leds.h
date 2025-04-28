#ifndef MATRIX_LEDS_H
#define MATRIX_LEDS_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pio_matrix.pio.h"

extern uint sm;

void setup_led_matrix();

uint8_t obter_index(uint8_t i);

void draw_code_matrix();

void matrix_code(const uint32_t code1,const uint32_t code2, const uint32_t multi);

extern uint32_t status[25];

extern const uint32_t colors[10];

#endif