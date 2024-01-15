#include "player.h"

#include "gate.h"

void CreateGateEntity(vf2d pos, int roomId) {

	const auto& ecs = ECS::getWorld();

	Texture sprite = Textures::GetTexture("assets/tilesets/dungeongate.png");

	flecs::entity entity;
	// Create the player entity


	entity = ecs.entity()
        .set<DeleteBulletsOnHit>({})
        .set<GateEntity>({ roomId })
        .set<RenderFrame>({ 16 })
		.set<Render2DComp>({})
        .emplace<Collision>(CATEGORY_GATE, MASK_GATE)
        .emplace<Sprite>(32.f, 64.f, sprite, true, true, 32, 32, 16.f, 48.f, direction::EAST);

    vf2d Size = { 1,1 };

	b2Body* RigidBody = nullptr;


	auto userData = std::make_unique<UserData>();
	userData->entity_id = entity.id();

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(pos.x, pos.y);
    bodyDef.angle = 0;
    bodyDef.userData.pointer = 0;
	bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(userData.get());
	RigidBody = World::createBody(&bodyDef);

    b2PolygonShape Box;
    Box.SetAsBox(Size.x / 2, Size.y / 2);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &Box;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	RigidBody->CreateFixture(&fixtureDef);
	
	entity.emplace<RigidBody2D>(RigidBody);

	entity.set<UserDataComponent>({ std::move(userData) });

}
