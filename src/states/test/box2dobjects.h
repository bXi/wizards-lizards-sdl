#pragma once

#include "box2d/box2d.h"
#include "utils/vectors.h"
#include "utils/colors.h"

#include "render2d/render2dhandler.h"
#include "entities/physicsobject.h"

//extern b2Vec2 Gravity;    // Y+ is down, so gravity is not negative
//extern b2World World;

class BoxObject : public PhysicsObject
{
public:

    vf2d Size = { 0,0 };
    vf2d HalfSize = { 0,0 };

//    b2PolygonShape Box;

    BoxObject(vf2d pos, vf2d size, Color c, float angle = 0, bool isDynamic = true);
    void draw() override;
};

class BallObject : public PhysicsObject
{
public:
    float Radius = 0;

//    b2CircleShape CircleShape;

    BallObject(vf2d pos, float radius, Color c);
    void draw() override;
};
