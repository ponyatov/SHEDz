#pragma once

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define SCR_W 144
#define SCR_H 168
#define SCR_LINES 6
#define TXT_H SCR_H/SCR_LINES

#define FONT_DATE FONT_KEY_GOTHIC_24_BOLD
#define FONT_TASK FONT_KEY_GOTHIC_24
#define FONT_SEL FONT_KEY_GOTHIC_28_BOLD

#ifdef THEME_BLACK
  #define BACK_COLOR GColorBlack
  #define DATE_TEXT_COLOR GColorBlack
  #define DATE_BACK_COLOR GColorWhite
  #define TASK_TEXT_COLOR GColorWhite
  #define TASK_BACK_COLOR GColorBlack
  #define ACTIVE_TEXT_COLOR GColorBlack
  #define ACTIVE_BACK_COLOR GColorWhite
#endif