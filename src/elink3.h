#ifndef elink3_h
#define elink3_h

#include <Arduino.h>

void elink3_setup();
void elink3_loop();
void loadFile(String name);
void clearScreen();
void elink_display_int(int16_t x, int16_t y, int i);
uint16_t getCenterX(String z);
uint16_t getCenterX(const char* z);
uint16_t getCenterX(int32_t z);
void display_partialLine(uint8_t line, String zf);
bool epd_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap);
uint8_t colorThresholdLimit(uint8_t val1, int8_t val2);

#endif