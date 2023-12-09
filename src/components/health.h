#pragma once
#include "ui/ui.h"

#include "sprite.h"
#include "rigidbody2d.h"

struct Health
{
	float maxHealth = -1;
	int healthBarYOffset = 0;


	float currentHealth = maxHealth;


	void draw(flecs::entity entity)
	{

		auto rigidBody2d = entity.get<RigidBody2D>();
		auto sprite = entity.get<Sprite>();

		UI::drawSmallBar(rigidBody2d->RigidBody->GetPosition(), { static_cast<int>(-(sprite->width / 2) + 2), static_cast<int>(-(sprite->height + 4 - healthBarYOffset)) }, 28, 6, RED, currentHealth / maxHealth);


	}
};
