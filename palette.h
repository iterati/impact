#ifndef __PALETTE_H
#define __PALETTE_H

#include <Arduino.h>
#include <avr/pgmspace.h>

#define NUM_COLORS 26

void unpackColor(uint8_t color, uint8_t& r, uint8_t& g, uint8_t& b);

#endif
