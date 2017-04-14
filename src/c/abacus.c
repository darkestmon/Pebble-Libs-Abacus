#include "abacus.h"
#define MAX_CHARS 32

/////////////////////////////////// for abacus
static TextLayer *target_text_layer;
static Window *target_window;
static ClickConfigProvider orig_click_config_provider;
static char text_value[MAX_CHARS];
static char orig_value[MAX_CHARS];
static char char_set[] = "0123456789.";
static unsigned char char_set_pointer;
static bool for_reset;
static AppTimer *blink_timer;
static bool blinker_showing;
static char *text_src;

static AbacusCallback target_callback;

static void show_log( char *message ) {
  APP_LOG( APP_LOG_LEVEL_DEBUG, "%s", message);
}
static void show_log_string( char *title, char *message ) {
  APP_LOG( APP_LOG_LEVEL_DEBUG, "%s: %s", title, message);
}
static void show_log_char( char *title, char value ) {
  APP_LOG( APP_LOG_LEVEL_DEBUG, "%s: %i", title, value);
}

static void set_for_reset(bool next){
  for_reset = next;
}
static void update_text_layer(){
  static char display_value[MAX_CHARS];
  if(blinker_showing){
    snprintf( display_value, MAX_CHARS, "%s%c", text_value, char_set[char_set_pointer]);
  } else {
    strcpy(display_value, text_value);
  }
  strcpy( text_src, display_value);
  text_layer_set_text(target_text_layer, text_src);
}

static void blinker_timer_callback(void *context){
  blinker_showing = !blinker_showing;
  update_text_layer();
  if(blinker_showing)
    blink_timer = app_timer_register( 500, blinker_timer_callback, NULL);
  else
    blink_timer = app_timer_register( 500, blinker_timer_callback, NULL);
}
static void restart_blinker(){
  app_timer_cancel(blink_timer);
  blinker_showing = true;
  blink_timer = app_timer_register( 1000, blinker_timer_callback, NULL);
}
static void stop_blinker(){
  blinker_showing = true;
  update_text_layer();
  app_timer_cancel(blink_timer);
}
static void start_blinker(){
  app_timer_cancel(blink_timer);
  blinker_showing = false;
  strcpy( text_src, "");
  text_layer_set_text(target_text_layer, text_src);
  blink_timer = app_timer_register( 500, blinker_timer_callback, NULL);
}

static void update_text(){
  restart_blinker();
  update_text_layer();
  target_callback(text_layer_get_text(target_text_layer), 0 );
}


static void move_pointer(signed char steps){
  if( char_set_pointer + steps < 0)
    char_set_pointer=strlen(char_set)-1;
  else {
    char_set_pointer = char_set_pointer + steps;
    if( char_set_pointer >= strlen(char_set) )
      char_set_pointer=0;
  }
    
}

static signed char search_char_set(char search_char){
  signed char match_position = -1;
  for(unsigned char ctr=0; ctr<strlen(char_set); ctr++){
    if(char_set[ctr]==search_char) {
      match_position = ctr;
      break;
    }
  }
  
  return match_position;
}

static void add_new_digit(){
  char old_value[MAX_CHARS] ;
  strcpy( old_value, text_value );
  snprintf( text_value, MAX_CHARS, "%s%c", old_value, char_set[char_set_pointer]);
  
  char_set_pointer=0;
  update_text();
}

static bool delete_last_char(){
  int last_char_pointer;
  bool success;
  last_char_pointer = strlen(text_value);
  if(last_char_pointer==0){
    success = false;
  } else {
    text_value[last_char_pointer-1] = text_value[last_char_pointer];
    success = true;
  }
  return success;
}

static bool pop_last_char_from_text_value(){
  signed char search_char_result;
  bool delete_success;
  search_char_result = search_char_set(text_value[strlen(text_value)-1]);
  delete_success = delete_last_char();
  if(search_char_result>=0)
    char_set_pointer = (unsigned char) search_char_result;
  else
    char_set_pointer = 0;
  return delete_success;
}

static void end(bool cancelled){
  char callback_type;
  stop_blinker();
  
  if(cancelled)
    callback_type = 2;
  else
    callback_type = 1;
  window_set_click_config_provider(target_window, orig_click_config_provider);
  
                             
  
//   static Window *w;
//   w = window_create();
//   window_set_window_handlers(w, (WindowHandlers){
//     .load=NULL,
//     .unload=NULL
//   });
//   window_stack_push(w, true);
  
  target_callback(text_layer_get_text(target_text_layer), callback_type );
}

static void cancel(){
  strcpy(text_src, orig_value);
  text_layer_set_text(target_text_layer, orig_value);
  end(true);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  move_pointer(1);
  update_text();
  set_for_reset(true);
}
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {  
  add_new_digit();
  set_for_reset(false);
}
static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(for_reset && char_set_pointer>0)
    char_set_pointer=0;
  else
    move_pointer(5);
//   if(for_reset)
//     add_new_digit();
//   move_pointer(5);
  update_text();
  set_for_reset(false);
}
static void back_click_handler(ClickRecognizerRef recognizer, void *context) { 
  bool delete_success;
  delete_success = pop_last_char_from_text_value();
  if(delete_success){
    set_for_reset(true);
    update_text();
  } else {
    cancel();
    set_for_reset(false);
  }
}
static void select_long_press_handler(ClickRecognizerRef recognizer, void *context) {
  stop_blinker();
  strcpy(text_src, "");
  text_layer_set_text( target_text_layer, "");
}
static void select_long_release_handler(ClickRecognizerRef recognizer, void *context) {
  update_text();
  end(false);
}

static void click_config_provider(void *context){
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 1000, select_long_press_handler, select_long_release_handler);
}



void abacus_start( TextLayer *text_layer, char *_text_src, Window *window, AbacusCallback callback) {
  text_src = _text_src;
  target_callback = callback;
  target_text_layer = text_layer;
  target_window = window;
  set_for_reset(true);
  strcpy( orig_value, text_layer_get_text( target_text_layer) );
  strcpy( text_value, orig_value );
  orig_click_config_provider = window_get_click_config_provider(target_window);
  window_set_click_config_provider(target_window, click_config_provider);
  pop_last_char_from_text_value();
  update_text();
  start_blinker();
}
