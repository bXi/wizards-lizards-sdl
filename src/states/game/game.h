#pragma once

#include <list>
#include <cstring>

#include "luminoveau.h"

#include "entities/spawner.h"

#include "entities/player.h"
#include "entities/lizard.h"
#include "entities/gate.h"
#include "entities/roomsensor.h"

#include "world/world.h"
#include "map/room.h"
#include "map/levelmanager.h"

#include <state/basestate.h>
//#include "entities/powerup.h"
inline auto doCirclesOverlap = [](float x1, float y1, float r1, float x2, float y2, float r2)
{
	return fabs((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) <= (r1 + r2) * (r1 + r2);
};

class GameState : public BaseState {
private:

	float accumulator = 0;
	float physTime = 1.f; // static_cast<float>(Settings::getMonitorRefreshRate());
	Uint32 velocityIterations = 6;
	Uint32 positionIterations = 2;

	//Music backgroundMusic;

	//Powerup *powerup = nullptr;

	Dungeon dungeon;

	bool startedGameplay = false;

	float speed = 10.0f;

	float screenWidth;
	float screenHeight;

	float screenRatio;
	
	std::vector<Spawner *> spawners;


    int widthMargin = 5, heightMargin = 5;
	Texture currentLevelTexture;

	std::vector<Room*> rooms;
	LerpAnimator* roomTitleLerp = nullptr;

	Spawner *spawner = nullptr;

	LevelManager lm;

	Room* currentRoom = nullptr;
	Room* lastRoom = nullptr;

	bool miniMapVisible = false;
    Texture minimapTexture;
    int miniMapSize = 201;

public:
	void load() override;
	void unload() override;
	void draw() override;

	void setupControls();

	void resetGame();

	void drawUI();

	void handleInput();
	void update();
	void drawEntities();

    Texture dungeonTileset;
    DungeonTileData tileData;

    rectf getRectangle(int _x, int _y)
	{
		const rectf rect = {static_cast<float>(_x * Configuration::tileWidth), static_cast<float>(_y * Configuration::tileHeight), static_cast<float>(Configuration::tileWidth), static_cast<float>(Configuration::tileHeight) };
		return rect;
	};

	rectf getTile(int tileId)
	{
		return getRectangle(tileId % 16, tileId / 16);
	};
};
