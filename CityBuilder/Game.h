#pragma once
#include <stack>
#include <map>
#include <string>
#include <SFML/Graphics.hpp>

#include "TextureManager.h"
#include "Tile.h"
#include "Gui.h"

class GameState;

class Game
{
private:

	void loadTextures();
	void loadTiles();
	void loadStylesheets();
	void loadFonts();

public:

	const static int tileSize = 8;

	std::stack<GameState*> states; // list of states

	sf::RenderWindow window; // game window
	TextureManager texmgr; // texture manager
	sf::Sprite background; // background image (sprite)

	std::map<std::string, Tile> tileAtlas;
	std::map<std::string, GuiStyle> styleSheets; // defined style sheets
	std::map<std::string, sf::Font> fonts; // fonts to load

	void pushState(GameState* state); // function to add new state to list
	void popState(); // function to remove state
	void changeState(GameState* state); // function to change to a new gamestate
	GameState* peekState(); // function to get current game state

	void gameLoop(); // gameloop...

	Game(); //ctor
	~Game(); //dtor

};