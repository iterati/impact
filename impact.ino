// ********************************************************************
// **** DO NOT EDIT ***************************************************
// ********************************************************************
#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <avr/wdt.h>
#include "LowPower.h"
#include "elapsedMillis.h"
#include "modes.h"

#define EEPROM_VERSION  99
// ********************************************************************

#define PIN_R 9
#define PIN_G 6
#define PIN_B 5
#define PIN_BUTTON 2
#define PIN_LDO A3

#define ADDR_SLEEPING   1022
#define ADDR_VERSION    1023

elapsedMicros limiter = 0;
iNova mode0 = iNova(0, 8, 16);
Mode *modes[1] = {&mode0};
Mode *mode = modes[0];

void setup() {
  Serial.begin(57600);

  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_LDO, OUTPUT);

  attachInterrupt(0, pushInterrupt, FALLING);
  if (EEPROM.read(ADDR_SLEEPING)) {
    EEPROM.write(ADDR_SLEEPING, 0);
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }
  detachInterrupt(0);

  if (EEPROM_VERSION != EEPROM.read(ADDR_VERSION)) {
    mode->save(1004);
    EEPROM.update(ADDR_VERSION, EEPROM_VERSION);
  } else {
    mode->load(1004);
  }

  digitalWrite(PIN_LDO, HIGH);
  mode->reset();

  noInterrupts();
  ADCSRA = 0; // Disable ADC
  TCCR0B = (TCCR0B & 0b11111000) | 0b001;  // no prescaler ~1/64ms
  TCCR1B = (TCCR1B & 0b11111000) | 0b001;  // no prescaler ~1/64ms
  interrupts();

  delay(40);
  limiter = 0;
}

void loop() {
  uint8_t r, g, b;
  handleModePress(digitalRead(PIN_BUTTON) == LOW);
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

  EEPROM.write(ADDR_SLEEPING, 1);
  delay(4000);
  wdt_enable(WDTO_15MS);

  digitalWrite(PIN_LDO, HIGH);
}

void pushInterrupt() {
}
