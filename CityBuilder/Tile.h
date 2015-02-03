#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

#include "AnimationHandler.h"

enum class TileType { VOID, GRASS, FOREST, WATER, RESIDENTIAL, COMMERCIAL, INDUSTRIAL, ROAD };

std::string tileTypeToStr(TileType type);

class Tile
{
public:
	AnimationHandler animHandler; // dec animation handler
	sf::Sprite sprite; // a sprite

	TileType tileType; // what tile type we are

	// tile variant, allowing for different looking versions of the same tile
	int tileVariant;

	// region ids of the tile, tiles in the same region are connected...first is for transport
	unsigned int regions[1];

	// placement cost of the tile
	unsigned int cost;

	// current residents / employees
	double population;

	// maximum population per growth stage / tile variant
	unsigned int maxPopPerLevel;

	// maximum number of building levels
	unsigned int maxLevels;

	// production output per customer / worker per day, either monetary or goods
	float production;

	// goods stored
	float storedGoods;

	Tile() {} // ctor

	Tile(const unsigned int tileSize, const unsigned int height, sf::Texture& texture, 
		const std::vector<Animation>& animations, 
		const TileType tileType, const unsigned int cost, const unsigned int maxPopPerLevel, 
		const unsigned int maxLevels) // overload, this is really the only way we are going to call it, as the base constructor is empty
	{
		this->tileType = tileType;
		this->tileVariant = 0;
		this->regions[0] = 0;

		this->cost = cost;
		this->population = 0;
		this->maxPopPerLevel = maxPopPerLevel;
		this->maxLevels = maxLevels;
		this->production = 0;
		this->storedGoods = 0;

		this->sprite.setOrigin(sf::Vector2f(0.0f, tileSize*(height - 1)));
		this->sprite.setTexture(texture);
		this->animHandler.frameSize = sf::IntRect(0, 0, tileSize * 2, tileSize * height);
		for (auto animation : animations)
		{
			this->animHandler.addAnim(animation);
		}
		this->animHandler.update(0.0f);
	}

	void draw(sf::RenderWindow& window, float frameSpeed); // method to draw the tile

	void update();

	//return a string containing the display cost of the tile
	std::string getCost()
	{
		return std::to_string(this->cost);
	}

	~Tile(); // dtor
};

