#pragma once

#include <vector>
#include "baseclass.h"


class WizardClass : public BaseClass
{

    

    struct lineSegment
    {
        float sx, sy;
        float ex, ey;
        float radius;
    };

    std::vector<lineSegment> vecLines;

    float secondsSinceLastShot = 0.0f;
    float laserLastDamageTick = 0.0f;

public:
    int getSpriteIndex() override
    {
        return 16;
    }

    void doDamage(flecs::entity* entity, int weaponId) override;
    void update() override;
    void shoot(flecs::entity entity) override;

};
