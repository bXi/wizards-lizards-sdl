#include "player.h"

#include "roomsensor.h"

void CreateRoomSensorEntity(vf2d pos, int roomId) {

	const auto& ecs = ECS::getWorld();

	Texture sprite = Textures::GetTexture("assets/tilesets/dungeongate.png");

	flecs::entity entity;
	// Create the player entity


	entity = ecs.entity()
        .set<RoomSensorEntity>({ roomId })
//        .emplace<DebugRigidBody2D>(1)
        .emplace<Collision>(CATEGORY_FIREBALL, MASK_FIREBALL)
        .emplace<Sprite>(32.f, 64.f, sprite, true, true, 32, 32, 16.f, 48.f, direction::EAST);

    vf2d outerTriggerSize = { 16, 16 };
    vf2d innerTriggerSize = { 36, 36 };

//	b2Body* RigidBody = nullptr;
//
//	auto userData = std::make_unique<UserData>();
//	userData->entity_id = entity.id();
//
//    b2BodyDef bodyDef;
//    bodyDef.type = b2_staticBody;
//    bodyDef.position.Set(pos.x, pos.y);
//    bodyDef.angle = 0;
//    bodyDef.userData.pointer = 0;
//	bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(userData.get());
//	RigidBody = World::createBody(&bodyDef);
//
//    float roomOffset = 9.f;
//
//    b2PolygonShape TopLeftBox;
//    TopLeftBox.SetAsBox(outerTriggerSize.x / 2, outerTriggerSize.y / 2, {roomOffset, roomOffset}, 0);
//	b2FixtureDef fixtureTopLeftBoxDef;
//    fixtureTopLeftBoxDef.isSensor = true;
//	fixtureTopLeftBoxDef.shape = &TopLeftBox;
//	RigidBody->CreateFixture(&fixtureTopLeftBoxDef);
//
//    b2PolygonShape TopRightBox;
//    TopRightBox.SetAsBox(outerTriggerSize.x / 2, outerTriggerSize.y / 2, {22 + roomOffset, roomOffset}, 0);
//	b2FixtureDef fixtureTopRightBoxDef;
//    fixtureTopRightBoxDef.isSensor = true;
//	fixtureTopRightBoxDef.shape = &TopRightBox;
//	RigidBody->CreateFixture(&fixtureTopRightBoxDef);
//
//    b2PolygonShape BottomLeftBox;
//    BottomLeftBox.SetAsBox(outerTriggerSize.x / 2, outerTriggerSize.y / 2, {roomOffset, 22 + roomOffset}, 0);
//	b2FixtureDef fixtureBottomLeftBoxDef;
//    fixtureBottomLeftBoxDef.isSensor = true;
//	fixtureBottomLeftBoxDef.shape = &BottomLeftBox;
//	RigidBody->CreateFixture(&fixtureBottomLeftBoxDef);
//
//    b2PolygonShape BottomRightBox;
//    BottomRightBox.SetAsBox(outerTriggerSize.x / 2, outerTriggerSize.y / 2, {22 + roomOffset, 22 + roomOffset}, 0);
//	b2FixtureDef fixtureBottomRightBoxDef;
//    fixtureBottomRightBoxDef.isSensor = true;
//	fixtureBottomRightBoxDef.shape = &BottomRightBox;
//	RigidBody->CreateFixture(&fixtureBottomRightBoxDef);
//
//    b2PolygonShape CenterBox;
//    CenterBox.SetAsBox(innerTriggerSize.x / 2, innerTriggerSize.y / 2, {11 + roomOffset, 11 + roomOffset}, 0);
//	b2FixtureDef fixtureCenterBoxDef;
//    fixtureCenterBoxDef.isSensor = true;
//	fixtureCenterBoxDef.shape = &CenterBox;
//	RigidBody->CreateFixture(&fixtureCenterBoxDef);
//
//
//	entity.emplace<RigidBody2D>(RigidBody);
//	entity.set<UserDataComponent>({ std::move(userData) });

//    entity.get_mut<DebugRigidBody2D>()->init();


}
