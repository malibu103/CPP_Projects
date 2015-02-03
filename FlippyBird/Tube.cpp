#include "stdafx.h"
#include "Tube.h"

void Tube::draw(sf::RenderWindow &window) // simple draw code
{
	this->sprite.setPosition(this->curPosition.x, this->curPosition.y); // set our position before drawing
	window.draw(this->sprite);

	return;
}

void Tube::update(float framespeed) // update our tube horizontal positions
{
	this->curPosition.x -= this->velocity * framespeed;
}

void Tube::setPosition(float x, float y) // set our position
{
	this->curPosition.x = x;
	this->curPosition.y = y;

	// we need to update the current y position based on if it is the top tube
	if (this->isTop)
	{
		this->curPosition.y -= this->sprite.getTexture()->getSize().y; // since the origin is still the top left of the texture...we need to move it up by its height
	}
}

void Tube::setPosition(sf::Vector2f pos)
{
	this->setPosition(pos.x, pos.y);
}

int Tube::score()
{
	int scoredPoints = this->points;
	this->points = 0;

	return scoredPoints;
}

void Tube::setPoints(int points)
{
	this->points = points;
	return;
}

Tube::~Tube()
{
}
