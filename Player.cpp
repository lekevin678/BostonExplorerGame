#include "Player.h"
#include <Windows.h>
#include <math.h>
#include <iostream>

#define M_PI 3.14159265358979323846

using namespace std;
using namespace sf;

Player::Player()
:status(STOP),
frame(0),
speed(10)

{
	if (!texture.loadFromFile("Data/player-move.png"))
	{
		MessageBox(NULL, "Textures not found!", "ERROR", NULL);
		return;
	}

	sprite.setTexture(texture);
	sprite.setTextureRect(IntRect(0, 0, 64, 64));
	sprite.setOrigin(32, 32);
	sprite.setPosition(1280 / 2, 720 / 2);

	anim_clock.restart();
}

Player::~Player()
{
}

void Player::draw(RenderTarget &target, RenderStates states) const
{
	states.transform *= getTransform();;

	target.draw(sprite);
}

void Player::update(Vector2f mousePos)
{
	//hero always directed towards the mouse position
	Vector2f norm = mousePos - sprite.getPosition();
	float rot = atan2(norm.y, norm.x);
	rot = rot * 180.f / M_PI;

	rot += 90;
	sprite.setRotation(rot);

	//hero animation
	if (anim_clock.getElapsedTime() > seconds(0.02f))
	{
		if (status == STOP) return;

#if FRAME_ANIMATION == 0
		//move with frames
				if (frame < 7)
				{
					frame++;
				}
				else
				{
					frame = 0;
				}
#endif

		sprite.setTextureRect(IntRect(frame * 64, 0, 64, 64));
		sprite.move(getSpeed());

		anim_clock.restart();
	}
}

//stop flag -- > animation in update - return 
void Player::stop()
{
	status = STOP;
	frame = 0;
}

void Player::go()
{
	status = GO;
}

Vector2f Player::getSpeed(){

	float rotacja = sprite.getRotation();
	float vx = sin((rotacja * M_PI)/180.0f);
	float vy = -cos((rotacja*M_PI/180.0f));

	return Vector2f(vx*speed, vy*speed);
}

void Engine::check_collision_withFood(Staty::GameStatistics &gameStatistics){

	for (int i = 0; i < howmanyFood; i++)
	{
		FloatRect getBoundingBoxFood(tabFood[i].sprite.getGlobalBounds());
		tabFood[i].setFoodTime();
		
		 if (getBoundingBoxFood.intersects(player.getBoundingBox()))
		{
			gameStatistics.audio2.playSound(Audio::GAIN);

			state2 += (tabFood[i].whichfood );
			tabFood[i].pointsPerFood -= tabFood[i].foodTime.asSeconds() * (i + 1) * 5;
				
			if (tabFood[i].pointsPerFood <= tabFood[i].whichfood * 100 / 2)
			{
				tabFood[i].pointsPerFood = tabFood[i].whichfood * 100 / 2;
			}
		
			ostringstream str;
			str << tabFood[i].pointsPerFood;
			string _str = str.str();

			pointAnimation.add(_str, tabFood[i].pos);

			gameStatistics.points += tabFood[i].pointsPerFood;
			levelpoints += tabFood[i].pointsPerFood;
			
			if (gameStatistics.points > gameStatistics.toNewLive)
			{
				flagtoanimationNewLife = true;
				newLifeClock.restart();
				cout << flagtoanimationNewLife << endl;
				gameStatistics.lives++;
				gameStatistics.toNewLive += 50000;
			}

			if (tabFood[i].whichfood == 5)
			{
				speed_powerbar += 0.08;
				to_new_level++;
				player.setspeed(player.getspeed() + 0.5);

				if (to_new_level == how_to_next_level)
				{
					flagtonextlevel = true;
					to_new_level = 0;
					player.setspeed(20);
				}
			}

			tabFood[i].changeFood(howmanyFood);

		}//if intersects
	}//for

}

void Engine::update_GUI(Staty::GameStatistics &gameStatistics){

	ostringstream str; 
	str << "Points : " << gameStatistics.points;
	pointsString.setString(str.str());
	pointsString.setCharacterSize(31);
	pointsString.setPosition( 640 - pointsString.getLocalBounds().width / 2 , 680);

	str.str("");
	str << "Lives :  " << gameStatistics.lives;
	livesString.setString(str.str());
	livesString.setCharacterSize(25);
	livesString.setPosition(1100 - livesString.getLocalBounds().width / 2, 3);

	str.str("");
	str << "Level :  " << gameStatistics.nrLevel;
	levelString.setString(str.str());
	levelString.setCharacterSize(25);
	levelString.setPosition(1200 - levelString.getLocalBounds().width / 2, 3);

	str.str("");
	str << "To next level :  " <<(how_to_next_level - to_new_level);
	tonextlevelString.setString(str.str());
	tonextlevelString.setStyle(Text::Bold);
	tonextlevelString.setCharacterSize(31);
	tonextlevelString.setPosition(650 - tonextlevelString.getLocalBounds().width / 2, 0);

	str.str("");
	str << "Power: ";
	powerString.setString(str.str());
	powerString.setCharacterSize(31);
	powerString.setPosition(50 - powerString.getLocalBounds().width / 2, 0);

	str.str("Esc - Menu		Space - Pause		F4 - End");
	keycodeString.setString(str.str());
	keycodeString.setCharacterSize(24);
	keycodeString.setPosition(950 - powerString.getLocalBounds().width / 2,680);

	str.str("F1  -  Fullscrean / Window Mode	  F5 - Sound ON / OFF");
	fullscreanString.setString(str.str());
	fullscreanString.setCharacterSize(23);
	fullscreanString.setPosition(215 - fullscreanString.getLocalBounds().width / 2, 683);


}
