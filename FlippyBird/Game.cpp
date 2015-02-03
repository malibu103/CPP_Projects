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

void Game::loadTextures()
{
	texmgr.loadTexture("background", "media/background.png");
	texmgr.loadTexture("scroll_bottom", "media/scroll_bottom.png");
	texmgr.loadTexture("bird", "media/flippybird_64x44.png");
	texmgr.loadTexture("tube", "media/tube.png");
	texmgr.loadTexture("tube_top", "media/tube_top.png");
	texmgr.loadTexture("numbers", "media/numbers.png");
}

void Game::loadSounds()
{
	sndmgr.loadSound("score", "media/score.aiff");
}

void Game::loadSprites()
{
	sprtmgr.loadSprite("0", texmgr.getRef("numbers"), sf::IntRect(2, 6, 33, 36));
	sprtmgr.loadSprite("1", texmgr.getRef("numbers"), sf::IntRect(37, 5, 19, 37));
	sprtmgr.loadSprite("2", texmgr.getRef("numbers"), sf::IntRect(60, 6, 28, 35));
	sprtmgr.loadSprite("3", texmgr.getRef("numbers"), sf::IntRect(92, 6, 26, 36));
	sprtmgr.loadSprite("4", texmgr.getRef("numbers"), sf::IntRect(121, 6, 34, 36));
	sprtmgr.loadSprite("5", texmgr.getRef("numbers"), sf::IntRect(158, 5, 28, 37));
	sprtmgr.loadSprite("6", texmgr.getRef("numbers"), sf::IntRect(188, 4, 34, 38));
	sprtmgr.loadSprite("7", texmgr.getRef("numbers"), sf::IntRect(224, 6, 32, 36));
	sprtmgr.loadSprite("8", texmgr.getRef("numbers"), sf::IntRect(258, 6, 33, 35));
	sprtmgr.loadSprite("9", texmgr.getRef("numbers"), sf::IntRect(294, 6, 34, 37));
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

		//std::cout << 1 / frameSpeed << std::endl;

		if (peekState() == nullptr) continue; // we dont have a state...loop

		// handle input, update stuff for current state
		peekState()->handleInput();
		peekState()->update(frameSpeed);

		// clear the screen
		this->window.clear(sf::Color::Black);

		// draw current state
		peekState()->draw();

		// display what we have drawn!
		this->window.display();
	}
}

Game::Game() // ctor
{
	// reseed our randomness
	srand((unsigned)time(NULL));

	this->loadTextures(); // load our textures
	this->loadFonts(); // load our fonts
	this->loadStylesheets(); // load our style sheets
	this->loadSounds(); // load our sounds
	this->loadSprites(); // load our sprites

	// set our global vars
	this->tubeSpawnTime = 1.6;
	this->tubeSpace = 140;
	this->gameSpeed = 250;
	this->score = 0;
	this->highscore = 0;

	this->window.create(sf::VideoMode(1024, 768), "Flippy Bird", sf::Style::Close | sf::Style::Titlebar); // create our window
	//this->window.setFramerateLimit(240); // limit  fps

	// create our bird
	bird = Bird(sf::IntRect(0, 0, 64, 44), sf::Vector2f(window.getSize().x / 2, window.getSize().y / 2), texmgr.getRef("bird"),
	{ Animation(0, 2, 0.15f), Animation(0, 2, 0.05f), Animation(0, 2, 0.15f) });

	this->background.setTexture(this->texmgr.getRef("background")); // set our background

	// add our scroll slices, this needs to be enough to make it the full width of the screen + 1 for overlap
	for (int x = 0; x < (this->window.getSize().x / this->texmgr.getRef("scroll_bottom").getSize().x) + 1; ++x)
	{
		this->scroll_bottom.push_back(sf::Sprite(this->texmgr.getRef("scroll_bottom")));
	}
}

Game::~Game() // dtor
{
	while (!this->states.empty()) popState(); // kill all current game states!
}