#include "world.h"

#include "ecs.h"
#include "components/health.h"

#include "components/owner.h"
#include "components/rigidbody2d.h"
#include "components/playerclass.h"

PhysicsWall::PhysicsWall(const vf2d& pos, const vf2d& size)
{
    type = ObjectType::WALL;

    ShapeColor = { 255, 0, 0, 72 };
    Size = size;
    HalfSize.x = size.x / 2;
    HalfSize.y = size.y / 2;

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(pos.x, pos.y);
    bodyDef.angle = 0.0f;
    bodyDef.userData.pointer = 0;
    RigidBody = World::createBody(&bodyDef);

    Box.SetAsBox(Size.x / 2, Size.y / 2);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &Box;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;

    RigidBody->CreateFixture(&fixtureDef);
}

void PhysicsWall::draw()
{
    return;

    auto origin = (vf2d)HalfSize * vi2d(Configuration::tileWidth, Configuration::tileHeight);
    Rectangle rec = { RigidBody->GetPosition().x * Configuration::tileWidth,RigidBody->GetPosition().y * Configuration::tileHeight, Size.x * Configuration::tileWidth, Size.y * Configuration::tileHeight };

    vf2d pos = vf2d{rec.x, rec.y} + origin;
    Render2D::DrawRectangle(pos, {rec.width, rec.height}, ShapeColor);
    
}

void World::_init()
{
    lm = LevelManager();

    dungeon = lm.getBossRoom();

    rooms = dungeon.getRooms();

    auto* leftPhysicsWall   = new PhysicsWall({ 1.0f,                             (float)dungeon.getHeight() / 2.0f }, { 2.0f                     , (float)dungeon.getHeight() });
    auto* topPhysicsWall    = new PhysicsWall({ (float)dungeon.getWidth() / 2.0f, 1.0f                              }, { (float)dungeon.getWidth(), 2.0f                       });
    auto* rightPhysicsWall  = new PhysicsWall({ (float)dungeon.getWidth() - 1.0f, (float)dungeon.getHeight() / 2.0f }, { 2.0f                     , (float)dungeon.getHeight() });
    auto* bottomPhysicsWall = new PhysicsWall({ (float)dungeon.getWidth() / 2.0f, (float)dungeon.getHeight() - 1.0f }, { (float)dungeon.getWidth(), 2.0f                       });

    World::addObject(leftPhysicsWall);
    World::addObject(topPhysicsWall);
    World::addObject(rightPhysicsWall);
    World::addObject(bottomPhysicsWall);

}

void World::_generateNewMap()
{
    lm = LevelManager();

    //UnloadTexture(currentLevelTexture);

    dungeon = lm.getDungeon();

    rooms = dungeon.getRooms();

    //currentLevelTexture = lm.drawLevel();



    auto walls = mergeWalls(dungeon.getTiles(), dungeon.getWidth(), dungeon.getHeight());
    
    for (auto& wall : walls) {
        PhysicsWall* tempWall = nullptr;

        tempWall = new PhysicsWall({ wall.x - (wall.width/2.f), wall.y - (wall.height /2.f) }, {wall.width, wall.height});

        World::addObject(tempWall);

    }
}

void World::handleCollision(b2Contact* contact, bool beginCollision)
{

}

//CREDIT: piratux on Javids Discord
std::vector<Rectangle> World::mergeWalls(const std::vector<Tile>& map, int width, int height) {

    std::vector<Rectangle> horizontalRectangles;
    std::vector<Rectangle> verticalRectangles;


    std::vector<Rectangle> mergedRectangles;


    // TODO: replace "asset_manager->map[index]" with get function
    auto create_tile_hitbox = [&](int32_t x_idx, int32_t y_idx, int32_t x_length, int32_t y_length, bool horizontal_hitbox)
    {
        Rectangle box = {
            static_cast<float>(x_idx),
            static_cast<float>(y_idx),
            static_cast<float>(x_length),
            static_cast<float>(y_length)
        };

        if (horizontal_hitbox)
            horizontalRectangles.push_back(box);
        else
            verticalRectangles.push_back(box);
    };

    // chain length of wall tiles next to each other counted in tiles
    int32_t chain_length;

    int32_t height_idx, width_idx;

    // horizontal iteration

    for (height_idx = 0; height_idx < height; height_idx++)
    {
        chain_length = 0;
        for (width_idx = 0; width_idx < width; width_idx++)
        {
            int32_t index = width * height_idx + width_idx;

            // 0 means empty tile
            if (map[index] == Tile::FLOOR)
            {
                if (chain_length > 0)
                {
                    create_tile_hitbox(width_idx, height_idx+1, chain_length, 1, true);
                    chain_length = 0;
                }
            }
            else
            {
                // if chain hasn't started
                if (chain_length == 0)
                {
                    // if hitbox length would be > 1
                    if (width_idx + 1 != width)
                    {
                        int32_t right_index = width * height_idx + width_idx + 1;
                        if (map[right_index] != Tile::FLOOR)
                        {
                            ++chain_length;
                            continue;
                        }
                    }

                    // if there is a wall tile on top or on bottom from this tile, don't
                    // create hitbox, because vertical iteration will create the hitbox
                    bool top_or_bottom_tile_exists = false;

                    if (height_idx != 0)
                    {
                        int32_t top_index = width * (height_idx - 1) + width_idx;
                        if (map[top_index] != Tile::FLOOR)
                            top_or_bottom_tile_exists = true;
                    }
                    if (height_idx + 1 != height)
                    {
                        int32_t bottom_index = width * (height_idx + 1) + width_idx;
                        if (map[bottom_index] != Tile::FLOOR)
                            top_or_bottom_tile_exists = true;
                    }

                    if (!top_or_bottom_tile_exists)
                        ++chain_length;
                }
                else
                {
                    ++chain_length;
                }
            }
        }

        if (chain_length > 0)
        {
            create_tile_hitbox(width_idx, height_idx + 1, chain_length, 1, true);
            chain_length = 0;
        }
    }

    // vertical iteration

    for (width_idx = 0; width_idx < width; width_idx++)
    {
        chain_length = 0;
        for (height_idx = 0; height_idx < height; height_idx++)
        {
            int32_t index = width * height_idx + width_idx;

            // 0 means empty tile
            if (map[index] == Tile::FLOOR)
            {
                if (chain_length > 0)
                {
                    create_tile_hitbox(width_idx + 1, height_idx, 1, chain_length, false);
                    chain_length = 0;
                }
            }
            else
            {
                // if chain hasn't started
                if (chain_length == 0)
                {
                    // if hitbox length would be > 1
                    if (height_idx + 1 != height)
                    {
                        int32_t bottom_index = width * (height_idx + 1) + width_idx;
                        if (map[bottom_index] != Tile::FLOOR)
                            ++chain_length;
                    }
                }
                else
                {
                    ++chain_length;
                }
            }
        }

        if (chain_length > 0)
        {
            create_tile_hitbox(width_idx + 1, height_idx, 1, chain_length, false);
            chain_length = 0;
        }
    }

    for (const auto& horizontalRect : horizontalRectangles) {
        if (horizontalRect.width > 2) mergedRectangles.push_back(horizontalRect);
    }

    for (const auto& verticalRect : verticalRectangles) {
        if (verticalRect.height > 2) mergedRectangles.push_back(verticalRect);
    }


    return mergedRectangles;

}


void World::_removeObject(PhysicsObject* object)
{

    auto it = std::find(objects.begin(), objects.end(), object);
    if (it != objects.end()) {
        b2Body* body = object->RigidBody;

        // Delete the fixtures attached to the body
        b2Fixture* fixture = body->GetFixtureList();
        while (fixture) {
            b2Fixture* nextFixture = fixture->GetNext();
            body->DestroyFixture(fixture);
            fixture = nextFixture;
        }

        body->GetWorld()->DestroyBody(body);

        objects.erase(it);
        delete object;
    }

}


void World::BeginContact(b2Contact* contact)
{
    // Get the fixtures involved in the contact
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    // Get the bodies associated with the fixtures
    b2Body* bodyA = fixtureA->GetBody();
    b2Body* bodyB = fixtureB->GetBody();

    // Get the user data of the bodies (assuming they store pointers)
    UserData* userDataA = reinterpret_cast<UserData*>(bodyA->GetUserData().pointer);
    UserData* userDataB = reinterpret_cast<UserData*>(bodyB->GetUserData().pointer);


    if (userDataA && !userDataB) {
        flecs::entity entityA = ECS::getWorld().entity(userDataA->entity_id);
        if (entityA.has<FireballEntity>() && bodyB->GetType() == b2_staticBody) {
            entityA.set<DeleteMe>({});
        }
    }
    else if (!userDataA && userDataB) {
        flecs::entity entityB = ECS::getWorld().entity(userDataB->entity_id);

        if (
            entityB.has<FireballEntity>() &&
            bodyA->GetType() == b2_staticBody)
        {
            entityB.set<DeleteMe>({});
        }
    }
    else if (userDataA && userDataB) {
        flecs::entity entityA = ECS::getWorld().entity(userDataA->entity_id);
        flecs::entity entityB = ECS::getWorld().entity(userDataB->entity_id);

        if (entityA.has<EnemyEntity>() && entityB.has<EnemyEntity>()) return;

        flecs::entity* player;
        flecs::entity* fireball;
        flecs::entity* enemy;

        bool doFireball = false;
        bool doPlayerHit = false;

        // Perform collision handling based on the entity types
        if (entityA.has<EnemyEntity>() && entityB.has<FireballEntity>()) {
            enemy = &entityA;
            fireball = &entityB;
            doFireball = true;
        }
        else if (entityA.has<FireballEntity>() && entityB.has<EnemyEntity>()) {
            fireball = &entityA;
            enemy = &entityB;
            doFireball = true;
        }
        else if (entityA.has<PlayerEntity>() && entityB.has<EnemyEntity>()) {
            player = &entityA;
            enemy = &entityB;
            doPlayerHit = true;
        }
        else if (entityA.has<EnemyEntity>() && entityB.has<PlayerEntity>()) {
            enemy = &entityA;
            player = &entityB;
            doPlayerHit = true;
        }


        if (doFireball && enemy) {
            flecs::entity owner = ECS::getWorld().entity(fireball->get<Owner>()->owner_id);

        	owner.get_mut<PlayerClass>()->doDamage(enemy, 1);

        	fireball->set<DeleteMe>({});

            enemy->get_mut<Health>()->currentHealth -= 10.f;
        }

        if (doPlayerHit && enemy) {
            player->get_mut<PlayerClass>()->playerHit(player);
        }
    } else
    {
	    
    }


}

void World::_clear() {

}
