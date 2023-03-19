#include "CommonFunc.h"
#include "BaseObject.h"
#include "game_map.h"
#include "MainObject.h"
#include "ImpTimer.h"
#include "ThreatsObject.h"


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
	bool ret = g_background.LoadImg("images/background.png", gScreen);
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

vector<ThreatsObject*> MakeThreatList()
{
	vector<ThreatsObject*> list_threats;

	ThreatsObject* dynamic_threats = new ThreatsObject[20];

	for (int i = 0; i < 20; i++)
	{
		ThreatsObject* p_threat = dynamic_threats + i;
		if (p_threat != NULL)
		{
			p_threat->LoadImg("images/thread_left.png", gScreen);
			p_threat->set_clips();
			p_threat->set_type_move(ThreatsObject::MOVE_IN_SPACE_THREAT);

			p_threat->set_x_pos(500 + i * 500);
			p_threat->set_y_pos(200);

			int pos1 = p_threat->get_x_pos() - 60;
			int pos2 = p_threat->get_x_pos() + 60;
			p_threat->SetAnimationPos(pos1, pos2);
			p_threat->set_input_left(1);

			list_threats.push_back(p_threat);
		}
	}

	ThreatsObject* threats_objs = new ThreatsObject[20];

	for (int i = 0; i < 20; i++)
	{
		ThreatsObject* p_threat = threats_objs + i;
		if (p_threat != NULL)
		{
			p_threat->LoadImg("images/threats1.png", gScreen);
			p_threat->set_clips();
			p_threat->set_x_pos(700 + i*1200);
			p_threat->set_y_pos(250);
			p_threat->set_type_move(ThreatsObject::STATIC_THREAT);
			p_threat->set_input_left(0);

			BulletObject* p_bullet = new BulletObject();
			p_threat->InitBullet(p_bullet, gScreen);

			list_threats.push_back(p_threat);
		}
	}

	return list_threats;
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
	game_map.LoadMap("images/map.dat");
	game_map.LoadTiles(gScreen);

	MainObject p_player;
	p_player.LoadImg("images/player_right.png", gScreen);
	p_player.set_clips();

	vector<ThreatsObject*> threats_list = MakeThreatList();



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

		p_player.HandleBullet(gScreen);

		p_player.SetMapXY(map_data.start_x_, map_data.start_y_);
		p_player.DoPlayer(map_data);
		p_player.Show(gScreen);

		game_map.SetMap(map_data);
		game_map.DrawMap(gScreen);

		for (int i = 0; i < threats_list.size(); i++)
		{
			ThreatsObject* p_threat = threats_list.at(i);
			if (p_threat != NULL)
			{
				p_threat->SetMapXY(map_data.start_x_, map_data.start_y_);
				p_threat->ImpMoveType(gScreen);
				p_threat->DoPlayer(map_data);
				p_threat->MakeBullet(gScreen, SCREEN_WIDTH, SCREEN_HEIGHT);
				p_threat->Show(gScreen);
			}
		}


		SDL_RenderPresent(gScreen);

		int real_imp_time = fps_timer.get_ticks();
		int time_one_frame = 1000 / FRAME_PER_SECOND; // (ms)

		if (real_imp_time < time_one_frame)
		{
			int delay_time = time_one_frame - real_imp_time;
			if (delay_time >= 0) SDL_Delay(delay_time);
		}


	}

	for (int i = 0; i < threats_list.size(); i++)
	{
		ThreatsObject* p_threat = threats_list.at(i);
		if (p_threat != NULL)
		{
			p_threat->Free();
			p_threat = NULL;
		}
	}

	close();

	return 0;
}
