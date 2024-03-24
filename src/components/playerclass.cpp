#include "playerclass.h"

#include "rigidbody2d.h"


int PlayerClass::getRenderFrame(flecs::entity* entity)
{


	auto* rigidBody = entity->get_mut<RigidBody2D>();

	int startFrame = 0;
	int frame = 0;
	constexpr int frameCount = 4;



	startFrame = getSpriteIndex(); //idle


	frame = (static_cast<int>(Window::GetRunTime() * 10.0f)) % frameCount;

	if (vf2d(b2Body_GetLinearVelocity(rigidBody->RigidBodyId)).mag2() > 0) {
		startFrame += 4;
	}

	return startFrame + frame;

}
