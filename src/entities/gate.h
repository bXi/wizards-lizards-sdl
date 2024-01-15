#pragma once

#include "ecs.h"
#include "world/world.h"

#include "components/collision.h"
#include "components/playerindex.h"
#include "components/rigidbody2d.h"
#include "components/playerclass.h"
#include "components/render2d.h"
#include "components/playerinput.h"
#include "components/deletebulletsonhit.h"

struct GateEntity {
    int roomId = -1;
    bool closed = false;
    float openTimer = 0.f;

    void close(flecs::entity entity) {
        entity.get_mut<GateEntity>()->closed = true;

        entity.get_mut<RenderFrame>()->RenderFrameNumber = 20;


        entity.get_mut<Collision>()->mask = 0x0000;


    }
    void open(flecs::entity entity) {
        entity.get_mut<GateEntity>()->closed = false;

        entity.get_mut<RenderFrame>()->RenderFrameNumber = 16;

        entity.get_mut<Collision>()->mask = MASK_GATE;
    }
};

void CreateGateEntity(vf2d pos, int roomId);

