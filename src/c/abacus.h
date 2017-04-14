#include <pebble.h>
#include <string.h>

// Important! Please implement custom back button when using this because the default back button functionality will not return.
// window_stack_pop(true);

// callback_type:
// 0 = update
// 1 = done confirmed
// 2 = done cancelled

typedef void(* AbacusCallback)( const char* result_text, char callback_type );
void abacus_start( TextLayer *text_layer, char *_text_src, Window *window, AbacusCallback callback);