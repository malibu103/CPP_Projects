#include "stdafx.h"
#include "SpriteManager.h"

void SpriteManager::loadSprite(std::string name, sf::Texture &txt, sf::IntRect rect) // constructor to set sprite dims
{
	sf::Sprite sprite;

	sprite.setTexture(txt); // set our texture
	sprite.setTextureRect(rect); // set our rectangle

	sprites[name] = sprite; // set our sprite
}

sf::Sprite& SpriteManager::getRef(const std::string& name) // this returns a texture based on the name provided
{
	return this->sprites.at(name);
}

SpriteManager::~SpriteManager()
{
}
