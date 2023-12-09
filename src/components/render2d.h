#pragma once

#include "flecs.h"


struct Render2DComp
{

	int i;

	void draw(flecs::entity* entity);
};
