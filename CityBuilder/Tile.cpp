#include <SFML/Graphics.hpp>

#include "AnimationHandler.h"
#include "Tile.h"

void Tile::draw(sf::RenderWindow& window, float frameSpeed)
{
	// change the sprite to reflect the tile variant
	this->animHandler.changeAnim(this->tileVariant);

	// update the animation
	this->animHandler.update(frameSpeed);

	// update the sprite
	this->sprite.setTextureRect(this->animHandler.bounds);

	// draw the tile
	window.draw(this->sprite);

	return;
}

void Tile::update() // updates the tile variant based on population
{
	// if the population is at the maximum value from the tile, there is a small chance that the tile will increase its building stage
	if ((this->tileType == TileType::RESIDENTIAL ||
		this->tileType == TileType::COMMERCIAL ||
		this->tileType == TileType::INDUSTRIAL) && // residential, commercial, or industrial...self explanitory
		this->population == this->maxPopPerLevel * (this->tileVariant + 1) && // population is at max
		this->tileVariant < this->maxLevels) // we are not at the max level yet
	{
		if (rand() % int(1e4) < 1e2 / (this->tileVariant + 1))++this->tileVariant; // ok...ill have to figure this out later...
	}

	return;
}

std::string tileTypeToStr(TileType type)
{
	switch (type)
	{
	default:
	case TileType::VOID: return "Void";
	case TileType::GRASS: return "Flatten";
	case TileType::FOREST: return "Forest";
	case TileType::WATER: return "Water";
	case TileType::RESIDENTIAL: return "Residential Zone";
	case TileType::COMMERCIAL: return "Commercial Zone";
	case TileType::INDUSTRIAL: return "Industrial Zone";
	}
}

Tile::~Tile()
{
}
