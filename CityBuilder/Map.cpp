#include <SFML/Graphics.hpp>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
//#include <algorithm>

#include "Map.h"
#include "Tile.h"

// load map from disk
void Map::load(const std::string& filename, unsigned int width, unsigned int height,
	std::map<std::string, Tile>& tileAtlas)
{
	std::ifstream inputFile; // file stream
	inputFile.open(filename, std::ios::in | std::ios::binary); // reading our file, read in binary

	this->width = width;
	this->height = height;

	for (int pos = 0; pos < this->width * this->height; ++pos)
	{
		this->resources.push_back(255); // set default resources for this tile?
		this->selected.push_back(0); // setting the tile default selected value as not selected

		TileType tileType;
		inputFile.read((char*)&tileType, sizeof(int)); // reads the (next?) tile type in file?

		switch (tileType) // add to our tiles based on type from tile atlas that was passed
		{
		default:
		case TileType::VOID:
		case TileType::GRASS:
			this->tiles.push_back(tileAtlas.at("grass"));
			break;
		case TileType::FOREST:
			this->tiles.push_back(tileAtlas.at("forest"));
			break;
		case TileType::WATER:
			this->tiles.push_back(tileAtlas.at("water"));
			break;
		case TileType::RESIDENTIAL:
			this->tiles.push_back(tileAtlas.at("residential"));
			break;
		case TileType::COMMERCIAL:
			this->tiles.push_back(tileAtlas.at("commercial"));
			break;
		case TileType::INDUSTRIAL:
			this->tiles.push_back(tileAtlas.at("industrial"));
			break;
		case TileType::ROAD:
			this->tiles.push_back(tileAtlas.at("road"));
			break;
		}

		Tile& tile = this->tiles.back(); // our latest added tile

		// all of the following read the next bits from the file and fill our tile variables that are missing
		inputFile.read((char*)&tile.tileVariant, sizeof(int)); // our variant
		inputFile.read((char*)&tile.regions, sizeof(int) * 1); // what regions it belongs to
		inputFile.read((char*)&tile.population, sizeof(double)); // current population
		inputFile.read((char*)&tile.storedGoods, sizeof(float)); // current goods
	}

	inputFile.close(); // close our file, of course

	return;
}


void Map::save(const std::string& filename)
{
	std::ofstream outputFile; // open our file stream
	outputFile.open(filename, std::ios::out | std::ios::binary); // open the file in write mode as binary

	for (auto tile : this->tiles) // basically a foreach loop
	{
		// all of these will write the bits from our current tile to the file as binary due to how we opened the file stream
		outputFile.write((char*)&tile.tileType, sizeof(int)); // tile type
		outputFile.write((char*)&tile.tileVariant, sizeof(int)); // variant
		outputFile.write((char*)&tile.regions, sizeof(int) * 1); // regions
		outputFile.write((char*)&tile.population, sizeof(double)); // current population
		outputFile.write((char*)&tile.storedGoods, sizeof(float)); // current stored goods
	}

	outputFile.close(); // close our file, of course

	return;
}

void Map::draw(sf::RenderWindow& window, float frameSpeed)
{
	for (int y = 0; y < this->height; ++y)
	{
		for (int x = 0; x < this->width; ++x)
		{
			// set the position of the tile in our 2nd world
			sf::Vector2f pos;
			pos.x = (x - y) * this->tileSize + this->width * this->tileSize; // hmm need to figure out the math here
			pos.y = (x + y) * this->tileSize * 0.5;
			this->tiles[y*this->width + x].sprite.setPosition(pos);

			// change the color of the tile if selected
			if (this->selected[y*this->width + x])
				this->tiles[y*this->width + x].sprite.setColor(sf::Color(0x7d, 0x7d, 0x7d)); // halve the brightness aparantly
			else
				this->tiles[y*this->width + x].sprite.setColor(sf::Color(0xff, 0xff, 0xff));

			// draw the tile
			this->tiles[y*this->width + x].draw(window, frameSpeed);

		}
	}

	return;
}

void Map::updateDirection(TileType tileType)
{
	for (int y = 0; y < this->height; ++y) // loop through the height of map
	{
		for (int x = 0; x < this->width; ++x) // loop through the width of map
		{
			int pos = y * this->width + x; // index of vector

			if (this->tiles[pos].tileType != tileType) continue; // current tile type doesnt match passed tile type, ignore current tile

			bool adjacentTiles[3][3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } }; // set our blank adjacent tiles (bunch of booleans)

			// check for adjacent tiles of the same type
			if (x > 0 && y > 0)
				adjacentTiles[0][0] = (this->tiles[(y - 1)*this->width + (x - 1)].tileType == tileType); // checking tile above us
			if (y > 0)
				adjacentTiles[0][3] = (this->tiles[(y - 1)*this->width + (x    )].tileType == tileType); // check tile top right of us
			if (x < this->width-1 && y > 0)
				adjacentTiles[0][4] = (this->tiles[(y - 1)*this->width + (x + 1)].tileType == tileType); // check tile right of us
			if (x > 0)
				adjacentTiles[1][0] = (this->tiles[(y    )*this->width + (x - 1)].tileType == tileType); // check tile top left of us
			if (x < width - 1)
				adjacentTiles[1][5] = (this->tiles[(y    )*this->width + (x + 1)].tileType == tileType); // check tile bottom right of us
			if (x > 0 && y < this->height - 1)
				adjacentTiles[2][0] = (this->tiles[(y + 1)*this->width + (x - 1)].tileType == tileType); // check tile left of us
			if (y < this->height-1)
				adjacentTiles[2][6] = (this->tiles[(y + 1)*this->width + (x    )].tileType == tileType); // check tile bottom left of us
			if (x < this->width-1 && y < this->height-1)
				adjacentTiles[2][7] = (this->tiles[(y + 1)*this->width + (x + 1)].tileType == tileType); // check tile left of us

			// change the tile variant depending on the tile position
			if (adjacentTiles[1][0] && adjacentTiles[1][8] && adjacentTiles[0][9] && adjacentTiles[2][10])
				this->tiles[pos].tileVariant = 2;
			else if (adjacentTiles[1][0] && adjacentTiles[1][11] && adjacentTiles[0][12])
				this->tiles[pos].tileVariant = 7;
			else if (adjacentTiles[1][0] && adjacentTiles[1][13] && adjacentTiles[2][14])
				this->tiles[pos].tileVariant = 8;
			else if (adjacentTiles[0][15] && adjacentTiles[2][16] && adjacentTiles[1][0])
				this->tiles[pos].tileVariant = 9;
			else if (adjacentTiles[0][16] && adjacentTiles[2][17] && adjacentTiles[1][18])
				this->tiles[pos].tileVariant = 10;
			else if (adjacentTiles[1][0] && adjacentTiles[1][19])
				this->tiles[pos].tileVariant = 0;
			else if (adjacentTiles[0][20] && adjacentTiles[2][21])
				this->tiles[pos].tileVariant = 1;
			else if (adjacentTiles[2][22] && adjacentTiles[1][0])
				this->tiles[pos].tileVariant = 3;
			else if (adjacentTiles[0][23] && adjacentTiles[1][24])
				this->tiles[pos].tileVariant = 4;
			else if (adjacentTiles[1][0] && adjacentTiles[0][25])
				this->tiles[pos].tileVariant = 5;
			else if (adjacentTiles[2][26] && adjacentTiles[1][27])
				this->tiles[pos].tileVariant = 6;
			else if (adjacentTiles[1][0])
				this->tiles[pos].tileVariant = 0;
			else if (adjacentTiles[1][28])
				this->tiles[pos].tileVariant = 0;
			else if (adjacentTiles[0][29])
				this->tiles[pos].tileVariant = 1;
			else if (adjacentTiles[2][30])
				this->tiles[pos].tileVariant = 1;
		}
	}

	return;
}

void Map::depthfirstsearch(std::vector<TileType>& whitelist,
	sf::Vector2i pos, int label, int regionType = 0)
{
	if (pos.x < 0 || pos.x >= this->width) return; // we arent even on the map anymore!
	if (pos.y < 0 || pos.y >= this->height) return; // we arent even on the map anymore!

	if (this->tiles[pos.y*this->width + pos.x].regions[regionType] != 0) return; // we have already given this a region label!
	bool found = false;
	for (auto type : whitelist)
	{
		if (type == this->tiles[pos.y*this->width + pos.x].tileType)
		{
			found = true;
			break;
		}
	}

	if (!found) return; // this tile isnt even on the whitelist....get outta here

	this->tiles[pos.y*this->width + pos.x].regions[regionType] = label;  // setting this to the current region label

	depthfirstsearch(whitelist, pos + sf::Vector2i(-1, 0), label, regionType); // check top left
	depthfirstsearch(whitelist, pos + sf::Vector2i(0, 1), label, regionType); // check bottom left
	depthfirstsearch(whitelist, pos + sf::Vector2i(1, 0), label, regionType); // check bottom right
	depthfirstsearch(whitelist, pos + sf::Vector2i(0, -1), label, regionType); // check top right

	return;
}

void Map::findConnectedRegions(std::vector<TileType> whitelist, int regionType = 0)
{
	int regions = 1;

	for (auto& tile : this->tiles) tile.regions[regionType] = 0; // reset our regions

	for (int y = 0; y < this->height; ++y) // loop on y axis
	{
		for (int x = 0; x < this->width; ++x) // loop on x axis
		{
			bool found = false;
			for (auto type : whitelist) // loop our whitelist
			{
				if (type == this->tiles[y*this->width + x].tileType) // this is on the whitelist
				{
					found = true;
					break;
				}
			}

			if (this->tiles[y*this->width + x].regions[regionType] == 0 && found) // this is on the whitelist and we dont have a region type yet
			{
				depthfirstsearch(whitelist, sf::Vector2i(x, y), regions++, regionType); // map out this region
			}
		}
	}
	this->numRegions[regionType] = regions; // set number of regions for that type
}

void Map::clearSelected()
{
	for (auto& tile : this->selected) tile = 0; // set each selected tile to not selected

	this->numSelected = 0; // clear num selected

	return;
}

void Map::select(sf::Vector2i start, sf::Vector2i end, std::vector<TileType> blacklist)
{
	// swap coordinates if necessary
	if (end.y < start.y) 
		std::swap(start.y, end.y); // we selected from bottom to top
	if (end.x < start.x) 
		std::swap(start.x, end.x); // we selected from right to left

	// clamp in range
	if (end.x >= this->width) end.x = this->width - 1; // we are off the map, pull back
	else if (end.x < 0) end.x = 0; // we are off the map on the left side...reset

	if (end.y >= this->height) end.y = this->height - 1; // we are off the bottom, reset to bottom
	else if (end.y < 0) end.y = 0; // we are off the top, reset to the top

	// these are the same for start, as for end
	if (start.x >= this->width) start.x = this->width - 1;
	else if (start.x < 0) start.x = 0;

	if (start.y >= this->height) start.y = this->height - 1;
	else if (start.y < 0) start.y = 0;

	// loop tiles in selection bounds
	for (int y = start.y; y <= end.y; ++y)
	{
		for (int x = start.x; x <= end.x; ++x)
		{
			// select tile
			this->selected[y*this->width + x] = 1;
			++this->numSelected;

			for (auto type : blacklist) // loop blacklist tiles
			{
				if (this->tiles[y*this->width + x].tileType == type) // we are a blacklisted tile...mark as invalid and decrement num selected
				{
					this->selected[y*this->width + x] = 2;
					--this->numSelected;
					break;
				}
			}
		}
	}

	return;
}

Map::~Map()
{
}
