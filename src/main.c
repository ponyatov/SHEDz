#include <pebble.h>
#include "main.h"

#define SCR_W 144
#define SCR_H 168
#define SCR_LINES 8
#define TXT_H SCR_H/SCR_LINES

char txtDateTime[] = "___00 Mmm Ddd MM:SS___";
#define TM_FORMAT "___%d %b %a %M:%S___"
#define TSZ sizeof(txtDateTime)

struct { int tick; char name[TSZ]; TextLayer *tl; char ts[TSZ]; } TaskPool [] = {
  { .name = "SHED" }, { .name = "bI script" },
  { .name = "pss" }, { .name = "tea" },
  { .name = "CNC" }
};
int szTaskPool = sizeof(TaskPool)/sizeof(TaskPool[0]);
//int active = 0;

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
  // text layers
  tlDate  = text_layer_create(GRect(0, 0*TXT_H, SCR_W, TXT_H));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(tlDate));
  for (int i=0;i<szTaskPool;i++) {
    TaskPool[i].tl = text_layer_create(GRect(0, (i+1)*TXT_H, SCR_W, TXT_H));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(TaskPool[i].tl));
  }
  // setup timers
  tick_timer_service_subscribe(SECOND_UNIT, &second_tick);
  // apprun
  app_event_loop();
  // cleanup
  text_layer_destroy(tlDate);
  for (int i=0;i<szTaskPool;i++) text_layer_destroy(TaskPool[i].tl);
  window_destroy(window);
}