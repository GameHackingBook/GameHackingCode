#define ALLEGRO_STATICLINK

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <stdio.h>

#include "..\allegro_linkers.h"
#include "Location-FindingFiles.h"


Location ballLocation(30, 20);
Location ballIncrement(6, 3);
Location playerPaddle(0, 235);
Location computerPaddle(0, 235);

int keys[255];

int score = 0;
int highscore = 0;
bool gameRunning = true;


void getHighscorePath(wchar_t* path)
{
	wchar_t temp[MAX_PATH];
	GetTempPathW(MAX_PATH, &temp[0]);
	wsprintfW(path, L"%s\\gh_hs_p.dat", temp);
}

void loadHighscore()
{
	wchar_t path[MAX_PATH+40];
	getHighscorePath(path);

	FILE* file = _wfopen(path, L"rb");
	if (file)
	{
		fread(&highscore, 1, 4, file);
		fclose(file);
	}
}

void saveHighscore()
{
	wchar_t path[MAX_PATH+40];
	getHighscorePath(path);

	FILE* file = _wfopen(path, L"wb");
	if (file)
	{
		fwrite(&highscore, 1, 4, file);
		fclose(file);
	}
}

void drawFrame(ALLEGRO_FONT *font)
{
	if (gameRunning)
	{
		ballLocation.x += ballIncrement.x;
		ballLocation.y += ballIncrement.y;

		if (ballLocation.y > 470 || ballLocation.y < 10)
			ballIncrement.y = ballIncrement.y * -1;

		if (ballLocation.x > 625)
			ballIncrement.x = ballIncrement.x * -1;

		if (ballLocation.x < 16 && ballIncrement.x < 0)
		{
			ballLocation.x = 16;
			if (ballLocation.y >= playerPaddle.y - 35 && ballLocation.y <= playerPaddle.y + 35)
			{
				score += 10;
				if (score > highscore)
					highscore = score;
				if (score % 50 == 0)
				{
					ballIncrement.x *= 1.5;
					ballIncrement.y *= 1.5;
				}
				ballIncrement.x = ballIncrement.x * -1;
			}
			else
				gameRunning = false;
		}
	}

	if (ballIncrement.x > 0)
	{
		if (computerPaddle.y < ballLocation.y && ballLocation.y - computerPaddle.y > 4)
			computerPaddle.y += 8;
		else if (computerPaddle.y > ballLocation.y && computerPaddle.y - ballLocation.y > 4)
			computerPaddle.y -= 8;

		if (computerPaddle.y < 35)
			computerPaddle.y = 35;
		else if (computerPaddle.y > 445)
			computerPaddle.y = 445;
	}

	al_clear_to_color(al_map_rgb(128, 128, 255));

	al_draw_filled_circle(ballLocation.x, ballLocation.y, 10, al_map_rgb(100, 100, 100));

	al_draw_filled_rectangle(0, playerPaddle.y - 35, 5, playerPaddle.y + 35, al_map_rgb(196, 196, 196));
	al_draw_filled_rectangle(635, computerPaddle.y - 35, 640, computerPaddle.y + 35, al_map_rgb(196, 196, 196));

	if (!gameRunning)
		al_draw_text(font, al_map_rgb(200, 200, 200), 320, 240, 1, "Game over!");
	else
	{
		char scoretext[1024];
		sprintf(scoretext, "Score: %d", score);
		al_draw_text(font, al_map_rgb(200, 200, 200), 300, 3, 0, scoretext);

		sprintf(scoretext, "High Score: %d", highscore);
		al_draw_text(font, al_map_rgb(200, 200, 200), 300, 22, 0, scoretext);
	}

	al_flip_display();
}

void keyboardEvent()
{
	if (!gameRunning)
		return;

	Location moveOffset;
	if (keys[ALLEGRO_KEY_UP])
		moveOffset = Location(0, -8);
	else if (keys[ALLEGRO_KEY_DOWN])
		moveOffset = Location(0, 8); 
	else
		return;

	Location newPaddle = playerPaddle + moveOffset;

	if (newPaddle.y < 35)
		newPaddle.y = 35;
	else if (newPaddle.y > 445)
		newPaddle.y = 445;

	playerPaddle = newPaddle;
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
		loadHighscore();
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
		al_register_event_source(event_queue, al_get_keyboard_event_source());
		al_register_event_source(event_queue, al_get_timer_event_source(timer));

		al_start_timer(timer);

		while(1)
		{
			ALLEGRO_EVENT ev;
			al_wait_for_event(event_queue, &ev);

			if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
				break;
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

			if (al_is_event_queue_empty(event_queue) && ev.type == ALLEGRO_EVENT_TIMER)
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
	saveHighscore();

	return 0;
}