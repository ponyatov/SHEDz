//#define DBG
#define REC_VERSION 25121048
#define DELAY_MINUTES 11

// #define PRIO_SHIFT TIMESLOT
#define PRIO_SHIFT 1

/////////////////////////////////////////////////////////////////////////////////////////////////

#include "main.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
struct TaskPoolRecord TaskPool [] = {
  { .name = "CNC" },
  { .name = "Machining" },
  { .name = "Android" },
  { .name = "VREP" },
  { .name = "bIlol" },
//   { .name = "Pij2d" },
//   { .name = "Pays" },
// #ifndef DBG
//   { .name = "Modula" },
// #endif    
  { .name = "SHED" },
  { .name = "e@mail" }
};
/////////////////////////////////////////////////////////////////////////////////////////////////

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
  if (persist_exists(STOR_VERSION) &&\
  persist_read_int(STOR_VERSION)==REC_VERSION) {
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
  } else for (int i=0;i<szTaskPool;i++) {
        TaskPool[i].tick=1;
        TaskPool[i].prio=1; }
  bubblesort();
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
  for (int i=0;i<min(szTaskPool,ACTIVE_TASKS);i++) {
    // mark selected
    if (i==selected-1)
      text_layer_set_font(TaskPool[i].tl, fonts_get_system_font(FONT_SEL));
    else
      text_layer_set_font(TaskPool[i].tl, fonts_get_system_font(FONT_TASK));
    // print task info
    snprintf(TaskPool[i].ts,TSZ," %i/%i %s %i/%i",\
             TaskPool[i].slot,TaskPool[i].tick,TaskPool[i].name,TaskPool[i].prio,TaskPool[i].delayed);
    text_layer_set_text(TaskPool[i].tl, TaskPool[i].ts);
  }
}

int formula(int idx) {  // magic prio calc formula
  return TaskPool[idx].prio * TaskPool[idx].tick;// + TaskPool[idx].delayed;
}  
void swap(int A, int B) {
  // A->R
  rec.tick = TaskPool[A].tick;
  rec.prio = TaskPool[A].prio;
  rec.slot = TaskPool[A].slot;
  strcpy(rec.name,TaskPool[A].name);
  // B->A
  TaskPool[A].tick = TaskPool[B].tick;
  TaskPool[A].prio = TaskPool[B].prio;
  TaskPool[A].slot = TaskPool[B].slot;
  strcpy(TaskPool[A].name,TaskPool[B].name);
  // R->B
  TaskPool[B].tick = rec.tick;
  TaskPool[B].prio = rec.prio;
  TaskPool[B].slot = rec.slot;
  strcpy(TaskPool[B].name,rec.name);
  if (selected==A) selected=B;//prev_selected=selected,
  if (selected==B) selected=A;//prev_selected=selected,
  if (active==A) active=B; if (active==B) active=A;
}
void bubblesort() { // sort task pool using simple time*prio order formula
  bool swapped;
  do {
    swapped=false;
    for (int i=0;i<szTaskPool-1;i++)
      if (formula(i)>formula(i+1)) { swap(i,i+1); swapped=true; }
  } while (swapped);
}

void shedule() {
  vibes_short_pulse();
  prev_active=active; active=0;
  bubblesort();
  prev_selected=selected; selected=1;
  redraw();
}

void raise_task() {
  TaskPool[selected-1].prio -= PRIO_SHIFT;
  if (TaskPool[selected-1].prio<1) TaskPool[selected-1].prio=1;
  shedule();
}

void lower_task() {
  TaskPool[selected-1].prio += PRIO_SHIFT;
  shedule();
}

void delay_task() {
  if (selected) TaskPool[selected-1].delayed+=DELAY_MINUTES;
  shedule();
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
  // proc delayed
  for (int i=0;i<szTaskPool;i++)
    if (TaskPool[i].delayed) { // do block for delayed only
      TaskPool[i].delayed--;
      if (!TaskPool[i].delayed) shedule(); // on delay end trigger
    }
  // redraw screen
  redraw();
}

static void second_tick(struct tm* tick_time, TimeUnits units_changed) { update(); }

void activate(int i) {
prev_active=active; active=i; TaskPool[active-1].slot=TIMESLOT; 
}

void decprio(int i) { if (TaskPool[i].prio) TaskPool[i].prio--; shedule(); }
void incprio(int i) { TaskPool[i].prio++; shedule(); }

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