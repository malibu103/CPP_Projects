// CityBuilder.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Game.h"
#include "GS_Start.h"

int _tmain(int argc, _TCHAR* argv[])
{
	Game game;

	game.pushState(new GS_Start(&game)); // setting our initial gamestate as the start screen
	game.gameLoop(); // do our game loop yo

	return 0;
}

