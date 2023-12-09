#pragma once

#include "texture/texturehandler.h"

enum class direction
{
	NORTH,
	EAST,
	SOUTH,
	WEST,
};

struct Sprite
{
	float width = 0.0f;
	float height = 0.0f;



	Texture sprite;

	bool multiSheet = false;

	bool doubleHeight = false;

	float originX = width / 2.f;
	float originY = height / 2.f;

	direction facing;

	
};