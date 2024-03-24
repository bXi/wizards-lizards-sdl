#pragma once

#include "ecs.h"
#include "rigidbody2d.h"

#include "entities/classes/baseclass.h"
#include "entities/classes/wizard.h"
#include "world/world.h"


enum class PlayerClassType
{
	WIZARD,
	BERSERKER,
	BOUNCER,
	TRAPPER

};


struct PlayerClass
{

	PlayerClassType classtype;


	BaseClass* selectedClass;

	void init()
	{

		switch (classtype)
		{
		case  PlayerClassType::WIZARD: selectedClass = new WizardClass(); break;
		case  PlayerClassType::BERSERKER: break;
		case  PlayerClassType::BOUNCER: break;
		case  PlayerClassType::TRAPPER: break;
		}
	}


	void doDamage(flecs::entity* entity, int weaponId)
	{
		if (selectedClass) selectedClass->doDamage(entity, weaponId);
	}

	int getSpriteIndex() const
	{
		if (selectedClass) return selectedClass->getSpriteIndex();
		return -1;
	}

	void shoot(flecs::entity entity)
	{
		if (selectedClass) selectedClass->shoot(entity);
	}

	void update()
	{
		if (selectedClass) selectedClass->update();
	}

	int getRenderFrame(flecs::entity* entity);


	float hitTimer = 0.0f;
	void playerHit(flecs::entity* entity)
	{

		World::startSlowMotion();
		hitTimer = 0.125f;

		auto rigidBody2d = entity->get<RigidBody2D>();

		float radius = 5.f;
		float forceMagnitude = 4500.f;

		vf2d pos = b2Body_GetPosition(rigidBody2d->RigidBodyId);

		const auto enemyFilter = ECS::getWorld().filter<EnemyEntity>();
		enemyFilter.each([&](flecs::entity enemy, EnemyEntity renderer) {
			auto* enemyRigidBody2d = enemy.get<RigidBody2D>();

			vf2d monsterPosition = b2Body_GetPosition(enemyRigidBody2d->RigidBodyId);  // Assuming the monster has a rigid body

			float distance = (pos - monsterPosition).mag();
			if (distance <= radius) {
				// Calculate the force vector pointing away from the player
				vf2d forceDirection = (monsterPosition - pos).norm();
				vf2d force = forceDirection * forceMagnitude;

				// Apply the force to the monster's body
                b2Body_ApplyForceToCenter(enemyRigidBody2d->RigidBodyId, force, true);
			}

		});


	}

};