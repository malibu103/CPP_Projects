#pragma once
#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Animation
{
public:
	unsigned int startFrame;
	unsigned int endFrame;

	float duration;

	Animation(unsigned int startFrame, unsigned int endFrame, float duration) // ctor
	{
		this->startFrame = startFrame;
		this->endFrame = endFrame;
		this->duration = duration;
	}

	unsigned int getLength() { return endFrame - startFrame + 1; } // get length...hrmm dunno yet cept to be like an array or something
};

class AnimationHandler
{
private:
	// array of animations
	std::vector<Animation> animations;

	// current time since the animation loop started
	float t;

	int currentAnim; // current animation number

public:

	// add a new animation
	void addAnim(Animation& anim);

	// update the current frame of animation
	void update(const float frameSpeed);

	// change the animation, also reset t
	void changeAnim(unsigned int animNum);

	// current section of the texture that should be displayed
	sf::IntRect bounds;

	// pixel dimensions of each individual frame
	sf::IntRect frameSize;

	AnimationHandler() // ctor
	{
		this->t = 0.0f; // set init time
		this->currentAnim = -1; // set no current animation, i am guessing here
	}

	AnimationHandler(const sf::IntRect& frameSize) // ctor overload passing in a frame size...
	{
		this->frameSize = frameSize; // set our frame size..

		this->t = 0.0f; // set init time
		this->currentAnim = -1; // set no current animation...
	}

	~AnimationHandler(); // dtor
};

