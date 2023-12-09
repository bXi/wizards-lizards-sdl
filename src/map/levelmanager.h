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
	Image dungeonTileset;

	Image finalTextureImage;
public:



	LevelManager();

	Dungeon dungeon;
	
	Dungeon getDungeon();
	Dungeon getBossRoom();
	Dungeon getTestRoom();
	Texture drawLevel();
	void renderRow(int start, int rows, Dungeon dungeon, Image finalTextureImage, Image dungeonTileset);

	std::vector<Room*> rooms;

};


