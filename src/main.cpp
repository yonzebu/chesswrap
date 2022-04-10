// chesswrap.cpp : Defines the entry point for the application.
//

#include "SDL2/SDL.h"
#include "SDL2/SDL_main.h"

#include <iostream>

#include "global.h"
#include "game_state.h"

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	ChessWrapGame game;
	game.run();


	SDL_Quit();

	return 0;
}
