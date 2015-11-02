#include "palette.h"

const PROGMEM uint8_t color_palette[26][3] = {
  {255,   0,   0},
  {252,  64,   0},
  {248, 128,   0},
  {244, 192,   0},
  {240, 240,   0},
  {192, 244,   0},
  {128, 248,   0},
  { 64, 252,   0},
  {  0, 255,   0},
  {  0, 252,  64},
  {  0, 248, 128},
  {  0, 244, 192},
  {  0, 240, 240},
  {  0, 192, 244},
  {  0, 128, 248},
  {  0,  64, 252},
  {  0,   0, 255},
  { 64,   0, 252},
  {128,   0, 248},
  {192,   0, 244},
  {240,   0, 240},
  {244,   0, 192},
  {248,   0, 128},
  {252,   0,  64},
  {255, 255, 255},
  {  0,   0,   0},
};


void unpackColor(uint8_t color, uint8_t& r, uint8_t& g, uint8_t& b) {
  uint8_t shade = color >> 6;                                // shade is first 2 bits
  uint8_t idx = color & 0b00111111;                          // palette index is last 6 bits
  r = pgm_read_byte(&color_palette[idx][0]); r = r >> shade; // get red value and shade
  g = pgm_read_byte(&color_palette[idx][1]); g = g >> shade; // get green value and shade
  b = pgm_read_byte(&color_palette[idx][2]); b = b >> shade; // get blue value and shade
}
