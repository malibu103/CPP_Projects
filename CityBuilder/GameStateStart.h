#pragma once
#include <SFML/Graphics.hpp>
#include "GameState.h"
#include "Gui.h"

class GameStateStart : public GameState
{
private:
	sf::View view;

	std::map<std::string, Gui> guiSystem; // we can have multiple gui objects

	void loadgame(); // to ummm...load...the game

public:
	virtual void draw(const float frameSpeed);
	virtual void update(const float frameSpeed);
	virtual void handleInput();
	virtual void resizeViews();

	GameStateStart(Game* game);
	~GameStateStart();
};

