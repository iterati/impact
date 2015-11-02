// ********************************************************************
// **** DO NOT EDIT ***************************************************
// ********************************************************************
#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "elapsedMillis.h"
#include "modes.h"

uint8_t current_version = 11;
// ********************************************************************

#define PIN_R 9
#define PIN_G 6
#define PIN_B 5
#define PIN_BUTTON 2
#define PIN_LDO A3

elapsedMicros limiter = 0;
iNova mode0 = iNova(0, 8, 16);
Mode *modes[1] = {&mode0};
Mode *mode = modes[0];

void setup() {
  power_spi_disable();
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_LDO, OUTPUT);
  digitalWrite(PIN_LDO, HIGH);

  if (current_version != EEPROM.read(1000)) {
    mode->save(1004);
    EEPROM.update(1000, current_version);
  } else {
    mode->load(1004);
  }

  mode->reset();

  noInterrupts();
  ADCSRA = 0; // Disable ADC
  TCCR0B = (TCCR0B & 0b11111000) | 0b001;  // no prescaler ~1/64ms
  TCCR1B = (TCCR1B & 0b11111000) | 0b001;  // no prescaler ~1/64ms
  interrupts();

  delay(4000);
  limiter = 0;
}

void loop() {
  handleModePress(digitalRead(PIN_BUTTON) == LOW);

  uint8_t r, g, b;
  mode->render(&r, &g, &b);
  writeFrame(r, g, b);
}

void writeFrame(uint8_t r, uint8_t g, uint8_t b) {
  while (limiter < 32000) {}
  limiter = 0;

  analogWrite(PIN_R, r);
  analogWrite(PIN_G, g);
  analogWrite(PIN_B, b);
}

void flash(uint8_t r, uint8_t g, uint8_t b, uint8_t flashes) {
  for (uint8_t i = 0; i < flashes; i++) {
    for (uint8_t j = 0; j < 200; j++) {
      if (j < 100) { writeFrame(r, g, b); }
      else {         writeFrame(0, 0, 0); }
    }
  }
}

void handleModePress(bool pressed) {
  int8_t rtn_code = mode->handlePress(pressed);
  switch (rtn_code) {
    case CMD_NEXT_MODE:
      break;
    case CMD_SAVE:
      flash(128, 128, 128, 5);
      mode->save(1004);
      break;
    case CMD_SLEEP:
      enterSleep();
      break;
    case CMD_FLASH_WHITE:
      flash(128, 128, 128, 5);
      break;
    case CMD_FLASH_RED:
      flash(128, 0, 0, 5);
      break;
    case CMD_FLASH_YELLOW:
      flash(128, 128, 0, 5);
      break;
    case CMD_FLASH_GREEN:
      flash(0, 128, 0, 5);
      break;
    case CMD_FLASH_CYAN:
      flash(0, 128, 128, 5);
      break;
    case CMD_FLASH_BLUE:
      flash(0, 0, 128, 5);
      break;
    case CMD_FLASH_MAGENTA:
      flash(128, 0, 128, 5);
      break;
    default:
      break;
  }
}

void enterSleep() {
  writeFrame(0, 0, 0);
  digitalWrite(PIN_LDO, LOW);
  delay(4000);

  // Set sleep mode and power down
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  noInterrupts();
  attachInterrupt(0, pushInterrupt, FALLING);
  EIFR = bit(INTF0);
  MCUCR = bit(BODS) | bit(BODSE);
  MCUCR = bit(BODS);
  interrupts();
  sleep_cpu();

  // Wait until button is releaed
  /* while (digitalRead(PIN_BUTTON) == LOW) {} */
  digitalWrite(PIN_LDO, HIGH);
  /* mode->reset(); */
}

void pushInterrupt() {
  sleep_disable();
  detachInterrupt(0);
}
