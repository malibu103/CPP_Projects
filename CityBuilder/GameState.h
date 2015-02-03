#pragma once
#include "Game.h"

class GameState
{
public:

	Game* game;

	virtual void draw(const float frameSpeed) = 0;
	virtual void update(const float frameSpeed) = 0;
	virtual void handleInput() = 0;
	virtual void resizeViews() = 0;
};