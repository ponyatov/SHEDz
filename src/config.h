#ifndef _H_CONFIG
#define _H_CONFIG

//in main.c: #define DBG

//in main.c: #define REC_VERSION 12201054

#define THEME_BLACK
//#define THEME_WHITE

#ifdef DBG
  #define TIMESLOT 5
#else
  #define TIMESLOT 15
#endif
#define SELECTED 2

#endif // _H_CONFIG