#pragma once

#include <mutex>
#include <thread>

#include "configuration/configuration.h"
#include "texture/texturehandler.h"

#include "map/dungeon.h"
#include "map/room.h"

class LevelManager
{
private:
	Texture finalTexture;
	Texture dungeonTileset;

	Texture finalTextureImage;
public:



	LevelManager();

	Dungeon dungeon;
	
	Dungeon getDungeon();
	Dungeon getBossRoom();
	Dungeon getTestRoom();
	Texture drawLevel();
	void renderRow(int start, int rows, Dungeon dungeon, Texture finalTextureImage, Texture dungeonTileset);

	std::vector<Room*> rooms;

};


