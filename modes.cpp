#include "modes.h"

uint8_t _r, _g, _b;

#define INOVA_TIMEOUT   6000
#define INOVA_OFF       0
#define INOVA_HIGH      1
#define INOVA_HIGH_HELD 2
#define INOVA_LOW       3
#define INOVA_BLINK     4
#define INOVA_CONFIG    10

void iNova::render(uint8_t *r, uint8_t *g, uint8_t *b) {
  _r = 0, _g = 0; _b = 0;

  switch (op_mode) {
    case INOVA_HIGH_HELD:
      if (tick >= 200) {
        cur_color = (cur_color + 1) % 3;
        tick = 0;
      }
      unpackColor(palette[cur_color], _r, _g, _b);
      break;
    case INOVA_HIGH:
      if (tick >= 200) {
        cur_color = (cur_color + 1) % 3;
        tick = 0;
      }
      if (button_state == 0) {
        unpackColor(palette[cur_color], _r, _g, _b);
      }
      break;
    case INOVA_LOW:
      if (tick >= 20) {
        cur_color = (cur_color + 1) % 3;
        tick = 0;
      }
      if (tick < 2 && button_state == 0) {
        unpackColor(palette[cur_color], _r, _g, _b);
      }
      break;
    case INOVA_BLINK:
      if (tick >= 600) {
        cur_color = (cur_color + 1) % 3;
        tick = 0;
      }
      if (tick < 20 && button_state == 0) {
        unpackColor(palette[cur_color], _r, _g, _b);
      }
      break;
    case INOVA_CONFIG:
      unpackColor(palette[cur_color], _r, _g, _b);
      break;
    default:
      break;
  }
  tick++;
  *r = _r; *g = _g, *b = _b;
}

void iNova::reset() {
  tick = counter = 0;
  op_mode = INOVA_OFF;
  go_off = false;
  button_state = 0;
}

int8_t iNova::handlePress(bool pressed) {
  switch (op_mode) {
    case INOVA_OFF:
      if (pressed) {
        Serial.println(F("pressed, go to high held"));
        counter = 0;
        button_state = 1;
        op_mode = INOVA_HIGH_HELD;
      } else if (counter > 1000) {
        Serial.println(F("delay reached, sleeping"));
        return CMD_SLEEP;
      }
      break;

    case INOVA_HIGH_HELD:
      if (!pressed) {
        Serial.println(F("released, go to high"));
        button_state = 0;
        if (counter >= 6000) {
          cur_color = 0;
          op_mode = INOVA_CONFIG;
        } else {
          op_mode = INOVA_HIGH;
        }
        op_mode = (counter >= 6000) ? INOVA_CONFIG : INOVA_HIGH;
      } else {
        if (counter == 6000) {
          counter++;
          return CMD_FLASH_YELLOW;
        }
      }
      break;

    case INOVA_HIGH:
      if (button_state == 0) {
        if (pressed) {
          button_state = 1;
          go_off = (counter >= INOVA_TIMEOUT) ? true : false;
          Serial.print(F("pressed ")); Serial.print(counter / 2);
          if (go_off) { Serial.println(F("ms, will turn off")); }
          else {        Serial.println(F("ms, will go to low")); }
        }
      } else {
        if (!pressed) {
          button_state = 0;
          tick = counter = cur_color = 0;
          op_mode = (go_off) ? INOVA_OFF : INOVA_LOW;
          Serial.print(F("released"));
          if (go_off) { Serial.println(F(", go to off")); }
          else {        Serial.println(F(", go to low")); }
        }
      }
      break;

    case INOVA_LOW:
      if (button_state == 0) {
        if (pressed) {
          button_state = 1;
          go_off = (counter >= INOVA_TIMEOUT) ? true : false;
          Serial.print(F("pressed ")); Serial.print(counter / 2);
          if (go_off) { Serial.println(F("ms, will turn off")); }
          else {        Serial.println(F("ms, will go to blink")); }
        }
      } else {
        if (!pressed) {
          button_state = 0;
          tick = counter = cur_color = 0;
          op_mode = (go_off) ? INOVA_OFF : INOVA_BLINK;
          Serial.print(F("released"));
          if (go_off) { Serial.println(F(", go to off")); }
          else {        Serial.println(F(", go to blink")); }
        }
      }
      break;

    case INOVA_BLINK:
      if (button_state == 0) {
        if (pressed) {
          button_state = 1;
          Serial.println(F("pressed, will go to off"));
        }
      } else {
        if (!pressed) {
          button_state = 0;
          tick = counter = cur_color = 0;
          op_mode = INOVA_OFF;
          Serial.println(F("released, go to off"));
        }
      }
      break;

    default:
      switch (button_state) {
        case 0:
          if (pressed) {
            tick = counter = 0;
            button_state = 1;
          }
          break;
        case 1:
          if (!pressed) {
            palette[cur_color] = (palette[cur_color] + 1) % NUM_COLORS;
            tick = counter = 0;
            button_state = 0;
          } else if (counter >= 2000) {
            counter = 1000;
            button_state = 2;
            return CMD_FLASH_YELLOW;
          }
          break;
        case 2:
          if (!pressed) {
            tick = counter = 0;
            button_state = 0;
            if (cur_color == 2) {
              op_mode = INOVA_OFF;
              return CMD_SAVE;
            } else {
              cur_color++;
              return CMD_NOOP;
            }
          }
          break;
        default:
          if (!pressed) {
            button_state = 0;
          }
          break;
      }
  }
  counter++;
  return CMD_NOOP;
}

void iNova::save(uint16_t addr) {
  EEPROM.update(addr + 0, palette[0]);
  EEPROM.update(addr + 1, palette[1]);
  EEPROM.update(addr + 2, palette[2]);
}

void iNova::load(uint16_t addr) {
  palette[0] = EEPROM.read(addr + 0);
  palette[1] = EEPROM.read(addr + 1);
  palette[2] = EEPROM.read(addr + 2);
}
