#include <SFML/Graphics.hpp>
#include <vector>

#include "AnimationHandler.h"


void AnimationHandler::update(const float frameSpeed)
{
	if (currentAnim >= this->animations.size() || currentAnim < 0) return; // we are not animating atm

	float duration = this->animations[currentAnim].duration; // current duration

	// check if the animation has progressed to a new frame, if so change to the next frame
	if (int((t + frameSpeed) / duration) > int(t / duration))
	{
		// calculate the frame number
		int frame = int((t + frameSpeed) / duration);

		// adjust for looping
		frame %= this->animations[currentAnim].getLength();

		// set the sprite to the new frame
		sf::IntRect rect = this->frameSize;
		rect.left = rect.width * frame; // left position is width times the frame we are on (0-x)
		rect.top = rect.height * this->currentAnim; // top of rect is always going to be the height for this animation
		this->bounds = rect; // set our bounds
	}

	// increment the time elapsed
	this->t += frameSpeed;

	// adjust for looping
	if (this->t > duration * this->animations[currentAnim].getLength()) // time spent is greater than the frame duration times number of frames
	{
		this->t = 0.0f; // reset time spent on animation, aka loop
	}

	return;
}

void AnimationHandler::addAnim(Animation& anim)
{
	this->animations.push_back(anim); // add our new animation

	return;
}

void AnimationHandler::changeAnim(unsigned int animID)
{
	// do not change the animation if the animation is currently active or the new animation does not exist
	if (this->currentAnim == animID || animID >= this->animations.size() || animID < 0) return;

	// set the current animation
	this->currentAnim = animID;

	// update the animation bounds
	sf::IntRect rect = this->frameSize;
	rect.top = rect.height * animID;
	this->bounds = rect;
	this->t = 0.0f;

	return;
}


AnimationHandler::~AnimationHandler()
{
}
