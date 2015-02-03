#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include "Gui.h"
#include "GameState.h"
#include "City.h"

enum class ActionState { NONE, PANNING, SELECTING };

class GameStateEditor : public GameState
{
private:

	ActionState actionState;

	sf::View gameView;
	sf::View guiView;

	std::map<std::string, Gui> guiSystem; // we can have multiple gui objects

	City city;

	sf::Vector2i panningAnchor;
	float zoomLevel;

	sf::Vector2i selectionStart;
	sf::Vector2i selectionEnd;

	Tile* currentTile;

public:
	virtual void draw(const float frameSpeed);
	virtual void update(const float frameSpeed);
	virtual void handleInput();
	virtual void resizeViews();

	GameStateEditor(Game* game);
	~GameStateEditor();
};

