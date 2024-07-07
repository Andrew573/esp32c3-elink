#ifndef _elink2_h_
#define _elink2_h_

#include "../.pio/libdeps/esp32-c3-devkitm-1/Adafruit GFX Library/Fonts/FreeSansBold24pt7b.h"
#include "../.pio/libdeps/esp32-c3-devkitm-1/Adafruit GFX Library/Fonts/FreeSansBold40pt7b.h"

#if 0
#define GxEPD2_DISPLAY_CLASS GxEPD2_3C
#define GxEPD2_DRIVER_CLASS GxEPD2_154_Z90c
#define GxEPD2_3C_IS_GxEPD2_3C true
#define IS_GxEPD2_3C(x) IS_GxEPD(GxEPD2_3C_IS_, x)
#define MAX_DISPLAY_BUFFER_SIZE 65536ul // e.g.
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
#endif

void elink_1_54_init();
void elink_hello_world();
void elink_display_date();
void elink_display_bmp();

#endif 