#include <pebble.h>
#include "main.h"

#define SCR_W 144
#define SCR_H 168
#define SCR_LINES 8
#define TXT_H SCR_H/SCR_LINES

#define FONT_DATE FONT_KEY_GOTHIC_18_BOLD
#define FONT_TASK FONT_KEY_GOTHIC_18
#define FONT_SEL FONT_KEY_GOTHIC_18_BOLD

#define BACK_COLOR GColorBlack
#define DATE_TEXT_COLOR GColorWhite
#define DATE_BACK_COLOR GColorBlack
#define TASK_TEXT_COLOR GColorWhite
#define TASK_BACK_COLOR GColorBlack

#define ACTIVE_TEXT_COLOR GColorBlack
#define ACTIVE_BACK_COLOR GColorWhite

char txtDateTime[] = "   00 Mmm Ddd MM:SS ";
#define TM_FORMAT "   %d %b %a %M:%S "
#define TSZ sizeof(txtDateTime)

struct { int tick; char name[TSZ]; TextLayer *tl; char ts[TSZ]; } TaskPool [] = {
  { .name = "SHED" }, { .name = "bI script" },
  { .name = "tea" },
  { .name = "CNC" }
};
#define SELECTED 2
int szTaskPool = sizeof(TaskPool)/sizeof(TaskPool[0]);
int prev_active=0,active=0;
int selected=SELECTED,prev_selected=SELECTED-1;

TextLayer *tlDate;

void update() {
  // date&time
  time_t now = time(NULL); struct tm *current_time = localtime(&now);
  strftime(txtDateTime, sizeof(txtDateTime), TM_FORMAT, current_time);
  text_layer_set_text(tlDate, txtDateTime);
  // tasks
  for (int i=0;i<szTaskPool;i++) {
    TaskPool[i].tick+=i+1;
    snprintf(TaskPool[i].ts,TSZ," %i %s",TaskPool[i].tick,TaskPool[i].name);
    text_layer_set_text(TaskPool[i].tl, TaskPool[i].ts);
  }
}
static void second_tick(struct tm* tick_time, TimeUnits units_changed) { update(); }

int main(void) {
  // main window
  Window *window = window_create(); window_stack_push(window, true);
  window_set_background_color(window,BACK_COLOR);
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
  if (active) {
    text_layer_set_background_color(TaskPool[active-1].tl, ACTIVE_BACK_COLOR);
    text_layer_set_text_color(TaskPool[active-1].tl, ACTIVE_TEXT_COLOR);
  }
  if (selected)
    text_layer_set_font(TaskPool[selected-1].tl, fonts_get_system_font(FONT_SEL));
  // setup timers
  tick_timer_service_subscribe(SECOND_UNIT, &second_tick);
  // apprun
  app_event_loop();
  // cleanup
  text_layer_destroy(tlDate);
  for (int i=0;i<szTaskPool;i++) text_layer_destroy(TaskPool[i].tl);
  window_destroy(window);
}