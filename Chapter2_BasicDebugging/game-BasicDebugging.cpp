#define ALLEGRO_STATICLINK

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <stdio.h>

#include "..\allegro_linkers.h"
#include "Location-BasicDebugging.h"


Location ballLocation(10, 20);
Location ballIncrement(6, 3);


void drawFrame(ALLEGRO_FONT *font)
{
	ballLocation.x += ballIncrement.x;
	ballLocation.y += ballIncrement.y;

	if (ballLocation.x > 630 || ballLocation.x < 10)
		ballIncrement.x = ballIncrement.x * -1;

	if (ballLocation.y > 470 || ballLocation.y < 10)
		ballIncrement.y = ballIncrement.y * -1;

	al_clear_to_color(al_map_rgb(128, 128, 255));

	al_draw_filled_rectangle(320, 0, 321, 480, al_map_rgb(166, 166, 166));

	if (ballLocation.x >= 320)
	{
		al_draw_filled_rectangle(ballLocation.x-1, 0, ballLocation.x+1, 480, al_map_rgb(196, 196, 196));
		al_draw_text(font, al_map_rgb(50, 50, 50), 10, 10, 0, "Ball hidden..");
	}
	else
		al_draw_filled_circle(ballLocation.x, ballLocation.y, 10, al_map_rgb(100, 100, 100));

	al_flip_display();
}

int main(void)
{
	char* error = "";
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_FONT *font = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;

	do
	{
		if(!al_init())
		{
			error = "failed to initialize allegro!";
			break;
		}
		if(!al_init_primitives_addon())
		{
			error = "failed to initialize primitives!";
			break;
		}
		al_init_font_addon();
		if(!al_init_ttf_addon())
		{
			error = "failed to initialize ttfs!";
			break;
		}
		if(!(font = al_load_ttf_font("arial.ttf", 18, 0)))
		{
			error = "failed to initialize font!";
			break;
		}
		if(!(timer = al_create_timer(1.0 / 20)))
		{
			error = "failed to initialize timer!";
			break;
		}
		if(!(display = al_create_display(640, 480)))
		{
			error = "failed to initialize display!";
			break;
		}
		if(!(event_queue = al_create_event_queue()))
		{
			error = "failed to initialize event_queue!";
			break;
		}

		al_register_event_source(event_queue, al_get_display_event_source(display));
		al_register_event_source(event_queue, al_get_timer_event_source(timer));

		al_start_timer(timer);

		while(1)
		{
			ALLEGRO_EVENT ev;
			al_wait_for_event(event_queue, &ev);

			if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
				break;

			if (al_is_event_queue_empty(event_queue))
				drawFrame(font);
		}
	} while (0);

	if (strlen(error) > 0)
	{
		al_show_native_message_box(NULL, NULL, "error", error, NULL, NULL);
		return -1;
	}

	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}