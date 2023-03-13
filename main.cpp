#include "CommonFunc.h"
#include "BaseObject.h"
#include "game_map.h"

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
	bool ret = g_background.LoadImg("img/4.png", gScreen);
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
	if (!init()) {
		cout << "Couldn't init \n" << SDL_GetError();
		return -1;
	}
	if (!LoadBackground()) {
		cout << "Couldn't load background \n" << SDL_GetError();
		return -1;
	}
	GameMap game_map;
	game_map.LoadMap("map_type4/map.dat");
	game_map.LoadTiles(gScreen);

	bool quit = false;
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		}
		SDL_SetRenderDrawColor(gScreen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);

		SDL_RenderClear(gScreen);

		g_background.Render(gScreen, NULL);
		game_map.DrawMap(gScreen);

		SDL_RenderPresent(gScreen);

	}

	close();

	return 0;
}
