#include "main.h"

void click_SELECT(ClickRecognizerRef recognizer, void *context) {
  prev_active=active;
  if (active && active==selected)  shedule();
  else                             activate(selected);
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

void click_NULL(ClickRecognizerRef recognizer, void *context) { }

void hold_UP(ClickRecognizerRef recognizer, void *context) { raise_task(); }
void hold_DOWN(ClickRecognizerRef recognizer, void *context) { lower_task(); }
void hold_SELECT(ClickRecognizerRef recognizer, void *context) { delay_task(); }

void WindowsClickConfigProvider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, click_UP);
  window_long_click_subscribe(BUTTON_ID_UP, 555, hold_UP,click_NULL);
  window_single_click_subscribe(BUTTON_ID_DOWN, click_DOWN);
  window_long_click_subscribe(BUTTON_ID_DOWN, 555, hold_DOWN,click_NULL);
  window_single_click_subscribe(BUTTON_ID_SELECT, click_SELECT);
  window_long_click_subscribe(BUTTON_ID_SELECT, 555, hold_SELECT,click_NULL);
  window_single_click_subscribe(BUTTON_ID_BACK, click_BACK);
}
