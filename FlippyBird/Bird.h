#pragma once
#include <SFML/Graphics.hpp>
#include "AnimationHandler.h"

class Bird
{
private:
	float velocity; // our current velocity (up or down)
	int minAngle; // max angle to look up
	int maxAngle; // max angle to look down
	float maxVelocity; // terminal velocity
	float angleVelFactor; // factor to apply to our velocity to increment angle
	int flyUpVelFactor; // this factor applies when we are flying up....as we want to increase our angle quicker!

	int angleOffset = 90; // this is so we can make 0 angle == bird looking up, 180 == bird looking down.  This makes life easier!

	AnimationHandler animHandler; // our animations

public:
	sf::Sprite sprite; // our sprite
	sf::IntRect birdDims; // height and width of bird
	sf::Vector2f curPosition; // our current position

	int variant; // which bird are we
	float gravity; // how quickly we go down

	float curAngle; // angle we are looking

	bool isAlive; // whether we are alive or not

	void flap(); // flap our wings - set velocity to go up

	Bird() {}
	
	Bird(sf::IntRect birdDims, sf::Vector2f pos, sf::Texture &texture, 
		const std::vector<Animation>& animations)
	{
		// set some vars that we assume
		this->isAlive = true;
		this->gravity = 1400;
		this->velocity = 0; // we arent moving
		this->maxVelocity = 1000;
		this->variant = 0;
		this->minAngle = 60;
		this->maxAngle = 180;
		this->angleVelFactor = 1;
		this->flyUpVelFactor = 2;
		this->curAngle = 0; // start at 90 degrees (remember we will use offset in our min / max comparisons)

		this->curPosition = pos;

		this->sprite.setOrigin(birdDims.width / 2, birdDims.height / 2); // set origin to middle of the bird
		this->sprite.setTexture(texture); // set our texture
		this->animHandler.frameSize = sf::IntRect(0, 0, birdDims.width, birdDims.height);
		for (auto animation : animations) // add our flappy animation
		{
			this->animHandler.addAnim(animation);
		}
		this->animHandler.update(0.0f);
	}

	void draw(sf::RenderWindow &window); // method to draw the bird

	void update(float frameSpeed); // update our bird

	~Bird();
};

