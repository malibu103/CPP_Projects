#pragma once
#include <SFML/Graphics.hpp>

#include <string>
#include <map>
#include <vector>

#include "Tile.h"

class Map
{
private:

	void depthfirstsearch(std::vector<TileType>& whitelist,
		sf::Vector2i pos, int label, int type);

public:

	unsigned int width;
	unsigned int height;

	std::vector<Tile> tiles;

	// resource map
	std::vector<int> resources;

	unsigned int tileSize;
	unsigned int numRegions[1];

	// load map from disk
	void load(const std::string& filename, unsigned int width, unsigned int height,
		std::map<std::string, Tile>& tileAtlas);

	// save map to disk
	void save(const std::string& filename);

	// draw the map
	void draw(sf::RenderWindow& window, float frameSpeed);

	//checks if one position in the map is connected to another by only traversing tiles in the whitelist
	void findConnectedRegions(std::vector<TileType> whitelist, int type);

	// update the direction of directional tiles so that they face the correct way.  used to orient roads, pylons, rivers, etc
	void updateDirection(TileType tileType);

	// 0 - deselected, 1 = selected, 2 = invalid
	std::vector<char> selected;
	unsigned int numSelected;

	// select the tiles within the bounds
	void select(sf::Vector2i start, sf::Vector2i end, std::vector<TileType> blacklist);

	// deselect all tiles
	void clearSelected();

	Map() //ctor
	{
		this->numSelected = 0;
		this->tileSize = 8;
		this->width = 0;
		this->height = 0;
		this->numRegions[0] = 1;
	}

	//load map from file constructor
	Map(const std::string& filename, unsigned int width, unsigned int height,
		std::map<std::string, Tile>& tileAtlas)
	{
		this->numSelected = 0;
		this->tileSize = 8;
		load(filename, width, height, tileAtlas);
	}

	~Map();
};

