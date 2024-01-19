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
#include "components/renderframe.h"

struct GateEntity {
    int roomId = -1;
    bool closed = true;
    float openTimer = 0.f;

    void close(flecs::entity entity) {
        entity.get_mut<GateEntity>()->closed = true;

        entity.get_mut<RenderFrame>()->RenderFrameNumber = 20;

        auto fixtures = entity.get_mut<RigidBody2D>()->RigidBody->GetFixtureList();
        for (b2Fixture* fixture = fixtures; fixture; fixture = fixture->GetNext()) {
            fixture->SetSensor(false);
        }

        entity.get_mut<DeleteBulletsOnHit>()->disable = false;

    }
    void open(flecs::entity entity) {
        entity.get_mut<GateEntity>()->closed = false;

        entity.get_mut<RenderFrame>()->RenderFrameNumber = 16;

        auto fixtures = entity.get_mut<RigidBody2D>()->RigidBody->GetFixtureList();
        for (b2Fixture* fixture = fixtures; fixture; fixture = fixture->GetNext()) {
            fixture->SetSensor(true);
        }

        entity.get_mut<DeleteBulletsOnHit>()->disable = true;
    }
};

void CreateGateEntity(vf2d pos, int roomId);

