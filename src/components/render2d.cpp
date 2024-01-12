#include "render2d.h"

#include "rigidbody2d.h"
#include "sprite.h"
#include "playerclass.h"
#include "configuration/configuration.h"
#include "texture/texturehandler.h"
#include "utils/vectors.h"

#include "render2d/render2dhandler.h"

void Render2DComp::draw(flecs::entity* entity)
{

	if (entity->has<Sprite>()) {

		auto* sprite = entity->get<Sprite>();
		auto* rigidBody = entity->get_mut<RigidBody2D>();

		int renderFrame = 0;



		if (entity->has<PlayerClass>())
		{
			auto* playerClass = entity->get_mut<PlayerClass>();

			renderFrame = playerClass->getRenderFrame(entity);

		}

		/*if (hitTimer > 0.0f) {
			renderFrame = 24;
		}*/

		const vf2d pos = rigidBody->RigidBody->GetPosition();

		const Rectangle position = {
			pos.x * static_cast<float>(Configuration::tileWidth) - sprite->originX,
			pos.y * static_cast<float>(Configuration::tileHeight) - sprite->originY,
			sprite->width,
			sprite->height
		};

		Rectangle source;
		if (sprite->multiSheet) {
			source = Textures::GetTile(renderFrame, sprite->doubleHeight);
		}
		else
		{
			source = { 0.f, 0.f, sprite->width, sprite->height };
		}

		if (sprite->facing == direction::WEST) {
			source.width *= -1.0f;
		}

        Render2D::DrawTexturePart(sprite->sprite, {position.x, position.y}, {position.width, position.height},source);

		// debug for body center and sprite size
		//DrawRectangleRec(position, { 255,255,255,64 });
		//DrawCircleV(pos * 32, 2.f, { 255,0,0,128 });
		//DrawCircleV(pos * 32, rigidBody->radius * 32.f, { 255,0,0,64 });
	}
}
