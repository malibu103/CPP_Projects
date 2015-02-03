#pragma once
#include <vector>
#include <map>

#include "Map.h"

class City
{
private:

	float currentTime;
	float timePerDay;

	std::vector<int> shuffledTiles;

	// number of residents who are not in the residential zone.
	double populationPool;

	// number of residents who are not currently employed but can work
	double employmentPool;

	// proportion of citizens who can work
	float propCanWork;

	// proportion of residents who die / give birth each day.
	// estimate for date = 1 / (life expectancy * 360)
	// current world values are 0.000055 and 0.000023, respectively
	double birthRate;
	double deathRate;

	double distributePool(double& pool, Tile& tile, double rate);

public:

	Map map;

	double population;
	double employable;

	double residentialTax;
	double commercialTax;
	double industrialTax;

	// running total of city earnings this month
	double earnings;
	double funds;

	int day;

	City()
	{
		// init all the settings
		this->birthRate = 0.00055;
		this->deathRate = 0.00023;
		this->propCanWork = 0.50;
		this->populationPool = 0;
		this->population = populationPool;
		this->employmentPool = 0;
		this->employable = employmentPool;
		this->residentialTax = 0.05;
		this->commercialTax = 0.05;
		this->industrialTax = 0.05;
		this->earnings = 0;
		this->funds = 0;
		this->currentTime = 0.0;
		this->timePerDay = 1.0;
		this->day = 0;
	}

	City(std::string cityName, int tileSize, std::map<std::string, Tile>& tileAtlas) : City() // ctor...not sure about the last part
	{
		this->map.tileSize = tileSize;
		load(cityName, tileAtlas);
	}

	void load(std::string cityName, std::map<std::string, Tile>& tileAtlas); // function to load our city
	void save(std::string cityName); // function to save our city

	void update(float framespeed); // standard update function
	void bulldoze(const Tile& tile); // function to bulldoze a tile
	void shuffleTiles(); // this is for the use of update, when we update we are going to add homeless into homes...but we want to assign them to seemingly random homes...so we shuffle the tiles
	void tileChanged(); // updates direction of tiles when a tile is added

	double getHomeless() { return this->populationPool; }
	double getUnemployed() { return this->employmentPool; }

	~City();
};

