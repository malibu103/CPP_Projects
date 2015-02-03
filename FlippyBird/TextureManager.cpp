#include "stdafx.h"
#include <SFML/Graphics.hpp>

#include <string>
#include <map>

#include "TextureManager.h"

void TextureManager::loadTexture(const std::string& name, const std::string& filename)
{
	// load the texture
	sf::Texture tex;
	tex.loadFromFile(filename);

	// add it to the list of textures
	this->textures[name] = tex;

	return;
}

sf::Texture& TextureManager::getRef(const std::string& texture) // this returns a texture based on the name provided
{
	return this->textures.at(texture);
}

TextureManager::TextureManager()
{
}


TextureManager::~TextureManager()
{
}
