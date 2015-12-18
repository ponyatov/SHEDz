/////////////////////////////////////////////////////////////////////////////////////////////////
//#define DBG
#define THEME_BLACK
//#define THEME_WHITE
#define ACTIVE_TASKS 5
/////////////////////////////////////////////////////////////////////////////////////////////////

#include <pebble.h>
#include "main.h"

#ifdef DBG
  char txtDateTime[] = "00 Mmm Ddd MM:SS";
  #define TM_FORMAT "%d %b %a %M:%S"
#else
  char txtDateTime[] = " 00 Mns Ddd HH:MM";
  #define TM_FORMAT " %d %b %a %H:%M"
#endif
#define TSZ sizeof(txtDateTime)

/////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef DBG
  #define TIMESLOT 5
#else
  #define TIMESLOT 30
#endif
struct { int tick; int prio; char name[TSZ]; TextLayer *tl; char ts[TSZ]; int slot;} TaskPool [] = {
  { .name = "CNC" , .prio=5 },
  { .name = "Pij2d", .prio=2 },
  { .name = "bI script" },
  { .name = "VREP" , .prio=3 }, 
  { .name = "SHED" },
  { .name = "LLVM" },
  { .name = "Modula" },
};
#define SELECTED 2
/////////////////////////////////////////////////////////////////////////////////////////////////

#define REC_VERSION 18121141
struct RECORD { int tick; int prio; int slot; char name[TSZ]; } rec;
#define STOR_VERSION 0
#define STOR_ACTIVE STOR_VERSION+1
#define STOR_SELECTED STOR_ACTIVE+1
#define STOR_TASK STOR_SELECTED+1

void save() {
  persist_write_int(STOR_VERSION,REC_VERSION);
  persist_write_int(STOR_ACTIVE,active);
  persist_write_int(STOR_SELECTED,selected);
  for (int i=0;i<szTaskPool;i++) {
    rec.tick=TaskPool[i].tick;
    rec.prio=TaskPool[i].prio;
    rec.slot=TaskPool[i].slot;
    strcpy(rec.name,TaskPool[i].name);
    persist_write_data(STOR_TASK+i,&rec,sizeof(rec));
  }
}
void load() {
  if (persist_exists(STOR_VERSION) && persist_read_int(STOR_VERSION)==REC_VERSION) {
  if (persist_exists(STOR_ACTIVE))
    active=persist_read_int(STOR_ACTIVE);
  if (persist_exists(STOR_SELECTED))
    selected=persist_read_int(STOR_SELECTED);
  for (int i=0;i<szTaskPool;i++) {
    if (persist_exists(STOR_TASK+i)) {
      persist_read_data(STOR_TASK+i,&rec,sizeof(rec));
      TaskPool[i].tick=rec.tick;
      TaskPool[i].prio=rec.prio;
      TaskPool[i].slot=rec.slot;
      strcpy(TaskPool[i].name,rec.name);
    }
  }
  }
}

int szTaskPool = sizeof(TaskPool)/sizeof(TaskPool[0]);
int prev_active=0,active=0;
int selected=SELECTED,prev_selected=SELECTED-1;

TextLayer *tlDate;

void redraw() {
  if (prev_active) {
    text_layer_set_background_color(TaskPool[prev_active-1].tl, TASK_BACK_COLOR);
    text_layer_set_text_color(TaskPool[prev_active-1].tl, TASK_TEXT_COLOR);
  }
  if (active) {
    text_layer_set_background_color(TaskPool[active-1].tl, ACTIVE_BACK_COLOR);
    text_layer_set_text_color(TaskPool[active-1].tl, ACTIVE_TEXT_COLOR);
  }
  if (prev_selected)
    text_layer_set_font(TaskPool[prev_selected-1].tl, fonts_get_system_font(FONT_TASK));
  if (selected)
    text_layer_set_font(TaskPool[selected-1].tl, fonts_get_system_font(FONT_SEL));
  for (int i=0;i<min(szTaskPool,ACTIVE_TASKS);i++) {
    snprintf(TaskPool[i].ts,TSZ," %i/%i %s %i",TaskPool[i].slot,TaskPool[i].tick,TaskPool[i].name,TaskPool[i].prio);
    text_layer_set_text(TaskPool[i].tl, TaskPool[i].ts);
  }
}

void shedule() {
  vibes_short_pulse();
  prev_active=active; active=0;
  redraw();
}

void update() {
  // date&time
  time_t now = time(NULL); struct tm *current_time = localtime(&now);
  strftime(txtDateTime, sizeof(txtDateTime), TM_FORMAT, current_time);
  text_layer_set_text(tlDate, txtDateTime);
  // update tasks
  if (active) {
    TaskPool[active-1].tick++; TaskPool[active-1].slot--;
    if (!TaskPool[active-1].slot) shedule();
  }
  // redraw screen
  redraw();
}

static void second_tick(struct tm* tick_time, TimeUnits units_changed) { update(); }

void click_UP(ClickRecognizerRef recognizer, void *context) {
  prev_selected=selected;
  selected--; if (selected<1) selected = min(ACTIVE_TASKS,szTaskPool);
  redraw();
}

void click_DOWN(ClickRecognizerRef recognizer, void *context) {
  prev_selected=selected;
  selected++; if (selected>min(ACTIVE_TASKS,szTaskPool)) selected = 1;
  redraw();
}

void click_SELECT(ClickRecognizerRef recognizer, void *context) {
  prev_active=active;
  if (active && active==selected)  active=0;
  else                             { active=selected; TaskPool[active-1].slot=TIMESLOT; }
  redraw();
}

void click_BACK(ClickRecognizerRef recognizer, void *context) {
  save();
  // cleanup
  text_layer_destroy(tlDate);
  for (int i=0;i<szTaskPool;i++) text_layer_destroy(TaskPool[i].tl);
  window_destroy(window);
  // terminate
  window_stack_pop_all(true);
}

void WindowsClickConfigProvider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, click_UP);
  window_single_click_subscribe(BUTTON_ID_DOWN, click_DOWN);
  window_single_click_subscribe(BUTTON_ID_SELECT, click_SELECT);
  window_single_click_subscribe(BUTTON_ID_BACK, click_BACK);
}

Window *window;
int main(void) {
  load();
  // main window
  window = window_create(); window_set_fullscreen(window, true);
  window_set_background_color(window,BACK_COLOR);
  window_stack_push(window, true);
  window_set_click_config_provider(window, WindowsClickConfigProvider);  
  // text layers
  tlDate  = text_layer_create(GRect(0, 0*TXT_H, SCR_W, TXT_H));
  text_layer_set_font(tlDate, fonts_get_system_font(FONT_DATE));
  text_layer_set_text_color(tlDate, DATE_TEXT_COLOR);
  text_layer_set_background_color(tlDate, DATE_BACK_COLOR);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(tlDate));
  for (int i=0;i<szTaskPool;i++) {
    TaskPool[i].tl = text_layer_create(GRect(0, (i+1)*TXT_H, SCR_W, TXT_H));
    text_layer_set_font(TaskPool[i].tl, fonts_get_system_font(FONT_TASK));  
    text_layer_set_text_color(TaskPool[i].tl, TASK_TEXT_COLOR);
    text_layer_set_background_color(TaskPool[i].tl, TASK_BACK_COLOR);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(TaskPool[i].tl));
  }
  redraw();
  // setup timers
  #ifdef DBG
    tick_timer_service_subscribe(SECOND_UNIT, &second_tick);
  #else
    tick_timer_service_subscribe(MINUTE_UNIT, &second_tick);
  #endif
  // apprun
  app_event_loop();
}