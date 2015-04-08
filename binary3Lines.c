#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_time_layer_hour;
static TextLayer *s_time_layer_msb;
static TextLayer *s_time_layer_lsb;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "0000";
  static char bufferMSB[] = "0000";
  static char bufferLSB[] = "0000";

  // Write the current hours and minutes into the buffer
  for (int i = 3, hour = tick_time->tm_hour; i>=0; hour/=2, i--){
    if ( hour % 2 != 0)
      buffer[i] = '1';
    else 
      buffer[i] = '0';
  }
  int min = tick_time->tm_min;
  for (int i = 3; i>=0; min/=2, i--){
    if ( min % 2 != 0)
      bufferLSB[i] = '1';
    else 
      bufferLSB[i] = '0';
  }
  
  for (int i = 3; i>=0; min/=2, i--){
    if ( min % 2 != 0)
      bufferMSB[i] = '1';
    else 
      bufferMSB[i] = '0';
  }
  
  
  
    
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer_hour, buffer);
  text_layer_set_text(s_time_layer_msb, bufferMSB);
  text_layer_set_text(s_time_layer_lsb, bufferLSB);
}

static void main_window_load(Window *window) {
  // Create time TextLayer
  s_time_layer_hour = text_layer_create(GRect(0, 13, 144, 43));
  text_layer_set_background_color(s_time_layer_hour, GColorClear);
  text_layer_set_text_color(s_time_layer_hour, GColorBlack);

  s_time_layer_msb = text_layer_create(GRect(0, 56, 144, 43));
  text_layer_set_background_color(s_time_layer_msb, GColorClear);
  text_layer_set_text_color(s_time_layer_msb, GColorBlack);
  
  s_time_layer_lsb = text_layer_create(GRect(0, 99, 144, 43));
  text_layer_set_background_color(s_time_layer_lsb, GColorClear);
  text_layer_set_text_color(s_time_layer_lsb, GColorBlack);
  
  
  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer_hour, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer_hour, GTextAlignmentCenter);
  
  text_layer_set_font(s_time_layer_msb, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer_msb, GTextAlignmentCenter);
  
  text_layer_set_font(s_time_layer_lsb, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer_lsb, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer_hour));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer_msb));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer_lsb));
  
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer_hour);
  text_layer_destroy(s_time_layer_msb);
  text_layer_destroy(s_time_layer_lsb);
}

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

