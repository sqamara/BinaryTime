#include <pebble.h>

static Window *s_main_window;
static TextLayer * s_time_layer[4][4];

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[4][4][2];
  for (int r = 0; r<4; r++){
    for (int c = 0; c<4; c++){
      buffer[r][c][1] = 0;
    }
  }
  int hour = tick_time->tm_hour;
  for (int i = 3; i>=0; hour/=2, i--){
    if ( hour % 2 != 0)
      buffer[1][i][0] = '1';
    else 
      buffer[1][i][0] = '0';
  }
  for (int i = 3; i>=0; hour/=2, i--){
    if ( hour % 2 != 0)
      buffer[0][i][0] = '1';
    else 
      buffer[0][i][0] = '0';
  }
  
  int min = tick_time->tm_min;
  for (int i = 3; i>=0; min/=2, i--){
    if ( min % 2 != 0)
      buffer[3][i][0] = '1';
    else 
      buffer[3][i][0] = '0';
  }
  
  for (int i = 3; i>=0; min/=2, i--){
    if ( min % 2 != 0)
      buffer[2][i][0] = '1';
    else 
      buffer[2][i][0] = '0';
  }

  // Display this time on the TextLayer
  for (int r = 0; r<4; r++) {
    for (int c = 0; c<4; c++)
      text_layer_set_text(s_time_layer[r][c], buffer[r][c]);
  }
}

static void main_window_load(Window *window) {
  for (int r = 0; r<4; r++) {
    for (int c = 0; c<4; c++) {
  // Create time TextLayer
      s_time_layer[r][c] = text_layer_create(GRect(c*32+5, r*39, 36, 52));
      text_layer_set_background_color(s_time_layer[r][c], GColorClear);
      text_layer_set_text_color(s_time_layer[r][c], GColorBlack);
      text_layer_set_text(s_time_layer[r][c], "0");

  // Improve the layout to be more like a watchface
      text_layer_set_font(s_time_layer[r][c], fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
      text_layer_set_text_alignment(s_time_layer[r][c], GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
      layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer[r][c]));
    }
  }
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  for (int r = 0; r<3; r++) {
    for (int c = 0; c<4; c++) {
      text_layer_destroy(s_time_layer[r][c]);
    }
  }
}

//  EDIT NOTHING BELOW HERE //

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

