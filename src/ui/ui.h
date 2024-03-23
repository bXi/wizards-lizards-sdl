#pragma once

#include "luminoveau.h"

#include "flecs.h"



class UI {


public:
	static void drawSmallBar(const vf2d &pos, const vi2d &offset, int width, int height, Color color, float percentage);
	static void drawTimerBar();
	static void DrawPlayerBar(flecs::entity entity);
    static void DrawMiniMap(Texture texture);
    static void drawDebugInfo();
};