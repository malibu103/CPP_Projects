#pragma once

#include <SFML/Graphics.hpp>

#include <string>
#include <map>

class TextureManager
{
private:

	// array of textures
	std::map<std::string, sf::Texture> textures;

public:

	// add a texture from a file
	void loadTexture(const std::string& name, const std::string &filename);

	// translate an id into a reference
	sf::Texture& getRef(const std::string& texture);

	TextureManager();  //ctor
	~TextureManager(); //dtor
};

