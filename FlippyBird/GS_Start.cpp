#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <utility>
#include <iostream>
#include <string>
#include <sstream>

#include "GS_Start.h"
#include "GameState.h"
#include "Tube.h"

void GS_Start::displayScore()
{
	// converting our score to string
	std::ostringstream strm;
	strm << this->game->score;
	std::string scorestr = strm.str();
	
	float scorelen = 0; // score length in pixels

	// need a vector of sprites, which sucks but we need to loop twice to do the offsets to align in center
	std::vector<sf::Sprite> scores;

	for (char& c : scorestr) // loop through each char in string
	{
		std::stringstream sstrm;
		sstrm << c;
		sf::Sprite num = this->game->sprtmgr.getRef(sstrm.str()); // set sprite
		num.setPosition(sf::Vector2f((this->game->window.getSize().x / 2) + scorelen, num.getTextureRect().height + 10));

		scores.push_back(num); // add to our vector of sprite numbers

		scorelen += num.getTextureRect().width; // increment score length
	}

	scorelen /= 2; // cut score length in half...this is now our offset

	for (auto &score : scores) // loop 
	{
		score.setPosition(score.getPosition().x - scorelen, score.getPosition().y); // move by our offset
		this->game->window.draw(score); // draw it!
	}
}

void GS_Start::draw()
{
	this->game->window.setView(this->view); // set window view to start view

	this->game->window.clear(sf::Color::Black); // clear the screen for some reason
	this->game->window.draw(this->game->background); // draw this background.

	// draw our tubes
	for (int i = 0; i < this->game->tubes.size(); ++i)
	{
		this->game->tubes[i].draw(this->game->window); // draw tube
	}

	// display score
	this->displayScore();

	// draw our bird
	this->game->bird.draw(this->game->window);

	// draw the scroll bottom
	for (auto &sb : this->game->scroll_bottom)
	{
		this->game->window.draw(sb);
	}

	for (auto gui : this->guiSystem)
		this->game->window.draw(gui.second); // draw each gui object

	return;
}

void GS_Start::update(const float frameSpeed)
{
	float xpos;
	float move_x = (frameSpeed * this->game->gameSpeed);
	int prev_x;
	float prev_x_pos;

	// update the scroll bottom
	for (int x = 0; x < this->game->scroll_bottom.size(); ++x)
	{
		sf::Sprite &sb = this->game->scroll_bottom[x];

		// get the sprite before us
		if (x == 0) // we are at the first element...get the last element
			prev_x = this->game->scroll_bottom.size() - 1;
		else
			prev_x = x - 1; // get the previous element

		prev_x_pos = this->game->scroll_bottom[prev_x].getPosition().x + sb.getTextureRect().width; // right edge position of sprite before us

		xpos = sb.getPosition().x - move_x;
		if (xpos + sb.getTextureRect().width < 0) // we are all the way off the screen, send this to the back
		{
			sb.setPosition(prev_x_pos, sb.getPosition().y);
		}
		else // set new position based on frame speed
		{
			sb.setPosition(xpos, sb.getPosition().y);
			
			if (sb.getPosition().x > prev_x_pos) // we arent at the next pixel...we have a gap...set x to prev
			{
				// need a hack for the first element...since it will be updated before it's "previous"
				if (x == 0)
					sb.setPosition(prev_x_pos - move_x, sb.getPosition().y); // move it to where the last element will be
				else
					sb.setPosition(prev_x_pos, sb.getPosition().y); // follow the leader
			}
		}
	}

	// update our tubes
	for (int i = 0; i < this->game->tubes.size(); ++i)
	{
		// update our tubes
		this->game->tubes[i].update(frameSpeed); // update our tube
	}

	// see if we need to add a new tube
	this->timeSinceTube += frameSpeed;

	if (this->timeSinceTube >= this->game->tubeSpawnTime) // we need to spawn a new tube!
	{
		int tubeIDX = -1;

		// find an available set of tubes if there are some
		for (int i = 0; i < this->game->tubes.size(); ++i) // will loop through our tubeset
		{
			if (this->game->tubes[i].curPosition.x + this->game->tubes[i].sprite.getTextureRect().width <= 0) // we are off the screen...we can use these tubes!
			{
				tubeIDX = i; // set our tube index
				break; // get out of our loop
			}
		}

		// get our positions
		int playarea_height = this->game->window.getSize().y - (this->game->texmgr.getRef("scroll_bottom").getSize().y);
		int rand_botstart = ((rand() % (playarea_height - (100 + (int)this->game->tubeSpace))) + 50); // so this should spawn tubes giving at least 100 pixels for the bottom and top to protrude
		int pos_x = this->game->window.getSize().x;
		int pos_y = playarea_height - rand_botstart;
		int pos_y_top = pos_y - this->game->tubeSpace;

		if (tubeIDX == -1) // we didnt find any, lets add some
		{
			Tube bot_tube = Tube(sf::Vector2f(pos_x, pos_y), this->game->texmgr.getRef("tube"), this->game->gameSpeed, 0, 1);
			Tube top_tube = Tube(sf::Vector2f(pos_x, pos_y_top), this->game->texmgr.getRef("tube_top"), this->game->gameSpeed, 1, 0); // create our new tubes

			this->game->tubes.push_back(bot_tube); // add to our tubes vector
			this->game->tubes.push_back(top_tube); // add to our tubes vector
		}
		else
		{
			// update our "off the map" tubes to spawn again, give bottom tube points
			this->game->tubes[tubeIDX].setPosition(pos_x, pos_y);
			this->game->tubes[tubeIDX].setPoints(1);
			this->game->tubes[tubeIDX + 1].setPosition(pos_x, pos_y_top);
		}

		this->timeSinceTube -= this->game->tubeSpawnTime;
	}

	// update our bird
	this->game->bird.update(frameSpeed);

	// auto play AI...lets look for the closest tube set
	this->ai_timesinceflap += frameSpeed;
	int close_tube = -1;
	int close_tube_x = 1000;

	for (int i = 0; i < this->game->tubes.size(); ++i)
	{
		if (i % 2 == 0) // we are on an even tube (bottom tube)
		{
			float bird_tail = this->game->bird.curPosition.x - (this->game->bird.sprite.getTexture()->getSize().x / 2); // x position of our birds tail...shouldnt ever change...but calc just in case
			float tube_x = this->game->tubes[i].curPosition.x + this->game->tubes[i].sprite.getTexture()->getSize().x; // this will get the x position of the RIGHT side of the tube

			if (bird_tail < tube_x && tube_x < close_tube_x) // so, this tube hasnt passed our tail...we still need to worry about it...and its closer than another other
			{
				close_tube_x = tube_x; // set our close tube x
				close_tube = i; // set our closest tube
			}

			// since we are looping through tubes, lets see if we "scored" from this one
			if (bird_tail > this->game->tubes[i].curPosition.x) // we have passed this one
			{
				int scoredPoints = this->game->tubes[i].score();

				if (scoredPoints > 0) // we scored!
				{
					this->game->score += scoredPoints; // increment our score
					this->game->sndmgr.getRef("score").play(); // play our sound
				}
			}
		}
	}

	// we should either have no tubes on the screen yet, or our closest tube...so if we have a closest tube lets make sure we dont fall into it!
	if (close_tube > -1) // we have a close tube
	{
		float bird_bot = this->game->bird.curPosition.y + (this->game->bird.sprite.getTexture()->getSize().y / 2); // this will be the bottom y position of our bird
		float bot_tube_top = this->game->tubes[close_tube].curPosition.y;

		if (bird_bot + 5 > bot_tube_top && ai_timesinceflap >= ai_flapspeed) // so we are either below or within 1 pixel of the bottom of the tube...and we can flap...we need to!
		{
			this->game->bird.flap();
			this->ai_timesinceflap = 0; // reset our time since flap
		}
	}

	// check if we are sorta close to the bottom...if so flap...and we dont have a tube on screen
	if (this->game->bird.curPosition.y >= this->game->scroll_bottom[0].getPosition().y - flapspot && close_tube == -1)
	{
		if (ai_timesinceflap >= ai_flapspeed) // we are due for a flap!
		{
			this->game->bird.flap();
			this->flapspot = (rand() % 100 + 250); // set our random flap spot
			this->ai_timesinceflap = 0; // reset our time since flap
		}
	}

	// update our bird position
	this->game->bird.sprite.setPosition(this->game->bird.curPosition);

	// update our bird angle
	this->game->bird.sprite.setRotation(this->game->bird.curAngle);
}

void GS_Start::handleInput()
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

												  if (msg == "start_game")
												  {
													  this->startgame();
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

void GS_Start::startgame()
{
	//this->game->pushState(new GameStateEditor(this->game)); // load up the editor state

	return;
}

void GS_Start::resizeViews()
{
	sf::Vector2f pos = sf::Vector2f(this->game->window.getSize()); // getting size of the window to set our initial view size

	this->view.setSize(pos); // set our view to encompase the whole screen

	// set gui position
	pos *= 0.5f;
	pos.y *= 1.25f;
	pos = this->game->window.mapPixelToCoords(sf::Vector2i(pos), this->view);
	this->guiSystem.at("menu").setPosition(pos);

	this->game->background.setPosition(this->game->window.mapPixelToCoords(sf::Vector2i(0, 0), this->view)); // move background to 0,0 (basically to reset the background)

	this->game->background.setScale(
		float(this->view.getSize().x) / float(this->game->background.getTexture()->getSize().x),
		float(this->view.getSize().y) / float(this->game->background.getTexture()->getSize().y)); // rescaling the background to cover the newly sized window

	// position the scroll bottoms
	for (int x = 0; x < this->game->scroll_bottom.size(); ++x)
	{
		sf::Vector2i pos;
		pos.x = x * this->game->scroll_bottom[x].getTexture()->getSize().x; // set x position, based on how many scroll bottoms we have loaded
		pos.y = this->view.getSize().y - this->game->scroll_bottom[x].getTexture()->getSize().y; // set y position, will always be window height minus height of texture

		this->game->scroll_bottom[x].setPosition(this->game->window.mapPixelToCoords(pos, this->view)); // set postion
	}

	return;
}

GS_Start::GS_Start(Game* game)
{
	this->flapspot = (rand() % 100 + 250); // set our random flap spot
	
	// some vars
	this->timeSinceTube = 0;
	this->ai_flapspeed = .20;
	this->ai_timesinceflap = 0;

	this->game = game; // set local game var to our main game pointer
	sf::Vector2f pos = sf::Vector2f(this->game->window.getSize()); // getting size of the window to set our initial view size
	pos *= 0.5f; // get center point
	this->view.setCenter(pos); // set center of view

	this->guiSystem.emplace("menu", Gui(sf::Vector2f(192, 32), 4, false, game->styleSheets.at("button"),
	{ std::make_pair("Start!", "start_game") })); // create our menu
	this->guiSystem.at("menu").setOrigin(96, 32 * 1 / 2); // center menu by setting menu origin to its center

	this->resizeViews();

	this->guiSystem.at("menu").show();
}


GS_Start::~GS_Start()
{
}
