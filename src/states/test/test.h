#pragma once


#include <vector>
#include <utility>

#include <utils/quadtree.h>
#include <utils/vectors.h>
#include <utils/lerp.h>
#include <utils/colors.h>
#include <utils/helpers.h>
#include <utils/camera.h>

#include "input/inputhandler.h"
#include "audio/audiohandler.h"
#include "fonts/fonthandler.h"
#include "render2d/render2dhandler.h"

#include "map/newdungeon.h"

#include "texture/texturehandler.h"

#include "box2d/box2d.h"
#include "box2dobjects.h"
#include "state/state.h"

#include <state/basestate.h>

enum class Test
{
	Quadtree,
	Lerp,
	Audio,
	Dungeon,
	LineIntersect,
	Box2D,
	Random
};

class Testpoint {
public:
	bool intersects = false;
	vf2d pos;

	void draw() {}

	void draw(int x, int y)
	{
		if (intersects)
			Render2D::DrawCircle({x + pos.x, y + pos.y}, 2.0f, GREEN);
		else
			Render2D::DrawCircle({x + pos.x, y + pos.y}, 2.0f, RED);

	}
};

class TestState : public BaseState {
	int maxWidth;

	std::map<Test, std::string> testList = {
		{Test::Quadtree, "Quadtree"},
		{Test::Lerp, "Lerp"},
		{Test::Audio, "Audio"},
		{Test::Dungeon, "Dungeon"},
		{Test::LineIntersect, "LineIntersect"},
		{Test::Box2D, "Box2D"},
		{Test::Random, "Random"},
	};

	std::unordered_map<int, Test> keys = {
		{SDLK_1, Test::Quadtree},
		{SDLK_2, Test::Lerp},
		{SDLK_3, Test::Audio},
		{SDLK_4, Test::Dungeon},
		{SDLK_5, Test::LineIntersect},
		{SDLK_6, Test::Box2D},
		{SDLK_7, Test::Random},
	};

	//quadtree
	std::vector<Testpoint*> testPoints;
	Testpoint* entity = nullptr;
	Test selected = Test::Dungeon;
	int size = 600;
	bool isCircle = false;

	//lerp
	LerpAnimator* testlerp = nullptr;

	//Dungeon
	DungeonGen* dg = nullptr;
	int wiggleCount = 0;
	bool showLines = true;

    //Box2D
	Rectangle scissorArea = { 0, 0, 300, 300 };
	std::vector<PhysicsObject*> Objects;

    vf2d groundSize = { 100, 2 };    // save a vector of our ground size, so we know what to draw
	float accumulator = 0;
	float physTime = 1 / 160.0f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;

    Texture dungeonTileset;

//	Camera cam = { {0.f, 0.f} };
	BoxObject ground = BoxObject({ 0, 0 }, { 200, 2 }, BROWN, 0, false);
    float testPercentage = 0.5;
public:
	void load() override;
	void unload() override;
	void draw() override;

	void customDrawGrid(int slices, float spacing);

};
