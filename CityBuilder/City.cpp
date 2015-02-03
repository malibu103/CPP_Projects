#include <cmath>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>

#include "City.h"
#include "Tile.h"

double City::distributePool(double& pool, Tile& tile, double rate = 0.0)
{
	const static int moveRate = 4; // how many we move a day?

	unsigned int maxPop = tile.maxPopPerLevel * (tile.tileVariant + 1); // set the max pop based on tile variant (residential level 2 will be double, etc)

	// if there is room in the zone, move up to 4 people from the pool into the zone
	if (pool > 0) // we have people to move
	{
		int moving = maxPop - (int)tile.population; // this will be how many in TOTAL we can move based on max pop
		if (moving > moveRate) moving = moveRate; // we can move more than 4...set to 4
		if (pool - moving < 0) moving = pool;  // pool is less than our moving var...set moving to be pool
		pool -= moving; // remove from our pool
		tile.population += moving; // add tile pop
	}

	// adjust the tile population for births and deaths
	tile.population += tile.population * rate;

	// move population that cannot be sustained by the tile into the pool
	if (tile.population > maxPop)
	{
		pool += tile.population - maxPop; // adding the excess back into the pool
		tile.population = maxPop; // set our tile pop to the max pop
	}

	return tile.population;
}

void City::bulldoze(const Tile& tile)
{
	// replace the selected tiles on the map with the tile and update populations etc accordingly
	for (int pos = 0; pos < this->map.width * this->map.height; ++pos) // loop EVERY tile
	{
		if (this->map.selected[pos] == 1) // we are at a selected tile
		{
			if (this->map.tiles[pos].tileType == TileType::RESIDENTIAL)
			{
				this->populationPool += this->map.tiles[pos].population; // redistribute residents to the homeless pop
			}
			else if (this->map.tiles[pos].tileType == TileType::COMMERCIAL)
			{
				this->employmentPool += this->map.tiles[pos].population; // redistribute workers to the unemployed pop
			}
			else if (this->map.tiles[pos].tileType == TileType::INDUSTRIAL)
			{
				this->employmentPool += this->map.tiles[pos].population; // lay off workers...same as above
			}
			this->map.tiles[pos] = tile; // set this tile to the desired tile
		}
	}

	return;
}

void City::shuffleTiles()
{
	while (this->shuffledTiles.size() < this->map.tiles.size()) // i guess we are just initializing the shuffled tiles with nothing so our vector sizes match?
	{
		this->shuffledTiles.push_back(0);
	}

	std::iota(shuffledTiles.begin(), shuffledTiles.end(), 0); // ok, this just iterates through the vector and changes the value sequentially, 0, 1, 2, etc...
	std::random_shuffle(shuffledTiles.begin(), shuffledTiles.end()); // shuffle our vector up

	return;
}

void City::tileChanged()
{
	this->map.updateDirection(TileType::ROAD); // we are only worried about updating roads...since these are the only tiles we can "place" that need turning
	this->map.findConnectedRegions(
	{
		TileType::ROAD, TileType::RESIDENTIAL,
		TileType::COMMERCIAL, TileType::INDUSTRIAL
	}, 0); // reset our regions...so if we place a new road connecting residential to commercial, etc...they will be on the same region so we can move goods / people

	return;
}

void City::load(std::string cityName, std::map<std::string, Tile>& tileAtlas) // load city from file
{
	int width = 0;
	int height = 0;

	std::ifstream inputFile(cityName + "_cfg.dat", std::ios::in); // open our input stream

	std::string line;

	while (std::getline(inputFile, line)) // read our file input stream into line
	{
		std::istringstream lineStream(line); // not sure here...looks like it converts the contents from a ?binary? stream string to a readable string?
		std::string key;
		if (std::getline(lineStream, key, '=')) // looks like this will get the string up until the "=" sign and fills it into the "key" string.  This is for our key=value pairs in the file im assuming
		{
			std::string value;
			if (std::getline(lineStream, value)) // reads the rest of the line to value
			{
				if (key == "width")					width = std::stoi(value); // set our width
				else if (key == "height")			height = std::stoi(value); // set our height
				else if (key == "day")				this->day = std::stoi(value); // set our current day
				else if (key == "populationPool")	this->populationPool = std::stod(value); // set our population pool
				else if (key == "employmentPool")	this->employmentPool = std::stod(value); // set our employment pool
				else if (key == "population")		this->population = std::stod(value); // set our population
				else if (key == "employable")		this->employable = std::stod(value); // set our unemployed peeps
				else if (key == "birthRate")		this->birthRate = std::stod(value); // set our birthrate
				else if (key == "deathRate")		this->deathRate = std::stod(value); // set our deathrate
				else if (key == "residentialTax")	this->residentialTax = std::stod(value); // set our residential tax rate
				else if (key == "commercialTax")	this->commercialTax = std::stod(value); // set our commercial tax rate
				else if (key == "industrialTax")	this->industrialTax = std::stod(value); // set our industrial tax rate
				else if (key == "funds")			this->funds = std::stod(value); // set our funds
				else if (key == "earnings")			this->earnings = std::stod(value); // set our earnings
			}
			else
			{
				std::cerr << "Error, no value for key " << key << std::endl; // we have a key without a value (width=), etc
			}
		}
	}

	inputFile.close(); // close our file, we are done here

	this->map.load(cityName + "_map.dat", width, height, tileAtlas); // load the map for our city
	tileChanged(); // make sure to update all of our connectors, etc

	return;
}

void City::save(std::string cityName)
{
	std::ofstream outputFile(cityName + "_cfg.dat", std::ios::out); // open our output file stream

	outputFile << "width="			<< this->map.width << std::endl; // save all of our data...same as load but in REVEEERRRRSSEE!!!!
	outputFile << "height="			<< this->map.height << std::endl;
	outputFile << "day="			<< this->day << std::endl;
	outputFile << "populationPool=" << this->populationPool << std::endl;
	outputFile << "employmentPool=" << this->employmentPool << std::endl;
	outputFile << "population="		<< this->population << std::endl;
	outputFile << "employable="		<< this->employable << std::endl;
	outputFile << "birthRate="		<< this->birthRate << std::endl;
	outputFile << "deathRate="		<< this->deathRate << std::endl;
	outputFile << "residentialTax=" << this->residentialTax << std::endl;
	outputFile << "commercialTax="	<< this->commercialTax << std::endl;
	outputFile << "industrialTax="	<< this->industrialTax << std::endl;
	outputFile << "funds="			<< this->funds << std::endl;
	outputFile << "earnings="		<< this->earnings << std::endl;

	outputFile.close(); // close our file

	this->map.save(cityName + "_map.dat"); // save our map

	return;
}

void City::update(float framespeed)
{
	// reset our values
	double popTotal = 0;
	double commercialRevenue = 0;
	double industrialRevenue = 0;

	// update the game time
	this->currentTime += framespeed;
	if (this->currentTime < this->timePerDay) return; // we havent hit a new day yet, get outta here man
	++day; // we are at the next day
	this->currentTime = 0.0; // reset time spent on this day
	
	if (day % 30 == 0) // we are at the end of the "month", lets add our funds
	{
		this->funds += this->earnings;
		this->earnings = 0; // reset monthly earnings...we are on new month
	}

	// run first pass of tile updates. mostly handles pool distribution
	for (int i = 0; i < this->map.tiles.size(); ++i)
	{
		Tile& tile = this->map.tiles[this->shuffledTiles[i]]; // looping through our shuffled tiles, not sure why when we could just loop through...doesnt matter though

		if (tile.tileType == TileType::RESIDENTIAL)
		{
			// redistribute the pool and increase the population total by the tile's population
			this->distributePool(this->populationPool, tile, this->birthRate - this->deathRate);

			popTotal += tile.population; // add to our total pop
		}
		else if (tile.tileType == TileType::COMMERCIAL)
		{
			// hire people
			if (rand() % 100 < 15 * (1.0 - this->commercialTax)) // hmmm -- more taxes == less chance to hire workers
				this->distributePool(this->employmentPool, tile, 0.00);
		}
		else if (tile.tileType == TileType::INDUSTRIAL)
		{
			// extract resources from the ground
			if (this->map.resources[i] > 0 && rand() % 100 < this->population) // we have resources on this tile...and increasing chance to get resources based on amount of workers
			{
				++tile.production; // we have produced goods
				--this->map.resources[i]; // remove resources from the ground
			}

			// hire people
			if (rand() % 100 < 15 * (1.0 - this->industrialTax)) // hire people increasingly based on how low the industrial tax is
				this->distributePool(this->employmentPool, tile, 0.0);
		}

		tile.update(); // update our tile
	}

	// run second pass. mostly handles goods manufacture
	for (int i = 0; i < this->map.tiles.size(); ++i)
	{
		Tile& tile = this->map.tiles[this->shuffledTiles[i]];

		if (tile.tileType == TileType::INDUSTRIAL)
		{
			int receivedResources = 0;

			// receive resources from smaller and connected zones
			for (auto& tile2 : this->map.tiles)
			{
				if (tile2.regions[0] == tile.regions[0] && tile2.tileType == TileType::INDUSTRIAL) // tiles are in the same region and both industrial
				{
					if (tile2.production > 0) // we are producing goods at tile 2
					{
						++receivedResources; // taking resources from tile 2
						--tile2.production;
					}
					if (receivedResources >= tile.tileVariant + 1) break; // we are getting enough resources to boost us up...stop getting resources
				}
			}

			// turn resources into goods
			tile.storedGoods += (receivedResources + tile.production) * (tile.tileVariant + 1); // we produced goods from the stuff we stole and our production times how big we are
		}
	}

	// run third pass. mostly handles goods distribution
	for (int i = 0; i < this->map.tiles.size(); ++i)
	{
		Tile& tile = this->map.tiles[this->shuffledTiles[i]];

		if (tile.tileType == TileType::COMMERCIAL)
		{
			int receivedGoods = 0;
			double maxCustomers = 0.0;

			for (auto& tile2 : this->map.tiles) // we are going to look for goods to bring in and to sell
			{
				if (tile2.regions[0] == tile.regions[0] &&
					tile2.tileType == TileType::INDUSTRIAL &&
					tile2.storedGoods > 0) // this is an industrial tile in our region with goods! joy!
				{
					while (tile2.storedGoods > 0 && receivedGoods != tile.tileVariant + 1) // get as many of these goodies as we can!
					{
						--tile2.storedGoods; // remove goods from industrial tile
						++receivedGoods; // add them to our store
						industrialRevenue += 100 * (1.0 - industrialTax); // pay the industry for these goods
					}
				}
				else if (tile2.regions[0] == tile.regions[0] &&
					tile2.tileType == TileType::RESIDENTIAL) // this is a connected residential tile
				{
					maxCustomers += tile2.population; // they are now potential customers
				}
				if (receivedGoods == tile.tileVariant + 1) break; // we have enough goods to fill us up
			}

			// calculate the overall revenue for the tile
			tile.production = (receivedGoods * 100.0 + rand() % 20) * (1.0 - this->commercialTax); // production is based on how many goods we received + a randomness times the commercial tax.  This will make us more productive if taxes are lower

			double revenue = tile.production * maxCustomers * tile.population / 100.0; // how much we produce this turn * how many customers are connected to us (potential to buy) * how many sellers we have...
			commercialRevenue += revenue; // add this to our total revenue
		}
	}

	// adjust population pool for births and deaths
	this->populationPool += this->populationPool * (this->birthRate - this->deathRate);
	popTotal += this->populationPool; // add the population pool to our total population

	// adjust the employment pool for the changing population
	float newWorkers = (popTotal - this->population) * this->propCanWork; // total population minus homeless population * percentage that can work
	newWorkers *= newWorkers < 0 ? -1 : 1; // if we have a negative amount of new workers, make it a positive
	this->employmentPool += newWorkers;
	this->employable += newWorkers;
	if (this->employmentPool < 0) this->employmentPool = 0; // cant be negative
	if (this->employable < 0) this->employable = 0;

	// update the city population
	this->population = popTotal;

	// calculate city income from tax
	this->earnings = (this->population - this->populationPool) * 15 * this->residentialTax; // not sure why 15 times, maybe to make it quicker for demo sake.  Anyways, basic calc of taxes from non homeless people
	this->earnings += commercialRevenue * this->commercialTax; // get some tax money man
	this->earnings += industrialRevenue * this->industrialTax; // cha-ching

	return;
}

City::~City()
{
}
