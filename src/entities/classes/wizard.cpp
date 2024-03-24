#include "wizard.h"

#include "components/health.h"
#include "entities/fireball.h"


void WizardClass::update()
{
	const float delta = (Window::GetFrameTime() / Configuration::slowMotionFactor);

	//spriteTimer += delta;
	secondsSinceLastShot -= delta;
	laserLastDamageTick -= delta;
	hitTimer -= delta;

}

void WizardClass::doDamage(flecs::entity* entity, int weaponId)
{
	
}


void WizardClass::shoot(flecs::entity entity)
{

	auto* player = entity.get_mut<PlayerInput>();
	auto* rigidBody2d = entity.get_mut<RigidBody2D>();


	auto dungeon = World::getDungeon();


	vf2d pos = b2Body_GetPosition(rigidBody2d->RigidBodyId);
	auto aim = player->aim;

	switch (player->selectedWeapon) {
	case 1:
	{
		if (secondsSinceLastShot <= 0.0f) {

			auto [spread, damage, speed] = player->getPowerUp(0);

			secondsSinceLastShot = 0.2f - (static_cast<float>(speed) / 200.0f);

			auto shootFireball = [&](const vf2d &_aim)
			{
				CreateFireballEntity(&entity, pos - vf2d(0.5f, 0.5f), (_aim / vi2d{ Configuration::tileWidth, Configuration::tileHeight }));
			};

			spread++;
			float degreesBetweenBullets = (PI / 180) * 3; // 7 degrees
			bool evenShots = !(spread % 2); //false
			int halfArc = spread / 2; //3

			vf2d aimright = aim;
			vf2d aimleft = aim;


			const auto halfArcOffset = static_cast<float>(halfArc + 1);

			if (evenShots) {
				aimleft.rotateBy(-((degreesBetweenBullets) * halfArcOffset) + (degreesBetweenBullets / 2.0f));
				aimright.rotateBy(((degreesBetweenBullets) * halfArcOffset) - (degreesBetweenBullets / 2.0f));
			}
			else {
				aimleft.rotateBy(-(degreesBetweenBullets) * halfArcOffset);
				aimright.rotateBy((degreesBetweenBullets) * halfArcOffset);
			}

			for (int shot = halfArc; shot > 0; shot--) {

				aimleft.rotateBy(degreesBetweenBullets);
				shootFireball(aimleft);

				aimright.rotateBy(-degreesBetweenBullets);
				shootFireball(aimright);

			}

			if (!evenShots) {
				shootFireball(aim);
			}

			player->shooting = false;
		}
	}
	break;
	case 2:
	{
		auto [damage, bounces, beamwidth] = player->getPowerUp(1);

		vf2d startPosition = {
			pos.x - 0.5f,
			pos.y - 0.5f
		};

		vf2d rayDir = aim.norm();
		vf2d stepsize = { sqrt(1 + (rayDir.y / rayDir.x) * (rayDir.y / rayDir.x)),
						 sqrt(1 + (rayDir.x / rayDir.y) * (rayDir.x / rayDir.y)) };
		vi2d mapCheck = startPosition;
		vf2d rayLength1D;
		vi2d step;

		if (rayDir.x < 0) {
			step.x = -1;
			rayLength1D.x = (startPosition.x - static_cast<float>(mapCheck.x)) * stepsize.x;
		}
		else {
			step.x = 1;
			rayLength1D.x = (static_cast<float>(mapCheck.x + 1) - startPosition.x) * stepsize.x;
		}

		if (rayDir.y < 0) {
			step.y = -1;
			rayLength1D.y = (startPosition.y - static_cast<float>(mapCheck.y)) * stepsize.y;
		}
		else {
			step.y = 1;
			rayLength1D.y = (static_cast<float>(mapCheck.y + 1) - startPosition.y) * stepsize.y;
		}

		bool lastHitX = false;

		bool foundTile = false;
		float maxDistance = 250.0f;
		float distance = 0.0f;
		while (!foundTile && distance < maxDistance) {
			if (rayLength1D.x < rayLength1D.y) {
				mapCheck.x += step.x;
				distance = rayLength1D.x;
				rayLength1D.x += stepsize.x;
				lastHitX = true;
			}
			else {
				mapCheck.y += step.y;
				distance = rayLength1D.y;
				rayLength1D.y += stepsize.y;
				lastHitX = false;
			}


			if (mapCheck.x >= 0 && mapCheck.x < dungeon.getWidth() && mapCheck.y >= 0 && mapCheck.y < dungeon.getHeight()) {
				if (dungeon.getTile(mapCheck.x, mapCheck.y) == Tile::WALL ||
					dungeon.getTile(mapCheck.x, mapCheck.y) == Tile::VOID) {
					foundTile = true;
				}
			}
		}

		vecLines.clear();

		vf2d hitWallNormal = { 0.0f, 0.0f };

		if (foundTile) {
			vf2d vIntersection;
			vIntersection = startPosition + rayDir * distance;
			if (lastHitX) {
				if (vIntersection.x < pos.x)
					hitWallNormal.x = 1.0f;
				else
					hitWallNormal.x = -1.0f;
			}
			else {
				if (vIntersection.y < pos.y)
					hitWallNormal.y = 1.0f;
				else
					hitWallNormal.y = -1.0f;
			}

			auto radius = 1.0f * (4.0f * (1.0f + static_cast<float>(beamwidth))) / 32.0f;

			vecLines.push_back({ startPosition.x, startPosition.y, vIntersection.x, vIntersection.y, radius });
		}

		vf2d lastAim = aim.norm();

		if (!vecLines.empty() && bounces > 0) {
			for (int i = 0; i < bounces; i++) {
				auto& lastEdge = vecLines.back();
				startPosition = {
					lastEdge.ex,
					lastEdge.ey
				};

				vf2d newRayDir = lastAim.reflectOn(hitWallNormal);
				hitWallNormal = { 0.0f, 0.0f };

				rayDir = { newRayDir.x, newRayDir.y };
				lastAim = rayDir.norm();
				stepsize = {sqrt(1 + (rayDir.y / rayDir.x) * (rayDir.y / rayDir.x)),
				 			  sqrt(1 + (rayDir.x / rayDir.y) * (rayDir.x / rayDir.y)) };
				mapCheck = startPosition;
				
				if (rayDir.x < 0) {
					step.x = -1;
					rayLength1D.x = (startPosition.x - static_cast<float>(mapCheck.x)) * stepsize.x;
				}
				else {
					step.x = 1;
					rayLength1D.x = (static_cast<float>(mapCheck.x + 1) - startPosition.x) * stepsize.x;
				}

				if (rayDir.y < 0) {
					step.y = -1;
					rayLength1D.y = (startPosition.y - static_cast<float>(mapCheck.y)) * stepsize.y;
				}
				else {
					step.y = 1;
					rayLength1D.y = (static_cast<float>(mapCheck.y + 1) - startPosition.y) * stepsize.y;
				}

				foundTile = false;
				distance = 0.0f;
				while (!foundTile && distance < maxDistance) {
					if (rayLength1D.x < rayLength1D.y) {
						mapCheck.x += step.x;
						distance = rayLength1D.x;
						rayLength1D.x += stepsize.x;
						lastHitX = true;
					}
					else {
						mapCheck.y += step.y;
						distance = rayLength1D.y;
						rayLength1D.y += stepsize.y;
						lastHitX = false;
					}

					if (mapCheck.x >= 0 && mapCheck.x < dungeon.getWidth() && mapCheck.y >= 0 && mapCheck.y < dungeon.getHeight()) {
						if (dungeon.getTile(mapCheck.x, mapCheck.y) == Tile::WALL ||
							dungeon.getTile(mapCheck.x, mapCheck.y) == Tile::VOID) {
							foundTile = true;
						}
					}
				}

				
				if (foundTile) {
					vf2d vIntersection;
					vIntersection = startPosition + rayDir * distance;
					if (lastHitX) {
						if (vIntersection.x < pos.x)
							hitWallNormal.x = 1.0f;
						else
							hitWallNormal.x = -1.0f;
					}
					else {
						if (vIntersection.y < pos.y)
							hitWallNormal.y = 1.0f;
						else
							hitWallNormal.y = -1.0f;
					}

					auto radius = 1.0f * (4.0f * (1.0f + static_cast<float>(beamwidth))) / 32.0f;
					vecLines.push_back({ startPosition.x, startPosition.y, vIntersection.x, vIntersection.y, radius });
				}
			}

		}

		for (auto& edge : vecLines) {

			vf2d start = { edge.sx * static_cast<float>(Configuration::tileWidth), edge.sy * static_cast<float>(Configuration::tileHeight )};
			vf2d end = { edge.ex * static_cast<float>(Configuration::tileWidth), edge.ey * static_cast<float>(Configuration::tileHeight )};

			float startAngle = 360.0f - (vf2d(edge.sx, edge.sy).getAngle() - vf2d( edge.sx, edge.sy ).getAngle());
			float endAngle = 360.0f - (vf2d(edge.sx, edge.sy).getAngle() - vf2d( edge.ex, edge.ey ).getAngle());


			if (startAngle >= 0.0f && startAngle <= 180.0f) {
				startAngle += 180.0f;
				endAngle += 180.0f;
			}

			Color laserBeam = { 255,0,0,30 };

			float radius = edge.radius * 32.0f;

			Render2D::DrawArcFilled(start, radius, startAngle, endAngle, 32, laserBeam);
			Render2D::DrawArcFilled(end, radius, endAngle + 180.0f, startAngle + 180.0f, 32, laserBeam);
			Render2D::DrawThickLine(start, end, laserBeam, radius * 2.0f);

			radius /= 2.5f;

			Render2D::DrawArcFilled(start, radius, startAngle, endAngle, 32, laserBeam);
			Render2D::DrawArcFilled(end, radius, endAngle + 180.0f, startAngle + 180.0f, 32, laserBeam);
			Render2D::DrawThickLine(start, end, laserBeam, radius * 2.0f);

			radius /= 3.0f;

			Render2D::DrawArcFilled(start, radius, startAngle, endAngle, 32, laserBeam);
			Render2D::DrawArcFilled(end, radius, endAngle + 180.0f, startAngle + 180.0f, 32, laserBeam);
			Render2D::DrawThickLine(start, end, { 255,100,100,25 }, radius * 2.0f);
		}
		if (laserLastDamageTick <= 0.0f) {
			for (auto& edge : vecLines) {

				const auto enemyFilter = ECS::getWorld().filter<EnemyEntity>();
				enemyFilter.each([edge,damage](flecs::entity entity, EnemyEntity enemyEntity) {
					auto* rigidBody2d = entity.get<RigidBody2D>();
					auto* health = entity.get_mut<Health>();

					const float fLineX1 = edge.ex - edge.sx;
					const float fLineY1 = edge.ey - edge.sy;

					const float fLineX2 = b2Body_GetPosition(rigidBody2d->RigidBodyId).x - edge.sx;
					const float fLineY2 = b2Body_GetPosition(rigidBody2d->RigidBodyId).y - edge.sy;

					const float fEdgeLength = fLineX1 * fLineX1 + fLineY1 * fLineY1;

					const float t = std::max(0.0f, std::min(fEdgeLength, (fLineX1 * fLineX2 + fLineY1 * fLineY2))) / fEdgeLength;

					const float fClosestPointX = edge.sx + t * fLineX1;
					const float fClosestPointY = edge.sy + t * fLineY1;

					const float fDistance = sqrtf((b2Body_GetPosition(rigidBody2d->RigidBodyId).x - fClosestPointX) * (b2Body_GetPosition(rigidBody2d->RigidBodyId).x - fClosestPointX) + (b2Body_GetPosition(rigidBody2d->RigidBodyId).y - fClosestPointY) * (b2Body_GetPosition(rigidBody2d->RigidBodyId).y - fClosestPointY));

					if (fDistance <= (rigidBody2d->radius + edge.radius)) {
						health->currentHealth -= (2 * (damage + 1));

					}

				});

				laserLastDamageTick = 0.12f;
			}
		}

	}
	break;
	default:
		break;
	}
}
