#include <SFML/Graphics.hpp>
#include <utility>

#include "GameStateStart.h"
#include "GameStateEditor.h"
#include "GameState.h"

void GameStateStart::draw(const float frameSpeed)
{
	this->game->window.setView(this->view); // set window view to start view

	this->game->window.clear(sf::Color::Black); // clear the screen for some reason
	this->game->window.draw(this->game->background); // draw this background.

	for (auto gui : this->guiSystem) 
		this->game->window.draw(gui.second); // draw each gui object

	return;
}

void GameStateStart::update(const float frameSpeed)
{

}

void GameStateStart::handleInput()
{
	sf::Event event; // our sfml event

	sf::Vector2f mousePos = this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window), this->view);

	while (this->game->window.pollEvent(event)) // event loop!
	{
		switch (event.type)
		{
			// close the window
		case sf::Event::Closed:
		{
			game->window.close();
			break;
		}

			// resize the window
		case sf::Event::Resized:
		{
			this->view.setSize(event.size.width, event.size.height); // set our view to encompase the whole screen

			// set gui position
			sf::Vector2f pos = sf::Vector2f(event.size.width, event.size.height);
			pos *= 0.5f;
			pos = this->game->window.mapPixelToCoords(sf::Vector2i(pos), this->view);
			this->guiSystem.at("menu").setPosition(pos);

			this->resizeViews();
			break;
		}
			// check for mouse movement (see if we are hovering over menu items)
		case sf::Event::MouseMoved:
		{
				this->guiSystem.at("menu").highlight(this->guiSystem.at("menu").getEntry(mousePos));
				break;
		}

			// check for mouse button pressed
		case sf::Event::MouseButtonPressed:
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				std::string msg = this->guiSystem.at("menu").activate(mousePos); // get menu trigger

				if (msg == "load_game")
				{
					this->loadgame();
				}
			}

			break;
		}

			// handle key press
		case sf::Event::KeyPressed:
		{
			if (event.key.code == sf::Keyboard::Escape) this->game->window.close(); // close window on escape
			//else if (event.key.code == sf::Keyboard::Space) this->loadgame(); // load the main game screen on spacebar
			break;
		}

		default:
			break;
		}
	}

	return;
}

void GameStateStart::loadgame()
{
	this->game->pushState(new GameStateEditor(this->game)); // load up the editor state

	return;
}

void GameStateStart::resizeViews()
{
	this->game->background.setPosition(this->game->window.mapPixelToCoords(sf::Vector2i(0, 0), this->view)); // move background to 0,0 (basically to reset the background)

	this->game->background.setScale(
		float(this->view.getSize().x) / float(this->game->background.getTexture()->getSize().x),
		float(this->view.getSize().y) / float(this->game->background.getTexture()->getSize().y)); // rescaling the background to cover the newly sized window

	return;
}

GameStateStart::GameStateStart(Game* game)
{
	this->game = game; // set local game var to our main game pointer
	sf::Vector2f pos = sf::Vector2f(this->game->window.getSize()); // getting size of the window to set our initial view size
	this->view.setSize(pos); // setting our initial view size
	pos *= 0.5f; // get center point
	this->view.setCenter(pos); // set center of view
	this->resizeViews();

	this->guiSystem.emplace("menu", Gui(sf::Vector2f(192, 32), 4, false, game->styleSheets.at("button"),
	{ std::make_pair("Load Game", "load_game") })); // create our menu

	this->guiSystem.at("menu").setPosition(pos); // position our menu
	this->guiSystem.at("menu").setOrigin(96, 32 * 1 / 2); // center menu by setting menu origin to its center
	this->guiSystem.at("menu").show();
}


GameStateStart::~GameStateStart()
{
}
