#include "stdafx.h"
#include <SFML/Audio.hpp>

#include <string>
#include <map>

#include "SoundManager.h"

void SoundManager::loadSound(const std::string& name, const std::string& filename)
{
	// load the sound into buffer
	sf::SoundBuffer *sb = new sf::SoundBuffer();
	sb->loadFromFile(filename); // load into a buffer

	// add the sound
	sf::Sound snd;
	snd.setBuffer(*sb);

	// add it to the list of sounds
	this->sounds[name] = snd;

	return;
}

sf::Sound& SoundManager::getRef(const std::string& sound) // this returns a sound based on the name provided
{
	return this->sounds.at(sound);
}

SoundManager::SoundManager()
{
}


SoundManager::~SoundManager()
{
}
