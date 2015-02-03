#pragma once
#include <SFML/Audio.hpp>

#include <string>
#include <map>

class SoundManager
{

private:
	// array of sounds
	std::map<std::string, sf::Sound> sounds;

public:
	// add a sound from a file
	void loadSound(const std::string& name, const std::string &filename);
	
	// translate an id into a reference
	sf::Sound& getRef(const std::string& sound);

	SoundManager();
	~SoundManager();
};

