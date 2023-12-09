#pragma once

#include <texture/texturehandler.h>

#include "lizard.h"
#include "physicsobject.h"
#include "map/dungeon.h"
#include "utils/vectors.h"

#include "ui/ui.h"
#include "render2d/render2dhandler.h"

class Lizard;
class Player;


class Spawner {
private:
	Lizard *lizard = nullptr;

public:

	Room* room;
	
	float width = 16.0f;
	float height = 16.0f;

	Texture sprite;

	vf2d pos = { 0.0f , 0.0f };

	float spawnTimer;
	float timeTillNextSpawn = 2.0f;

	int maxSpawns = 100;
	int spawnsLeft = 100;

	void load() 
	{
		width = 32.0f;
		height = 32.0f;
		sprite = Textures::GetTexture("assets/entities/crate.png");

		maxSpawns = 100;
		spawnsLeft = 100;
		spawnTimer = 2.0f;
	}

	void draw() 
	{
        Render2D::DrawTexture(sprite, pos * width, {width, height});

		UI::drawSmallBar(pos, {1, 2}, 28, 6, BLUE, spawnTimer / timeTillNextSpawn);
		UI::drawSmallBar(pos, {1, 12}, 28, 6, GREEN, static_cast<float>(spawnsLeft) / static_cast<float>(maxSpawns));
	}


};