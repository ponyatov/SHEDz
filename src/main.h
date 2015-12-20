#pragma once

#include "config.h"
#include <pebble.h>

#define ACTIVE_TASKS 5

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
#define SCR_LINES (ACTIVE_TASKS+1)
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

extern int prev_active,active,prev_selected,selected;
extern Window *window;
extern int szTaskPool;
extern void bubblesort();

extern void redraw();
extern void shedule();
extern void activate(int);

extern void incprio(int),decprio(int);
extern void save(),load();

extern TextLayer *tlDate;

#ifdef DBG
  static char txtDateTime[] = "00 Mmm Ddd MM:SS";
  #define TM_FORMAT "%d %b %a %M:%S"
#else
  static char txtDateTime[] = " 00 Mns Ddd HH:MM";
  #define TM_FORMAT " %d %b %a %H:%M"
#endif
#define TSZ sizeof(txtDateTime)

struct TaskPoolRecord { 
  int tick; int prio; char name[TSZ]; int slot;
  TextLayer *tl; char ts[TSZ]; }  ;

extern struct TaskPoolRecord TaskPool [];

#include "buttons.h"