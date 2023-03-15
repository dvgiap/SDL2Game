#include "CommonFunc.h"
#include "BaseObject.h"
#include "game_map.h"
#include "MainObject.h"
#include "ImpTimer.h"


BaseObject g_background;
bool init()
{
	bool success = true; //Initialization flag

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cout << "SDL could not initialize! SDL Error: \n" << SDL_GetError();
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			cout << "Warning: Linear texture filtering not enabled!";
		}

		//Create window
		gWindow = SDL_CreateWindow("SDL2.0 Game",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			cout << "Window could not be created! SDL Error: \n" << SDL_GetError();
			success = false;
		}
		else
		{
			//Create renderer for window
			gScreen = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gScreen == NULL)
			{
				cout << "Renderer could not be created! SDL Error: \n" << SDL_GetError();
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gScreen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					cout << "SDL_image could not initialize! SDL_image Error: \n" << IMG_GetError();
					success = false;
				}
			}
		}
	}

	return success;
}

bool LoadBackground()
{
	bool ret = g_background.LoadImg("images/background/2.png", gScreen);
	if (!ret) return false;
	return true;
}

void close()
{
	g_background.Free();
	SDL_DestroyRenderer(gScreen);
	gScreen = NULL;

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
}
int main(int argc, char* argv[])
{
	ImpTimer fps_timer;

	if (!init()) {
		cout << "Couldn't init \n" << SDL_GetError();
		return -1;
	}
	if (!LoadBackground()) {
		cout << "Couldn't load background \n" << SDL_GetError();
		return -1;
	}
	GameMap game_map;
	game_map.LoadMap("images/map/map_type2/map.dat");
	game_map.LoadTiles(gScreen);

	MainObject p_player;
	p_player.LoadImg("images/player/p5/player_right.png", gScreen);
	p_player.set_clips();


	bool quit = false;
	while (!quit)
	{
		fps_timer.start();
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}

			p_player.HandleInputAction(e, gScreen);
		}

		SDL_SetRenderDrawColor(gScreen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);

		SDL_RenderClear(gScreen);

		g_background.Render(gScreen, NULL);
		game_map.DrawMap(gScreen);

		Map map_data = game_map.getMap();

		p_player.SetMapXY(map_data.start_x_, map_data.start_y_);
		p_player.DoPlayer(map_data);
		p_player.Show(gScreen);

		game_map.SetMap(map_data);
		game_map.DrawMap(gScreen);

		SDL_RenderPresent(gScreen);

		int real_imp_time = fps_timer.get_ticks();
		int time_one_frame = 1000 / FRAME_PER_SECOND; // (ms)

		if (real_imp_time < time_one_frame)
		{
			int delay_time = time_one_frame - real_imp_time;
			if (delay_time >= 0) SDL_Delay(delay_time);
		}


	}

	close();

	return 0;
}
