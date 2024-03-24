#include "fireball.h"

void CreateFireballEntity(flecs::entity* player, vf2d pos, vf2d vel) {

	const auto& ecs = ECS::getWorld();

	Texture sprite = Textures::GetTexture("assets/players/wizard/fireball.png");

	flecs::entity entity;

	entity = ecs.entity()
		.set<FireballEntity>({ })
		.set<Sprite>({ 8.f, 8.f, sprite })
		.set<Render2DComp>({  });

	b2BodyId RigidBodyId;

	b2Circle CircleShape;
    CircleShape.radius = 0.125f;

	auto userData = std::make_unique<UserData>();
	userData->entity_id = entity.id();

	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = pos;
	bodyDef.userData = userData.get();

	RigidBodyId = World::createBody(&bodyDef);
    b2ShapeDef shapeDef = b2DefaultShapeDef();

    shapeDef.density = 1.0f;
	shapeDef.friction = 0.3f;

    b2CreateCircleShape(RigidBodyId, &shapeDef, &CircleShape);


	entity.emplace<RigidBody2D>(RigidBodyId);

	entity.set<UserDataComponent>({ std::move(userData) });

	entity.emplace<Collision>(CATEGORY_FIREBALL, MASK_FIREBALL);

	entity.get_mut<Collision>()->init(&entity);

	entity.emplace<Owner>(player->id());

	vf2d velocity = vel.norm() * 20;

    b2Body_SetLinearVelocity(entity.get_mut<RigidBody2D>()->RigidBodyId, velocity);
}
