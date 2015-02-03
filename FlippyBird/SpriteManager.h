#pragma once
#include <SFML/Graphics.hpp>

#include <string>
#include <map>

class SpriteManager
{

private:
	
	// array of sprites
	std::map<std::string, sf::Sprite> sprites;

public:

	void loadSprite(std::string name, sf::Texture &txt, sf::IntRect rect); // add sprite

	// translate an id into a reference
	sf::Sprite& getRef(const std::string& name);

	SpriteManager(){}
	~SpriteManager();
};

