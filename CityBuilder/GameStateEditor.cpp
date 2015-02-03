#include <SFML/Graphics.hpp>
#include <iostream>

#include "GameState.h"
#include "GameStateEditor.h"

void GameStateEditor::draw(const float frameSpeed)
{
	this->game->window.clear(sf::Color::Black); // clear to a black screen

	this->game->window.setView(this->guiView); // setting our view to the gui view
	this->game->window.draw(this->game->background); // draw our background

	this->game->window.setView(this->gameView); // setting active to our game view
	this->city.map.draw(this->game->window, frameSpeed); // draw our map!

	this->game->window.setView(this->guiView); // set to guiview before drawing our gui
	for (auto gui : this->guiSystem) this->game->window.draw(gui.second); // draw the second in each gui entry (the gui object)

	return;
}

void GameStateEditor::update(const float framespeed)
{
	this->city.update(framespeed); // update our city

	// update the info bar at the bottom of the screen
	this->guiSystem.at("infoBar").setEntryText(0, "Day: " + std::to_string(this->city.day)); // show the current day
	this->guiSystem.at("infoBar").setEntryText(1, "$: " + std::to_string(long(this->city.funds))); // show how much moneys we gots
	this->guiSystem.at("infoBar").setEntryText(2, std::to_string(long(this->city.population)) + 
		" (" + std::to_string(long(this->city.getHomeless())) + ")"); // shows population (homeless)
	this->guiSystem.at("infoBar").setEntryText(3, std::to_string(long(this->city.employable)) +
		" (" + std::to_string(long(this->city.getUnemployed())) + ")"); // shows employable (unemployed)
	this->guiSystem.at("infoBar").setEntryText(4, tileTypeToStr(currentTile->tileType)); // show the current tile

	// highlight entries of the right click context menu
	this->guiSystem.at("rightClickMenu").highlight(
		this->guiSystem.at("rightClickMenu").getEntry(
		this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window), this->guiView)
		));

	return;
}

void GameStateEditor::handleInput()
{
	sf::Event event; // our sfml event

	sf::Vector2f guiPos = this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window), this->guiView);
	sf::Vector2f gamePos = this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window), this->gameView);

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
			

			this->resizeViews();
			break;
		}

			// mouse moving in window
		case sf::Event::MouseMoved:
		{
			// pan the camera
			if (this->actionState == ActionState::PANNING) // we are holding down middle mouse
			{
				sf::Vector2f pos = sf::Vector2f(sf::Mouse::getPosition(this->game->window) - this->panningAnchor); // get difference between where we are - where we started panning
				gameView.move(-1.0f * pos * this->zoomLevel); // moving that many positions on the game view
				panningAnchor = sf::Mouse::getPosition(this->game->window); // resetting our anchor, since we moved from the last anchor already
			}
			else if (this->actionState == ActionState::SELECTING)
			{
				sf::Vector2f pos = this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window), this->gameView); // get position of mouse right now
				selectionEnd.x = pos.y / (this->city.map.tileSize) + pos.x / (2 * this->city.map.tileSize) - this->city.map.width * 0.5 - 0.5; // not really sure about this one...
				selectionEnd.y = pos.y / (this->city.map.tileSize) - pos.x / (2 * this->city.map.tileSize) + this->city.map.width * 0.5 + 0.5; // same here...

				this->city.map.clearSelected(); // resetting our selected tiles

				if (this->currentTile->tileType == TileType::GRASS) // we are on a grass tile
				{
					this->city.map.select(selectionStart, selectionEnd, { this->currentTile->tileType, TileType::WATER }); // it looks like we are blacklisting current tile, which doesnt make sense...but other than that it is selecting our tiles we have selected so far
				}
				else
				{
					this->city.map.select(selectionStart, selectionEnd,
					{
						this->currentTile->tileType, TileType::FOREST,
						TileType::WATER, TileType::ROAD,
						TileType::RESIDENTIAL, TileType::COMMERCIAL, 
						TileType::INDUSTRIAL
					}); // blacklisting all tile types, except grass....but this once again includes our own tile type...which seems weird
				}

				// show selection cost text
				this->guiSystem.at("selectionCostText").setEntryText(0, "$" + std::to_string(this->currentTile->cost * this->city.map.numSelected)); // cost of tile * num selected
				if (this->city.funds <= this->city.map.numSelected * this->currentTile->cost)
					this->guiSystem.at("selectionCostText").highlight(0); // we have the funds to do this, highlight it
				else
					this->guiSystem.at("selectionCostText").highlight(-1); // do dont have the funds, dont hightlight it

				this->guiSystem.at("selectionCostText").setPosition(guiPos + sf::Vector2f(16, -16)); // show our selection cost to the top right of our mouse
				this->guiSystem.at("selectionCostText").show(); // show it
			}

			// highlight entries of the right click context menu
			this->guiSystem.at("rightClickMenu").highlight(this->guiSystem.at("rightClickMenu").getEntry(guiPos)); // this will only happen if we are hoving over a menu item

			break;
		}

			// mouse pressed in window
		case sf::Event::MouseButtonPressed:
		{
			// start the panning action!
			if (event.mouseButton.button == sf::Mouse::Middle) // check if its the middle mouse button
			{
				// hide our other gui elements
				this->guiSystem.at("rightClickMenu").hide();
				this->guiSystem.at("selectionCostText").hide();

				if (this->actionState != ActionState::PANNING) // we arent already panning, this matters because we will lose anchor pixels (maybe), also saves some cpu time
				{
					this->actionState = ActionState::PANNING; // start us panning yo!
					this->panningAnchor = sf::Mouse::getPosition(this->game->window); // set our anchor to where we pressed down
				}
			}
			else if (event.mouseButton.button == sf::Mouse::Left) // we are pressing left mouse
			{
				//select a context menu entry
				if (this->guiSystem.at("rightClickMenu").visible == true)
				{
					std::string msg = this->guiSystem.at("rightClickMenu").activate(guiPos); // activate our menu and save to msg
					if (msg != "null") this->currentTile = &this->game->tileAtlas.at(msg); // set our current tile to what we clicked

					this->guiSystem.at("rightClickMenu").hide();
				}
				else
				{
					// select map tile
					if (this->actionState != ActionState::SELECTING) // we arent currently selecting
					{
						this->actionState = ActionState::SELECTING; // set as we are selecting

						sf::Vector2f pos = this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window), this->gameView); // getting our position in the game view
						selectionStart.x = pos.y / (this->city.map.tileSize) + pos.x / (2 * this->city.map.tileSize) - this->city.map.width * 0.5 - 0.5; // getting our x position...still not sure how this makes sense :P
						selectionStart.y = pos.y / (this->city.map.tileSize) - pos.x / (2 * this->city.map.tileSize) + this->city.map.width * 0.5 + 0.5; // getting our y position...
					}
				}
			}
			else if (event.mouseButton.button == sf::Mouse::Right) // right click, finished with our selection
			{
				if (this->actionState == ActionState::SELECTING) // we are currently selecting
				{
					this->actionState == ActionState::NONE; // reset action state
					this->city.map.clearSelected(); // clear our selected tiles
				}
				else
				{
					// open the tile select menu
					sf::Vector2f pos = guiPos;

					if (pos.x > this->game->window.getSize().x - this->guiSystem.at("rightClickMenu").getSize().x) // we would put the menu off right of the screen
					{
						pos -= sf::Vector2f(this->guiSystem.at("rightClickMenu").getSize().x, 0); // move our position to the left
					}
					if (pos.y > this->game->window.getSize().y - this->guiSystem.at("rightClickMenu").getSize().y) // we are off the bottom of the screen
					{
						pos -= sf::Vector2f(0, this->guiSystem.at("rightClickMenu").getSize().y); // move our position up
					}

					// set our gui position and show
					this->guiSystem.at("rightClickMenu").setPosition(pos);
					this->guiSystem.at("rightClickMenu").show();
				}
			}

			break;
		}

			// mouse released
		case sf::Event::MouseButtonReleased:
		{
			// stop panning
			if (event.mouseButton.button == sf::Mouse::Middle) // we have released our middle mouse button
			{
				this->actionState = ActionState::NONE; // set the state to nothing
			}
			else if (event.mouseButton.button == sf::Mouse::Left) // we have released our left mouse button
			{
				if (this->actionState == ActionState::SELECTING) // we were selecting
				{
					// replace tiles if enough funds and a tile is selected
					if (this->currentTile != nullptr)
					{
						unsigned int cost = this->currentTile->cost * this->city.map.numSelected; // cost of our selection

						if (this->city.funds >= cost) // we have enough money for this
						{
							this->city.bulldoze(*this->currentTile); // set tiles to current tile
							this->city.funds -= this->currentTile->cost * this->city.map.numSelected; // remove our funds
							this->city.tileChanged(); // update our tiles
						}
					}

					this->guiSystem.at("selectionCostText").hide(); // hide our selection text
					this->actionState = ActionState::NONE; // no longer selecting
					this->city.map.clearSelected(); // clearing our selected tiles, not sure why we wanna do that here...
				}
			}

			break;
		}

			// check for wheel moving
		case sf::Event::MouseWheelMoved:
		{
			if (event.mouseWheel.delta < 0) // we are scrolling up...delta is negative for this for some reason
			{
				gameView.zoom(2.0f); // zoom in by a factor of 2
				zoomLevel *= 2.0f;
			}
			else // scrolling down...delta is positive for this...
			{
				gameView.zoom(0.5f); // zoom by a factor of .5...seems like zooming out
				zoomLevel *= 0.5f;
			}

			break;
		}

		default:
			break;
		}
	}

	return;
}

void GameStateEditor::resizeViews()
{
	this->gameView.setSize(this->game->window.getSize().x, this->game->window.getSize().y); // set our game view to encompase the whole screen
	this->gameView.zoom(zoomLevel); // set our zoom
	this->guiView.setSize(this->game->window.getSize().x, this->game->window.getSize().y); // set our gui view to encompase the whole screen

	// resize our info bar
	this->guiSystem.at("infoBar").setDimensions(sf::Vector2f(this->game->window.getSize().x / this->guiSystem.at("infoBar").entries.size(), 24));
	this->guiSystem.at("infoBar").setPosition(this->game->window.mapPixelToCoords(sf::Vector2i(0, this->game->window.getSize().y - 24), this->guiView));
	this->guiSystem.at("infoBar").show();

	this->game->background.setPosition(this->game->window.mapPixelToCoords(sf::Vector2i(0, 0), this->guiView)); // move background to 0,0 (basically to reset the background)

	this->game->background.setScale(
		float(this->guiView.getSize().x) / float(this->game->background.getTexture()->getSize().x),
		float(this->guiView.getSize().y) / float(this->game->background.getTexture()->getSize().y)); // rescaling the background to cover the newly sized window

	return;
}

GameStateEditor::GameStateEditor(Game* game)
{
	this->game = game; // set local to game pointer
	sf::Vector2f pos = sf::Vector2f(this->game->window.getSize()); // get window size
	this->guiView.setSize(pos); // set initial gui size
	this->gameView.setSize(pos); // set initial game size
	
	pos *= 0.5f; // find center
	
	// set center
	this->guiView.setCenter(pos);
	this->gameView.setCenter(pos);

	// create our default map - OLD WAY
	//map = Map("city_map.dat", 64, 64, game->tileAtlas);

	// create our city! - New Way
	this->city = City("city", this->game->tileSize, this->game->tileAtlas);
	this->city.shuffleTiles();

	// create gui elements
	this->guiSystem.emplace("rightClickMenu", Gui(sf::Vector2f(196, 16), 2, false, this->game->styleSheets.at("button"),  // our right click menu, for changing tiles
	{
		std::make_pair("Flatten $" + this->game->tileAtlas["grass"].getCost(), "grass"),
		std::make_pair("Forest $" + this->game->tileAtlas["forest"].getCost(), "forest"),
		std::make_pair("Residential Zone $" + this->game->tileAtlas["residential"].getCost(), "residential"),
		std::make_pair("Commercial Zone $" + this->game->tileAtlas["commercial"].getCost(), "commercial"),
		std::make_pair("Industrial Zone $" + this->game->tileAtlas["industrial"].getCost(), "industrial"),
		std::make_pair("Road $" + this->game->tileAtlas["road"].getCost(), "road")
	}));

	// im guessing this is for hover when we have an area selected
	this->guiSystem.emplace("selectionCostText", Gui(sf::Vector2f(196, 16), 0, false, this->game->styleSheets.at("text"), 
	{
		std::make_pair("", "")
	}));

	// this one looks to be our "hud", showing the perts of whats going on in game
	this->guiSystem.emplace("infoBar", Gui(sf::Vector2f(this->game->window.getSize().x / 5, 20), 2, true, 
		this->game->styleSheets.at("button"), 
		{
		std::make_pair("time", "time"),
		std::make_pair("funds", "funds"),
		std::make_pair("population", "population"),
		std::make_pair("employment", "employment"),
		std::make_pair("current tile", "tile")
		}));

	this->guiSystem.at("infoBar").setPosition(sf::Vector2f(0, this->game->window.getSize().y - 20)); // setting the position to be at the bottom
	this->guiSystem.at("infoBar").show(); // always show this shit

	this->zoomLevel = 1.0f; // our default zoom

	/* Centre the camera on the map */
	sf::Vector2f centre(this->city.map.width, this->city.map.height*0.5);
	centre *= float(this->city.map.tileSize);
	gameView.setCenter(centre);

	// default selections to nothing really
	this->selectionStart = sf::Vector2i(0, 0);
	this->selectionEnd = sf::Vector2i(0, 0);

	this->currentTile = &this->game->tileAtlas.at("grass"); // select a default grass tile, doesnt really matter

	this->actionState = ActionState::NONE; // default to no action going on
}


GameStateEditor::~GameStateEditor()
{
}
