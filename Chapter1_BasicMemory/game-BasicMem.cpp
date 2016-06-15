#define ALLEGRO_STATICLINK

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <stdio.h>

#include "..\allegro_linkers.h"
#include "Location-BasicMem.h"

Location selfLocation(12, 16);
Location ballLocation(15, 13);

BYTE map[24][32];

int keys[255];

bool checkWin()
{
	return (map[ballLocation.y][ballLocation.x] == 4);
}
void drawBackground()
{	for (int i = 0; i <= 24; i++)
	{
		for (int t = 0; t <= 32; t++)
		{
			if (map[i][t] == 1)
				al_draw_filled_rectangle(t * 20, i * 20, (t + 1) * 20, (i + 1) * 20, al_map_rgb(128, 255, 255));
			else if(map[i][t] == 2)
				al_draw_filled_rectangle(t * 20, i * 20, (t + 1) * 20, (i + 1) * 20, al_map_rgb(255, 128, 0));
			else if(map[i][t] == 3)
				al_draw_filled_rectangle(t * 20, i * 20, (t + 1) * 20, (i + 1) * 20, al_map_rgb(255, 0, 0));
			else if(map[i][t] == 4)
				al_draw_filled_rectangle(t * 20, i * 20, (t + 1) * 20, (i + 1) * 20, al_map_rgb(0, 0, 0));
		}
	}
}

void drawObjects()
{
	al_draw_filled_rectangle(selfLocation.x * 20,  selfLocation.y * 20,
								(selfLocation.x + 1) * 20, (selfLocation.y + 1) * 20,
								al_map_rgb(255, 255, 255));
	al_draw_filled_circle(ballLocation.x * 20 + 10, ballLocation.y * 20 + 10,
							10, al_map_rgb(100, 100, 100));
}

void drawHUD(ALLEGRO_FONT *font)
{
	al_draw_text(font, al_map_rgb(50, 50, 50), 10, 10, 0, "Arrow Keys:  Move");
	al_draw_text(font, al_map_rgb(50, 50, 50), 10, 25, 0, "Spacebar:     Swap");
}

void keyboardEvent()
{
	Location moveOffset;
	if (keys[ALLEGRO_KEY_UP])
		moveOffset = Location(0, -1);
	else if (keys[ALLEGRO_KEY_DOWN])
		moveOffset = Location(0, 1); 
	else if (keys[ALLEGRO_KEY_LEFT])
		moveOffset = Location(-1, 0);
	else if (keys[ALLEGRO_KEY_RIGHT])
		moveOffset = Location(1, 0);
	else if (keys[ALLEGRO_KEY_SPACE])
	{
		moveOffset = ballLocation - selfLocation;
		if (abs(moveOffset.x) + abs(moveOffset.y) <= 1)
		{
			moveOffset = selfLocation; //placehold
			selfLocation = ballLocation;
			ballLocation = moveOffset;
		}
		return;
	}
	else
		return;

	Location newSelf = selfLocation + moveOffset;
	Location newBall = ballLocation;

	if (newSelf == newBall)
		newBall += moveOffset;

	if (map[newBall.y][newBall.x] == 1 || map[newSelf.y][newSelf.x] == 1)
		return;

	ballLocation = newBall;
	selfLocation = newSelf;
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
		ZeroMemory(&keys, 255);

		FILE* file = fopen("game.map", "rb");
		if (!file)
		{
			error = "failed to initialize map!";
			break;
		}
		else
		{
			fread(map, 1, 24*32, file);
			fclose(file);
		}

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
		if(!al_install_keyboard())
		{
			error = "failed to initialize keyboard!";
			break;
		}
		if(!(font = al_load_ttf_font("arial.ttf", 18, 0)))
		{
			error = "failed to initialize font!";
			break;
		}
		if(!(timer = al_create_timer(1.0 / 10)))
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
		al_register_event_source(event_queue, al_get_keyboard_event_source());
		al_register_event_source(event_queue, al_get_timer_event_source(timer));

		al_start_timer(timer);


		while(1)
		{
			ALLEGRO_EVENT ev;
			al_wait_for_event(event_queue, &ev);

			if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
				break;
			}
			else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
					break;
				else if (ev.keyboard.keycode <= 255)
					keys[ev.keyboard.keycode] = true;
			}
			else if (ev.type == ALLEGRO_EVENT_KEY_UP)
			{
				if (ev.keyboard.keycode <= 255)
					keys[ev.keyboard.keycode] = false;
			}
			else if (ev.type == ALLEGRO_EVENT_KEY_CHAR)
				keyboardEvent();

			if (al_is_event_queue_empty(event_queue))
			{
				al_clear_to_color(al_map_rgb(255, 255, 255));
				drawBackground();
				drawObjects();

				if (checkWin())
				{
					al_draw_text(font, al_map_rgb(200, 200, 200), 320, 240, 1, "Good job! You've completed the first lab!");
					al_flip_display();
					continue;
				}

				drawHUD(font);
				al_flip_display();
			}
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


