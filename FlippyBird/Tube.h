#pragma once
#include <SFML/Graphics.hpp>

class Tube
{
private:

	float velocity; // horizontal velocity
	bool isTop; // is top
	int points; // how many points is this tube worth

public:
	sf::Sprite sprite; // our sprite
	sf::Vector2f curPosition; // our current position

	Tube(sf::Vector2f pos, sf::Texture &texture, float velocity, bool isTop, int points)
	{
		// local vars (sorta)
		this->isTop = isTop;
		this->velocity = velocity;
		this->points = points;

		// create our sprite
		this->sprite.setTexture(texture); // set our texture

		setPosition(pos);
	}

	void draw(sf::RenderWindow &window); // method to draw the bird

	void update(float frameSpeed); // update our bird

	void setPosition(sf::Vector2f pos); // set our position
	void setPosition(float x, float y); // set our position
	void setPoints(int points); // set how many points this is worth

	int score(); // score the points

	~Tube();
};

