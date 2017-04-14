#include <pebble.h>
#include "abacus.h"

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_time_layer2;
static char s_time_layer_text[32] = "1230";
static char s_time_layer_2_text[32] = "5678";

static void abacus_callback_2( const char* result_text, char callback_type ){
  
}
static void abacus_callback( const char* result_text, char callback_type ){
  
//   if(callback_type==2)
//     text_layer_set_text(s_time_layer, "Thank you!");
//    abacus_start( s_time_layer2, s_main_window, NULL );
  if(callback_type>0)
  abacus_start( s_time_layer2, s_time_layer_2_text, s_main_window, abacus_callback_2 );
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_time_layer, "Goodbye!");
}
static void back_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_time_layer, "Welcome back!");
}


static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
}
static void main_window_load ( Window *window ){
  Layer *window_layer= window_get_root_layer( window );
  s_time_layer = text_layer_create( GRect(0,0, 144, 168) );
  s_time_layer2 = text_layer_create( GRect(0,72, 144, 168) );
  text_layer_set_font(s_time_layer,  fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  static char s_my_new_count[32];
  text_layer_set_text(s_time_layer, s_time_layer_text);
  text_layer_set_text(s_time_layer2, s_time_layer_2_text);
  layer_add_child( window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child( window_layer, text_layer_get_layer(s_time_layer2));
  
//   abacus_start( s_time_layer2, window, abacus_callback );
  
   abacus_start( s_time_layer, s_time_layer_text, s_main_window, abacus_callback );
  
//   abacus_start( s_time_layer2, s_time_layer_2_text, s_main_window, abacus_callback_2 );
  
//   static char boms[32];
//   strcpy( boms, "A" );
//   strcat( boms, "!" );
//   text_layer_set_text( s_time_layer, boms);   
//   strcpy( boms, "B" );
//   strcat( boms, "!" );
  //text_layer_set_text( s_time_layer2, boms);   
}
static void main_window_unload ( Window *window ){
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_time_layer2);
}

int main(void) {
  s_main_window = window_create();
  window_set_click_config_provider(s_main_window, click_config_provider);
  window_set_window_handlers( s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_stack_push(s_main_window, true);
  
  app_event_loop();
  
  window_destroy(s_main_window);
}