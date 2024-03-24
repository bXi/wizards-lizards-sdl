#pragma once

#include <vector>
#include <audio/audiohandler.h>


#include "map/levelmanager.h"
#include "utils/lerp.h"
#include "box2d/box2d.h"
#include "entities/physicsobject.h"
#include "map/room.h"
#include "flecs.h"
#include "components/type.h"
#include "components/deletebulletsonhit.h"
#include "map/dungeon.h"

class PhysicsWall : public PhysicsObject
{
public:

    vf2d Size = { 0,0 };
    vf2d HalfSize = { 0,0 };

    b2Polygon Box;

    PhysicsWall(const vf2d& pos, const vf2d& size);
    void draw() override;
};



class World {
public:

    static void init()
    {
        get()._init();
    }

    static void clear()
    {
        get()._clear();
    }

    static std::vector<Room*> getRooms()
    {
        return get().rooms;
    }

    static Color getBackgroundColor()
    {
        return get()._backgroundColor;
    }

    static b2BodyId createBody(const b2BodyDef* body)
    {
        return b2CreateBody(get().worldId, body);
    }

    static void addObject(PhysicsObject* object)
    {
        return get().objects.push_back(object);
    }

    static void removeObject(PhysicsObject* object)
    {
        get()._removeObject(object);
    }

    static void clearObjects()
    {
        get().objects.clear();

        // Create a list to store the bodies that need to be removed
//        std::vector<b2Body*> bodiesToRemove;
//
//        // Iterate over all the bodies in the world and add them to the removal list
//        b2Body* body = get().world.GetBodyList();
//        while (body != nullptr) {
//            bodiesToRemove.push_back(body);
//            body = body->GetNext();
//        }
//
//        // Remove the bodies from the world
//        for (b2Body* _body : bodiesToRemove) {
//            get().world.DestroyBody(_body);
//        }
    }

    static std::vector<vi2d> getSpawners()
    {
        return get().dungeon.getSpawners();
    }

    static vi2d getStart()
    {
        return { get().dungeon.start.x, get().dungeon.start.y };
    }

    static void doStep(float timeStep, int velocityIterations, int positionIterations)
    {
        b2World_Step(get().worldId, timeStep, 8);
    }

//    static b2World& getPhysicsWorld()
//    {
//        return get().world;
//    }


    static void draw()
    {
        std::vector<PhysicsObject*> tempObjects = get().objects;  // Create a temporary copy

//        std::sort(tempObjects.begin(), tempObjects.end(), [](PhysicsObject* a, PhysicsObject* b) {
//            return a->RigidBody->GetPosition().y < b->RigidBody->GetPosition().y;
//        });

        for (auto const object : tempObjects) {
            if (b2Body_IsValid(object->  RigidBodyId)) {
                object->draw();
            }
        }
    }

    static std::vector<PhysicsObject*> getObjectsOfType(ObjectType type) {
        std::vector<PhysicsObject*> filteredObjects;

        for (auto const object : get().objects) {
            if (object->type == type) {
                filteredObjects.push_back(object);
            }
        }

        return filteredObjects;
    };

    static LerpAnimator* getSlowMotionLerp()
    {
        return get().slowMotionLerp;
    }

    static float getSlowMotionTimer()
    {
        return get().slowMotionTimer;
    }

    static void setSlowMotionTimer(float newValue)
    {
        get().slowMotionTimer = newValue;
    }

    static bool getPlaySlowMotionExit()
    {
        return get().playSlowMotionExit;
    }

    static void setPlaySlowMotionExit(bool newState)
    {
        get().playSlowMotionExit = newState;
    }


    // Implement the collision listener functions

//    void BeginContact(b2Contact* contact) override;
//
//    void EndContact(b2Contact* contact) override
//    {
//        get().handleCollision(contact, false);
//    }

    static Dungeon getDungeon() {
        return get().dungeon;
    }

    static void startSlowMotion()
    {
        if (!get().slowMotionLerp->started) {
            //Audio::PlaySound("assets/sfx/slowmo-enter.wav");
            get().playSlowMotionExit = true;
            get().slowMotionTimer = 3.0f;
            get().slowMotionLerp->started = true;
            get().slowMotionLerp->time = 0;
        }
    }


    static void generateNewMap()
    {
        get()._generateNewMap();
    }

    std::vector<PhysicsObject*> objects;

private:
    void _init();
    void _clear();
    //void handleCollision(b2Contact* contact, bool beginCollision);

    std::vector<Room*> rooms;

    void _generateNewMap();

    void _removeObject(PhysicsObject* object);

    //std::vector<Entity*> nonPlayerCollideEntities;

    std::vector<rectf> mergeWalls(const std::vector<Tile>& map, int width, int height);

    Color _backgroundColor = {66, 57, 58, 255};

    LevelManager lm;

    Dungeon dungeon;

    LerpAnimator* slowMotionLerp = nullptr;
    float slowMotionTimer = 0.0f;
    bool playSlowMotionExit = false;

    b2Vec2 gravity = {0.0f, 0.0f};    // Y+ is down, so gravity is not negative
    b2WorldDef worldDef;
    b2WorldId worldId;

public:
    World(const World&) = delete;
    static World& get() { static World instance; return instance; }

private:
    World()
    {
        worldDef = b2DefaultWorldDef();
        worldDef.gravity = gravity;
        worldId = b2CreateWorld(&worldDef);

        //world.SetContactListener(this);

        slowMotionLerp = Lerp::getLerp("SlowMoLerp", 1.0f, 2.0f, 0.5f);
        slowMotionLerp->started = false;
    }
};


