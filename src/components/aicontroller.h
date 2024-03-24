#pragma once

struct AIController
{
	flecs::entity target;
	flecs::entity self;

	float maxSpeed = 4.0f;

	flecs::entity selectRandomTarget() {

		std::vector<flecs::entity> entities;

		const auto playerFilter = ECS::getWorld().filter<PlayerIndex>();
		playerFilter.each([&](flecs::entity entity, PlayerIndex index) {
			entities.push_back(entity);
		});

		std::random_device rd;
		std::mt19937 generator(rd());
		std::uniform_int_distribution<int> distribution(0, static_cast<int>(entities.size()) - 1);
		return entities[distribution(generator)];
	}

	AIController()
	{
		target = selectRandomTarget();
	}

	void applyForce(const vf2d& desiredVelocity)
	{
		const b2Vec2 currentVelocity = b2Body_GetLinearVelocity(self.get<RigidBody2D>()->RigidBodyId);
		const b2Vec2 steering = desiredVelocity - currentVelocity;
		const b2Vec2 force = b2Vec2(steering.x * maxSpeed, steering.y * maxSpeed);

        b2Body_ApplyForceToCenter(self.get<RigidBody2D>()->RigidBodyId, force, true);
	}

	void seek()
	{
        vf2d targetPos = b2Body_GetPosition(target.get<RigidBody2D>()->RigidBodyId);
        vf2d currentPos = b2Body_GetPosition(self.get<RigidBody2D>()->RigidBodyId);
		const vf2d desired = vf2d(targetPos - currentPos).norm() * maxSpeed / Configuration::slowMotionFactor;
		applyForce(desired);
	}


};