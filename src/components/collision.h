#pragma once
#include <cstdint>

#include "rigidbody2d.h"

const uint16_t CATEGORY_FIREBALL = 0x0002;
const uint16_t MASK_FIREBALL = 0x0001;

struct Collision
{

	uint16_t category;
	uint16_t mask;


	Collision(uint16_t _category, uint16_t _mask) : category(_category), mask(_mask)
	{
		
	}

	void init(flecs::entity* entity)
	{
//		auto rigidBody = entity->get_mut<RigidBody2D>()->RigidBodyId;


//		b2Filter filter;
//		filter.categoryBits = category;
//		filter.maskBits = mask;

		// Set the collision filtering for all fixtures
        //TODO: fix this
//		b2Fixture* fixture = b2Ge;
//		while (fixture != nullptr) {
//			fixture->SetFilterData(filter);
//			fixture = fixture->GetNext();
//		}
	}
	
};
