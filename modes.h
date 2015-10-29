#ifndef __MODES_H
#define __MODES_H

#include <Arduino.h>
#include <EEPROM.h>

#define CMD_NOOP          0
#define CMD_NEXT_MODE     1
#define CMD_SAVE          2
#define CMD_SLEEP         3
#define CMD_FLASH_WHITE   10
#define CMD_FLASH_RED     11
#define CMD_FLASH_YELLOW  12
#define CMD_FLASH_GREEN   13
#define CMD_FLASH_CYAN    14
#define CMD_FLASH_BLUE    15
#define CMD_FLASH_MAGENTA 16

class Mode {
  public:
    Mode() : tick(0), button_state(0) {}

    virtual void render(uint8_t *r, uint8_t *g, uint8_t *b) {}
    virtual void reset() {}
    virtual int8_t handlePress(bool pressed) {}
    virtual void load(uint16_t addr) {}
    virtual void save(uint16_t addr) {}

    uint32_t tick;
    uint8_t button_state;
};

class iNova : public Mode {
  public:
    iNova(uint8_t c0, uint8_t c1, uint8_t c2) :
      Mode(), cur_color(0), counter(0), op_mode(0), go_off(false) {
        palette[0] = c0;
        palette[1] = c1;
        palette[2] = c2;
      }

    void render(uint8_t *r, uint8_t *g, uint8_t *b);
    void reset();
    int8_t handlePress(bool pressed);
    void load(uint16_t addr);
    void save(uint16_t addr);

    uint32_t counter;
    uint8_t op_mode;
    uint8_t palette[3];
    uint8_t cur_color;
    bool go_off;
};

#endif
