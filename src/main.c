#include <pebble.h>
#include "mytypes.h"
#include "fire.h"
	
Window *glbWindowP;
Layer *glbFireLayerP;

#define WIDTH 144
#define HEIGHT 168
	
#define REFRESH_RATE 50
#define PROC_RATE 20
	
void handle_timer(void *data);
	
void firelayer_update(Layer *me, GContext *ctx)
{
	(void) me;
	static int toggle = 0;
	toggle++;
	
	if (toggle % PROC_RATE == 0)
	{
		char buf[12];
		time_t t = time(0);
		strftime(buf, 8, "%H%M", localtime(&t));
		buf[4] = 0;
		fire_putstring2x(5, 20, buf);
    }
	
	fire_tick(me, ctx);
}

void handle_init(void) {
	glbWindowP = window_create();
	window_stack_push(glbWindowP, true /* Animated */);
	window_set_background_color(glbWindowP, GColorWhite);

	glbFireLayerP = layer_create(layer_get_frame(window_get_root_layer(glbWindowP)));
	layer_set_update_proc(glbFireLayerP, &firelayer_update);
	layer_add_child(window_get_root_layer(glbWindowP), glbFireLayerP);
	
	// Star ttimer
	app_timer_register(REFRESH_RATE, handle_timer, 0);
	
	layer_mark_dirty(glbFireLayerP);
}

void handle_deinit(void) 
{
	window_destroy(glbWindowP);
	glbWindowP = 0;
	layer_destroy(glbFireLayerP);
	glbFireLayerP = 0;
}

void handle_timer(void *data)
{
	app_timer_register(REFRESH_RATE, handle_timer, 0);
	layer_mark_dirty(glbFireLayerP);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
