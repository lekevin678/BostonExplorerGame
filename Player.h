#pragma once
#include <SFML\Graphics.hpp>
#include <vector>

#define FRAME_ANIMATION 0 // 0 - hero animation 

using namespace std;
using namespace sf;

class Player: public Drawable,Transformable
{

public:

	Player();
	~Player();
	Player(float n_speed);

	enum Status{ GO, STOP};

	FloatRect getBoundingBox()		{ FloatRect f(sprite.getGlobalBounds()); return f; };
	Vector2f getSpeed();

	void update(Vector2f mousePos);

	void stop();
	void go();

	Vector2f getPosition()		{ return sprite.getPosition(); };
	Status getStatus()		{ return status; };

	void setspeed(float n_speed)		{ speed = n_speed; };
	float getspeed()		{ return speed; };

private:

	Sprite sprite;
	Texture texture;

	Status status;

	Clock anim_clock;

	size_t frame;
	float speed;

	virtual void draw(RenderTarget &target, RenderStates states) const;
};
