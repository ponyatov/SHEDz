#include <pebble.h>
#include "main.h"

struct { int tick; char name[]; } TaskPool [] = {
	{0,"SHED"},
	{0,"CNC"},
  {0,"SS"}
};
int szTaskPool = sizeof(TaskPool)/sizeof(TaskPool[0]);

#define SCR_W 144
#define SCR_H 168

TextLayer *text_layer;

char txtDateTime[] = "00 Mmm Ddd HH:MM:SS";
char Scr[80*25];

char tsk[]="00000000000000000000000";
char *task(int n) {
  snprintf(tsk+0,4,"%i",TaskPool[n].tick);
  snprintf(tsk+4,sizeof(tsk)-4,"%s",TaskPool[n].name);
  return tsk;
}

void update() {
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  strftime(txtDateTime, sizeof(txtDateTime), "%d %b %a %H:%M:%S", current_time);
  for (int i=0;i<szTaskPool;i++) TaskPool[i].tick++;
  snprintf(Scr,sizeof(Scr),"= %s = %s %s %s",txtDateTime,task(0),task(1),task(2));
  text_layer_set_text(text_layer, Scr);
}
static void second_tick(struct tm* tick_time, TimeUnits units_changed) {
  update();
}

int main(void) {
  // main window
  Window *window = window_create(); window_stack_push(window, true);
  // text layer
  text_layer = text_layer_create(GRect(0, 0, SCR_W, SCR_H));;
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));
  // setup timers
  tick_timer_service_subscribe(SECOND_UNIT, &second_tick);
  // apprun
  app_event_loop();
  // cleanup
  text_layer_destroy(text_layer);
  window_destroy(window);
}