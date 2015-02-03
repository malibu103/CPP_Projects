#pragma once
#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include "GameState.h"
#include "Gui.h"
#include "Bird.h"

class GS_Start : public GameState
{
private:
	sf::View view;

	std::map<std::string, Gui> guiSystem; // we can have multiple gui objects

	void startgame();

	float flapspot; // spot we are going to flap at for our random pretty start screen
	float timeSinceTube; // this is the time elapsed since a tube was added

	float ai_flapspeed; // how fast our AI can flap
	float ai_timesinceflap; // how long since we flapped

	void displayScore();

public:
	virtual void draw();
	virtual void update(const float frameSpeed);
	virtual void handleInput();
	virtual void resizeViews();

	GS_Start(Game* game);
	~GS_Start();
};

