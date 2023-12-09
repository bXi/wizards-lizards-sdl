#pragma once
#include "flecs.h"


class BaseClass
{
public:

    float hitTimer = 0.0f;

    virtual void update() = 0;

    virtual void doDamage(flecs::entity* entity, int weaponId) { }

    virtual int getSpriteIndex() = 0;
    virtual void shoot(flecs::entity entity) = 0;

};
