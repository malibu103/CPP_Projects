#include "stdafx.h"
#include "Bird.h"

void Bird::update(float frameSpeed) 
{
	// normally we would handle updates in the update function...but we need framespeed to update our animation, so lets just do it here before we draw
	this->animHandler.changeAnim(this->variant);
	this->animHandler.update(frameSpeed);
	this->sprite.setTextureRect(this->animHandler.bounds);

	// increase our velocity heading downward
	this->velocity += this->gravity * frameSpeed; // move us downward by the gravity

	if (this->velocity > this->maxVelocity) // we are going above our terminal velocity
		this->velocity = maxVelocity;

	// update our current position on the y axis
	this->curPosition.y += this->velocity * frameSpeed;

	// update our current rotation
	if (this->velocity < 0) // if we are flying upwards (getting closer to 0 y)....lets rotate faster!
		this->curAngle += this->velocity * frameSpeed * this->angleVelFactor * this->flyUpVelFactor;
	else
		this->curAngle += this->velocity * frameSpeed * this->angleVelFactor;

	// if past 360, we need to convert to 0-360
	if (this->curAngle >= 360)
		this->curAngle -= 360;

	// translate angles
	float angleTranslate = this->curAngle + angleOffset;
	
	// make translation a valid angle
	if (angleTranslate >= 360)
		angleTranslate -= 360;

	// min max checks
	if (angleTranslate < minAngle)
		this->curAngle = minAngle - angleOffset;
	else if (angleTranslate > maxAngle)
		this->curAngle = maxAngle - angleOffset;

	// if past 360, we need to convert to 0-360 (need to redo this due to our offsets)
	if (this->curAngle >= 360)
		this->curAngle -= 360;

	return;
}

void Bird::draw(sf::RenderWindow &window) // draw our bird
{
	window.draw(this->sprite); // draw our sprite

	return;
}

void Bird::flap()
{
	// reset our angle to flat
	//this->curAngle = 0;

	// set our velocity to go up!
	this->velocity = -400;
}

Bird::~Bird()
{
}
