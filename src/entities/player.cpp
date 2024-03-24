#include "player.h"



void CreatePlayerEntity(int index, vf2d pos) {

	const auto& ecs = ECS::getWorld();

	Texture sprite = Textures::GetTexture("assets/entities/entities.png");

	flecs::entity entity;
	// Create the player entity


	entity = ecs.entity()
		.set<PlayerEntity>({  })
		.set<PlayerIndex>({ index })
		.set<PlayerInput>({  })
		.set<PlayerClass>({ PlayerClassType::WIZARD })
		.set<Sprite>({ 32.f, 56.f, sprite, true, true, 32, 32, 16.f, 40.f, direction::WEST })
		.set<Render2DComp>({})
		.emplace<Collision>(CATEGORY_FIREBALL, MASK_FIREBALL);
		





	b2BodyId RigidBodyId;

	b2Circle CircleShape;
	CircleShape.radius = 0.5f;

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

	entity.get_mut<PlayerClass>()->init();
	entity.get_mut<Collision>()->init(&entity);

    entity.get_mut<PlayerInput>()->init(index);
	
}
