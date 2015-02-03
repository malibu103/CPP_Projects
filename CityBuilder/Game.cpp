#pragma once
#include "stdafx.h"
#include <stack>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Game.h"
#include "GameState.h"
#include "TextureManager.h"
#include "AnimationHandler.h"
#include "Gui.h"

void Game::loadFonts()
{
	sf::Font font;
	font.loadFromFile("media/font.ttf"); // load font from file
	this->fonts["main_font"] = font; // add new font to our font map

	return;
}

void Game::loadStylesheets()
{
	this->styleSheets["button"] = GuiStyle(&this->fonts.at("main_font"), 1, 
		sf::Color(0xc6, 0xc6, 0xc6), sf::Color(0x94, 0x94, 0x94), sf::Color(0x00, 0x00, 0x00),
		sf::Color(0x61, 0x61, 0x61), sf::Color(0x94, 0x94, 0x94), sf::Color(0x00, 0x00, 0x00)); // setting our default button style
	this->styleSheets["text"] = GuiStyle(&this->fonts.at("main_font"), 0,
		sf::Color(0x00, 0x00, 0x00, 0x00), sf::Color(0x00, 0x00, 0x00), sf::Color(0xff, 0xff, 0xff),
		sf::Color(0x00, 0x00, 0x00, 0x00), sf::Color(0x00, 0x00, 0x00), sf::Color(0xff, 0x00, 0x00)); // setting our default text style
}

void Game::loadTiles() // loading all of our pretty tiles
{
	Animation staticAnim(0, 0, 1.0f); // default animation

	// GRASS
	this->tileAtlas["grass"] = 
		Tile(this->tileSize, 1, texmgr.getRef("grass"),
		{ staticAnim },
		TileType::GRASS, 50, 0, 1);

	// FOREST
	this->tileAtlas["forest"] =
		Tile(this->tileSize, 1, texmgr.getRef("forest"),
		{ staticAnim },
		TileType::FOREST, 100, 0, 1);

	// WATER
	this->tileAtlas["water"] =
		Tile(this->tileSize, 1, texmgr.getRef("water"),
		{ Animation(0, 3, 0.5f), 
		Animation(0, 3, 0.5f), 
		Animation(0, 3, 0.5f) },
		TileType::WATER, 0, 0, 1);

	// RESIDENTIAL
	this->tileAtlas["residential"] =
		Tile(this->tileSize, 2, texmgr.getRef("residential"),
		{ staticAnim, staticAnim, staticAnim,
		staticAnim, staticAnim, staticAnim },
		TileType::RESIDENTIAL, 300, 50, 6);

	// COMMERCIAL
	this->tileAtlas["commercial"] =
		Tile(this->tileSize, 2, texmgr.getRef("commercial"),
		{ staticAnim, staticAnim, staticAnim, staticAnim },
		TileType::COMMERCIAL, 300, 50, 4);

	// INDUSTRIAL
	this->tileAtlas["industrial"] =
		Tile(this->tileSize, 2, texmgr.getRef("industrial"),
		{ staticAnim, staticAnim, staticAnim, staticAnim },
		TileType::INDUSTRIAL, 300, 50, 4);

	// ROAD
	this->tileAtlas["road"] =
		Tile(this->tileSize, 1, texmgr.getRef("road"),
		{ staticAnim, staticAnim, staticAnim, 
		staticAnim, staticAnim, staticAnim, 
		staticAnim, staticAnim, staticAnim, 
		staticAnim, staticAnim },
		TileType::ROAD, 100, 0, 1);

	return;
}

void Game::loadTextures()
{
	texmgr.loadTexture("grass",       "media/grass.png");
	texmgr.loadTexture("forest",      "media/forest.png");
	texmgr.loadTexture("water",       "media/water.png");
	texmgr.loadTexture("residential", "media/residential.png");
	texmgr.loadTexture("commercial",  "media/commercial.png");
	texmgr.loadTexture("industrial",  "media/industrial.png");
	texmgr.loadTexture("road",        "media/road.png");

	texmgr.loadTexture("background",  "media/background.png");
}

void Game::pushState(GameState* state)
{
	this->states.push(state); // add state to our stack
	return;
}

void Game::popState()
{
	delete this->states.top(); // kill the top state
	this->states.pop(); // pop it off the stack

	return;
}

void Game::changeState(GameState* state)
{
	if (!this->states.empty()) // we have a current state, delete it
		popState();

	pushState(state); // add new state

	return;
}

GameState* Game::peekState()
{
	if (this->states.empty()) return nullptr; // we dont have any states
	return this->states.top(); // return current state
}

void Game::gameLoop() // main game loop
{
	sf::Clock clock;
	sf::Time elapsed;
	float frameSpeed;

	while (this->window.isOpen()) // the actual loop
	{
		// getting our frame speed
		elapsed = clock.restart();
		frameSpeed = elapsed.asSeconds();

		if (peekState() == nullptr) continue; // we dont have a state...loop

		// handle input, update stuff for current state
		peekState()->handleInput();
		peekState()->update(frameSpeed);

		// clear the screen
		this->window.clear(sf::Color::Black);

		// draw current state
		peekState()->draw(frameSpeed);

		// display what we have drawn!
		this->window.display();
	}
}

Game::Game() // ctor
{
	this->loadTextures(); // load our textures
	this->loadTiles(); // load our tiles
	this->loadFonts(); // load our fonts
	this->loadStylesheets(); // load our style sheets

	this->window.create(sf::VideoMode(1280, 720), "City Builder"); // create our window
	this->window.setFramerateLimit(60); // limit to 60  fps

	this->background.setTexture(this->texmgr.getRef("background")); // set our background
}

Game::~Game() // dtor
{
	while (!this->states.empty()) popState(); // kill all current game states!
}