#include "Engine.h"
#include "Player.h"
#include <iostream>
#include <cmath>
#include <sstream> 
#include <fstream>
#include <string>
#include <Windows.h>


using namespace std;
using namespace sf;

Engine::~Engine()
{
	
}

//loading map.level to sprite & run engine
Engine::Engine(RenderWindow &win, Staty::GameStatistics &gameStatistics )
:pointsString("Points: 0", font),
levelString("Level: 0", font),
livesString("Lives: 0", font),
tonextlevelString("To next level: 0", font),
powerString("Power", font),
keycodeString("", font),
pauseString("PAUSE", font),
endlifeString("Play again - press S", font),
gameover1String("GAME OVER", font),
pictureNextLevel("Level  -   0", font),
fullscreanString("F1 - Fullscreen/Window Mode   F5 - Sound ON / OFF", font),
newRecordString("New Record !",font),
newGameRecordString("New Game Record !", font),
newLife("+1", font),

barsBackground(Vector2f(210, 30)),
powerbar(Vector2f(200, 20)),
underGameOver(Vector2f(111, 111)),
underNewGameRecord(Vector2f(111, 111)),

how_to_next_level(21), //!
to_new_level(0),
powerCapacity(95.f),
state2(0),
speed_powerbar(2), //!
toCountdownPoints(0),
maxLevel(15),
tonextLevelpicture(0),
levelpoints(0),

isPause(false),
isEndLife(false),
flagtogameover(false),
flagtogameover2(false),
flagtonextlevel(false),
isNextLevel(false),
fullScrean(false),
flagtoanimationNewLife(false),
flagtosaverecords(true),
flagrecordpoints(true),
ifgamerecord(true),
flagtocompletelevel(true)

{
	if (!font.loadFromFile("Data/GoodDog.otf"))
	{
		MessageBox(NULL, "Font not found !", "ERROR", NULL);
		return;
	}

	//GUI
	powerbar.setPosition(Vector2f(100, 10));
	powerbar.setFillColor(Color::Red);
	barsBackground.setPosition(Vector2f(powerCapacity, 5));
	barsBackground.setFillColor(Color::Yellow);
	pauseString.setCharacterSize(174);
	pauseString.setPosition(Vector2f(1280/2 - pauseString.getLocalBounds().width/2,160));
	endlifeString.setCharacterSize(88);
	endlifeString.setPosition(Vector2f(1280 / 2 - endlifeString.getLocalBounds().width / 2, 160));
	gameover1String.setCharacterSize(100);
	gameover1String.setPosition(Vector2f(1280 / 2 - gameover1String.getLocalBounds().width / 2, 160));
	underGameOver.setPosition(Vector2f(endlifeString.getGlobalBounds().left - 30, endlifeString.getGlobalBounds().top - 30));
	underGameOver.setFillColor(Color::Black);
	underGameOver.setSize(Vector2f(endlifeString.getGlobalBounds().width + 60, endlifeString.getGlobalBounds().height + 60));
	newRecordString.setCharacterSize(174);
	newRecordString.setPosition(Vector2f(1280 / 2 - newRecordString.getLocalBounds().width / 2, 300));
	newGameRecordString.setCharacterSize(174);
	newGameRecordString.setFillColor(Color::Black);
	newGameRecordString.setPosition(Vector2f(1280 / 2 - newGameRecordString.getLocalBounds().width / 2, 300));
	underNewGameRecord.setPosition(Vector2f(newGameRecordString.getGlobalBounds().left - 30, newGameRecordString.getGlobalBounds().top - 30));
	underNewGameRecord.setFillColor(Color::Red);
	underNewGameRecord.setSize(Vector2f(newGameRecordString.getGlobalBounds().width + 60, newGameRecordString.getGlobalBounds().height + 60));
	newLife.setCharacterSize(200);
	newLife.setPosition(Vector2f(640, 320));
	

	int ifnrLevelgreaterthanmaps = gameStatistics.nrLevel;
	if (gameStatistics.nrLevel > maxLevel)
		ifnrLevelgreaterthanmaps = 15;

	std::string _str = std::to_string(ifnrLevelgreaterthanmaps);
	
	//create map texture 
	for  (int i = 1;  i < Level::COUNT; i++)
	{
		texture[i].loadFromFile("Data/tiles.png", IntRect((i - 1)*level.tile_width, 0, level.tile_width, level.tile_height));
	}

	//creates map sprites
	level.loadFromFile("Levels/map_" + _str + ".level");
	for (int i = 0; i < level.height; i++)
	{
		for (int j = 0; j < level.width; j++)
		{                                             
			if (level.loadLevel[i][j].type != Level::NONE)
			{
				sprite[i][j].setTexture(texture[level.loadLevel[i][j].type]);
				sprite[i][j].setPosition(j*level.tile_width,i*level.tile_height);
			}
		}
	}

	if (Shader::isAvailable())
	{
		string shaderPath = "Data/blur.frag";
		if (!blurEffect.loadFromFile(shaderPath, Shader::Fragment))
		{
			MessageBox(NULL, "Shader not found !", "ERROR", NULL);
			return;
		}
		blurEffect.setUniform("texture", Shader::CurrentTexture);
	}

	gameStatistics.audio2.loadMusic(false);
	gameStatistics.audio2.setVolume(false);
	gameStatistics.audio2.loadSounds();

	if (!gameStatistics.flagsoundingame)
	{
		gameStatistics.audio2.playMusic();
	}
	if (gameStatistics.flagsoundingame)
	{
		gameStatistics.audio2.setVolume(true);
	}

	//run engine
	runEngine(win, gameStatistics);
}




void Engine::runEngine(RenderWindow &window, Staty::GameStatistics &gameStatistics) 
{

	Food food1(1);
	Food food2(2);
	Food food3(3);
	Food food4(4);
	Food food5(5);

	tabFood[0] = Food(food1);
	tabFood[1] = Food(food2);
	tabFood[2] = Food(food3);
	tabFood[3] = Food(food4);
	tabFood[4] = Food(food5);
	
	GameState2 = NEXT;

	while (GameState2==NEXT)
	{
		Vector2f mousePos(Mouse::getPosition(window));

		processEvents(window,gameStatistics);

		//if pause or endlife -> not move hero
		if (isPause){}
		else if (isEndLife){}
		else if (isNextLevel){}
		else
		{
			check_collision();
			check_collision_withFood(gameStatistics);

			blurEffect.setUniform("blur_radius", 0.f);
			player.update(mousePos);
		}

		//time to powerbar
		if (isPause)
		{
			pauseElapsedTime = pauseClock.getElapsedTime();
		}

		update_GUI(gameStatistics);
		render(window,gameStatistics);

	}//!menu

}



void Engine::check_collision(){

	if (player.getStatus() == Player::STOP) return;

	FloatRect box1(player.getBoundingBox());

	//box as position a next move
	box1.left += (player.getSpeed().x) /2 ;
	box1.top += (player.getSpeed().y) /2 ;

	//if new move is possible
	//check lt
	int x = box1.left / level.tile_width;
	int y = box1.top / level.tile_height;
	if (level.loadLevel[y][x].isWall && box1.intersects(sprite[y][x].getGlobalBounds()))
	{
		player.stop();
	}

	//check rt
	x = (box1.left + level.tile_width) / level.tile_height;
	y = box1.top / level.tile_height;
	if (level.loadLevel[y][x].isWall && box1.intersects(sprite[y][x].getGlobalBounds()))
	{
		player.stop();
	}

	//ckeck lb
	x = box1.left / level.tile_width;
	y = (box1.top + level.tile_height) / level.tile_height;
	if (level.loadLevel[y][x].isWall && box1.intersects(sprite[y][x].getGlobalBounds()))
	{
		player.stop();
	}

	//check rb
	x = (box1.left + level.tile_width)/level.tile_width;
	y = (box1.top + level.tile_height) / level.tile_height;
	if (level.loadLevel[y][x].isWall && box1.intersects(sprite[y][x].getGlobalBounds()))
	{
		player.stop();
	}

}

void Engine::processEvents(RenderWindow &window, Staty::GameStatistics &gameStatistics)
{

	Event event;

	while (window.pollEvent(event))
	{
		//Esc
		if (event.type == Event::KeyReleased && event.key.code == Keyboard::Escape)
		{
			GameState2 = MENU_AGAIN;
		}
		//Pause
		if (event.type == Event::KeyPressed && event.key.code == Keyboard::Space)
		{			
			isPause = !isPause;
			if (isPause)
			{
				pauseClock.restart();

				gameStatistics.audio2.setVolume(true);
				
			}
			if (!isPause)
			{
				pauseElapsedTimeInAllLevel += pauseElapsedTime;

				gameStatistics.audio2.setVolume(false);
			
			}
		}

		//S - New Life
		if (isEndLife)
		{
			if (event.type == Event::KeyPressed && event.key.code == Keyboard::S)
			{
				if (gameStatistics.lives > 0)
				{
					GameState2 = LEVEL_AGAIN;
				}
			}

		}

		//END
		if (event.type == Event::KeyPressed && event.key.code == Keyboard::F4)
		{
			GameState2 = END;
		}

		//SOUND ON/OFF
		if (event.type == Event::KeyPressed && event.key.code == Keyboard::F5)
		{
			gameStatistics.flagsoundingame = !gameStatistics.flagsoundingame;
			if (gameStatistics.flagsoundingame)
			{
				gameStatistics.audio2.setVolume(true);
			}
			if (!gameStatistics.flagsoundingame)
			{
				gameStatistics.audio2.setVolume(false);
			}
		}

		//FULLSCREAN
		if (event.type == Event::KeyPressed && event.key.code == Keyboard::F1)
		{
			window.close();
			if (fullScrean)
			window.create(VideoMode(1280, 720), "Boston Explorer", Style::Fullscreen);
			if (!fullScrean)
			window.create(VideoMode(1280, 720), "Boston Explorer", Style::Close);

			window.setFramerateLimit(65);
			fullScrean = !fullScrean;
		}


		//hero go or stop
		if (event.type == Event::KeyPressed && event.key.code == Keyboard::W)
		{
			player.go();
		}
		else if (event.type == Event::KeyReleased)
		{
			if (event.key.code == Keyboard::W)
			{
				player.stop();
			}
		}

	}//window.pollEvent
}

void Engine::render(RenderWindow &window, Staty::GameStatistics &gameStatistics){

	window.clear(Color::Black);

	//render map
	for (int i = 0; i < level.height; i++)
	{
		for (int j = 0; j < level.width; j++)
		{
			if (level.loadLevel[i][j].type != Level::NONE)
			{
				window.draw(sprite[i][j], &blurEffect);
			}
		}
	}


	//if pause or endlife - stop powerbar
	if (isPause){}
	else if (isEndLife){}
	else if (isNextLevel){}
	else
	{
		powerbar.setScale(getPowerState(window, gameStatistics), 1.f);
	}

	window.draw(player);
	window.draw(barsBackground);
	window.draw(powerbar);
	window.draw(pointsString);
	window.draw(levelString);
	window.draw(livesString);
	window.draw(powerString);
	window.draw(tonextlevelString);
	window.draw(keycodeString);
	window.draw(fullscreanString);
	window.draw(tabFood[0].sprite);
	window.draw(tabFood[1].sprite);
	window.draw(tabFood[2].sprite);
	window.draw(tabFood[3].sprite);
	window.draw(tabFood[4].sprite);

	pointAnimation.update();
	pointAnimation.drawPoint(window);

	if (flagtoanimationNewLife)
	{
		float time = newLifeClock.getElapsedTime().asSeconds();
		float scaleFactor = 1.f;
		Vector2f pos;
		pos.x = 640 + time * 98.f;
		pos.y = 320 - time * 98.f * 0.7204;
		if (pos.x > 1130 || pos.y < 3) flagtoanimationNewLife = false;
		if (time > 2.5f) scaleFactor = (5 - time) / 2.5f;
		newLife.setPosition(pos);
		newLife.setFillColor(Color(0, 0, 255, static_cast<Uint8>(255.f * scaleFactor)));
		newLife.setScale(scaleFactor, scaleFactor);
		window.draw(newLife);
	}

	if (isPause)
	{
		window.draw(pauseString);
	}

	if (flagtonextlevel)
	{
		tonextLevelpicture++;

		if (flagtocompletelevel == true)
		{
			gameStatistics.audio2.stopMusic();
			gameStatistics.audio2.playSound(Audio::LEVEL_COMPLETED);
			flagtocompletelevel = false;
		}

		if (tonextLevelpicture < 150)
		{

			isNextLevel = true;
			ostringstream str;
			str << "Level completed  -  points " << levelpoints;
			pictureNextLevel.setString(str.str());
			pictureNextLevel.setCharacterSize(111);
			pictureNextLevel.setPosition(Vector2f(1280 / 2 - pictureNextLevel.getLocalBounds().width / 2, 160));
			underGameOver.setPosition(Vector2f(pictureNextLevel.getGlobalBounds().left - 60, pictureNextLevel.getGlobalBounds().top - 60));
			underGameOver.setFillColor(Color::Black);
			underGameOver.setSize(Vector2f(pictureNextLevel.getGlobalBounds().width + 120, pictureNextLevel.getGlobalBounds().height + 120));
			window.draw(underGameOver);
			window.draw(pictureNextLevel);

			if (flagrecordpoints == false)
			{
				gameStatistics.ifRecord[gameStatistics.nrLevel - 1] = true;
				window.draw(newRecordString);
			}

			if (levelpoints > gameStatistics.tabWithRecords[gameStatistics.nrLevel - 1].nr_points)
			{
				gameStatistics.tabWithRecords[gameStatistics.nrLevel - 1].nr_points = levelpoints;
				stringstream str;
				str << gameStatistics.nrLevel << "\t\t" << getDate() << "\t\t" << gameStatistics.tabWithRecords[gameStatistics.nrLevel - 1].nr_points << "\t";
				update_records_file("Data/records.txt", str.str(), gameStatistics.nrLevel + 1);
				flagrecordpoints = 0;
			}
		}//tnlp < 150

		if (tonextLevelpicture >= 150 && tonextLevelpicture <= 250)
		{
			window.clear();
			ostringstream str;
			str << "Level   -   " << gameStatistics.nrLevel + 1;
			pictureNextLevel.setString(str.str());
			pictureNextLevel.setCharacterSize(174);
			pictureNextLevel.setPosition(Vector2f(1280 / 2 - pictureNextLevel.getLocalBounds().width / 2, 160));
			window.draw(pictureNextLevel);
		}//tnlp 150-250

		if (tonextLevelpicture == 251)
		{
			GameState2 = NEXT_LEVEL;
			window.clear();
		}//tnlp ==251

	}//flagtonextlevel

	if (isEndLife)
	{
		if (gameStatistics.lives <= 0)
		{

			//countdown points afer gameover (4s)
			if (!flagtogameover2)
			{
				gameStatistics.audio2.stopMusic();
				gameStatistics.audio2.playSound(Audio::GAME_OVER);

				toCountdownPoints = gameStatistics.points;
				flagtogameover2 = true;
			}

			window.draw(gameover1String);

			if (flagtosaverecords)
			{
				if (gameStatistics.points > gameStatistics.tabWithRecords[0].firstplace)
				{
					gameStatistics.tabWithRecords[0].thirdplace = gameStatistics.tabWithRecords[0].secondplace;
					gameStatistics.tabWithRecords[0].secondplace = gameStatistics.tabWithRecords[0].firstplace;
					gameStatistics.tabWithRecords[0].firstplace = gameStatistics.points;
					gameStatistics.ifGameRecord[0] = true;
					ifgamerecord = true;
				}

				else if (gameStatistics.points > gameStatistics.tabWithRecords[0].secondplace &&
					gameStatistics.points <= gameStatistics.tabWithRecords[0].firstplace)
				{
					gameStatistics.tabWithRecords[0].thirdplace = gameStatistics.tabWithRecords[0].secondplace;
					gameStatistics.tabWithRecords[0].secondplace = gameStatistics.points;
					gameStatistics.ifGameRecord[1] = true;
					ifgamerecord = true;
				}

				else if (gameStatistics.points > gameStatistics.tabWithRecords[0].thirdplace &&
					gameStatistics.points <= gameStatistics.tabWithRecords[0].secondplace)
				{
					gameStatistics.tabWithRecords[0].thirdplace = gameStatistics.points;
					gameStatistics.ifGameRecord[2] = true;
					ifgamerecord = 1;
				}

				stringstream str;
				str << gameStatistics.tabWithRecords[0].firstplace << "\t\t" << gameStatistics.tabWithRecords[0].secondplace << "\t\t" << gameStatistics.tabWithRecords[0].thirdplace << "\t";
				update_records_file("Data/records.txt", str.str(), 32);

				flagtosaverecords = false;
			}

			if (ifgamerecord == true)
			{
				window.draw(underNewGameRecord);
				window.draw(newGameRecordString);
			}

			gameStatistics.points -= toCountdownPoints / 260;

			//reset gamestatics after gameover
			if (gameStatistics.points <= 0)
			{
				gameStatistics.points = 0;
				gameStatistics.lives = 3;
				gameStatistics.nrLevel = 1;
				gameStatistics.maxLevel = 15;
				gameStatistics.toNewLive = 50000;
				GameState2 = GAME_OVER;
			}

		}//gameStatistics.lives <= 0
		else if (!(gameStatistics.lives <= 0))
		{
			window.draw(underGameOver);
			window.draw(endlifeString);

			if (flagtogameover == true)
			{
				gameStatistics.audio2.stopMusic();
				gameStatistics.audio2.playSound(Audio::LIFE_OVER);
				gameStatistics.lives--;
				flagtogameover = false;
			}
		}
	}//isEndLife


	window.display();

}

float Engine::getPowerState(RenderWindow &window, Staty::GameStatistics &gameStatistics)
{
	float state = 0;
	Time timePast = clock.getElapsedTime();
	
	if (state > 1)
	{
		state = 1;
		state2 = timePast.asSeconds();
	}
	
	state = (powerCapacity - (timePast.asSeconds()*speed_powerbar - state2 - pauseElapsedTimeInAllLevel.asSeconds() * speed_powerbar)) / powerCapacity;

	/*if (state > 1)
	{
		state = 1;
		state2 = timePast.asSeconds();
	}*/

	if (state < 0)
	{
		isEndLife = true;
		clock.restart();
		flagtogameover = true;
	}

	return  max(state, 0.f);
}

string Engine::getDate()
{
	time_t rawtime;
	struct tm*timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "%d-%m-%Y/%I:%M:%S", timeinfo);
	string str(buffer);

	return str;
}

void Engine::update_records_file(const string path, string newLine, size_t werse)
{
	vector <string > vec;
	ifstream in(path.c_str());
	string tmp;

	while (getline(in, tmp)) vec.push_back(tmp);

	in.close();
	std::ofstream out(path.c_str());

	for (size_t i = 0; i < vec.size(); ++i)
	{
		if (i + 1 != werse) out << vec[i] << endl;
		else out << newLine << endl;
	}

	out.close();
}
