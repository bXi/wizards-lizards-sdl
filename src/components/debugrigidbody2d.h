#pragma once

#include "flecs.h"
#include "utils/colors.h"

struct DebugRigidBody2D
{

	int i;

    Color color = WHITE;

	void draw(flecs::entity entity);

    void init() {

        Color c (
        rand() % 255,
        rand() % 255,
        rand() % 255,
        100);

        color = c;

    }
};
