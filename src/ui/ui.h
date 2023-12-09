#pragma once

#include "utils/vectors.h"
#include "utils/camera.h"
#include <fonts/fonthandler.h>
#include "flecs.h"

#include "render2d/render2dhandler.h"

class UI {


public:
	static void drawSmallBar(const vf2d &pos, const vi2d &offset, int width, int height, Color color, float percentage);
	static void drawTimerBar();
	static void DrawPlayerBar(flecs::entity entity);
    static void DrawMiniMap(Texture texture);
    static void drawDebugInfo();
};