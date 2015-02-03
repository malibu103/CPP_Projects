#pragma once
#include <stack>
#include <map>
#include <string>
#include <SFML/Graphics.hpp>

#include "TextureManager.h"
#include "Gui.h"
#include "Bird.h"
#include "Tube.h"
#include "SoundManager.h"
#include "SpriteManager.h"

class GameState;

class Game
{
private:

	void loadTextures();
	void loadStylesheets();
	void loadFonts();
	void loadSounds();
	void loadSprites();

public:

	Bird bird;
	std::vector<Tube> tubes; // vector of tubes

	float tubeSpawnTime; // this is how long before another tube spawns
	float tubeSpace; // this is how big the space between the tubes is
	float gameSpeed; // scrolling speed
	float gravity; // gravity...
	int score; // current score
	int highscore; // current highscore

	std::stack<GameState*> states; // list of states

	sf::RenderWindow window; // game window
	TextureManager texmgr; // texture manager
	SoundManager sndmgr; // sound manager
	SpriteManager sprtmgr; // sprite manager
	sf::Sprite background; // background image (sprite)
	std::vector<sf::Sprite> scroll_bottom; // scroll bottom

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