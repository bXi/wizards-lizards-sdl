#include "game.h"

#include <state/state.h>

void GameState::setupControls()
{

	const auto playerFilter = ECS::getWorld().filter<PlayerIndex>();
	playerFilter.each([&](flecs::entity entity, PlayerIndex index) {
		auto* input = entity.get_mut<PlayerInput>();

		input->controllers.clear();

		if (index.index == 1)
		{
			input->addController(Input::GetController(0));

			if (Input::GetAllInputs().size() > 1)
			{
				if (playerFilter.count() == 1)
					input->addController(Input::GetController(1));
			}
		}
	});
}

void GameState::resetGame()
{
	ECS::reset();

	dungeon = World::getDungeon();


	spawners.clear();
	for (unsigned int i = 0; i < World::getSpawners().size(); i++) {
		spawner = new Spawner();
		spawner->pos = World::getSpawners()[i];

		spawner->load();

		spawners.emplace_back(spawner);
		spawner = nullptr;
	}

	rooms = World::getRooms();
    tileData = World::getDungeon().getDungeonTileData();

    for (auto& room: rooms) {
        if (room->specialRoom) {

            vf2d testpos = (room->pos);// + room->size);// * vf2d(totalRoomSize, totalRoomSize);

            if (room->northExits > 0) {
                CreateGateEntity(testpos + vf2d(18.5f, 0.5f), room->id);
                CreateGateEntity(testpos + vf2d(19.5f, 0.5f), room->id);
                CreateGateEntity(testpos + vf2d(20.5f, 0.5f), room->id);
                CreateGateEntity(testpos + vf2d(21.5f, 0.5f), room->id);
            }

            if (room->southExits > 0) {
                CreateGateEntity(testpos + vf2d(18.5f, 39.5f), room->id);
                CreateGateEntity(testpos + vf2d(19.5f, 39.5f), room->id);
                CreateGateEntity(testpos + vf2d(20.5f, 39.5f), room->id);
                CreateGateEntity(testpos + vf2d(21.5f, 39.5f), room->id);
            }

            if (room->westExits > 0) {
                CreateGateEntity(testpos + vf2d(0.5f, 18.5f), room->id);
                CreateGateEntity(testpos + vf2d(0.5f, 19.5f), room->id);
                CreateGateEntity(testpos + vf2d(0.5f, 20.5f), room->id);
                CreateGateEntity(testpos + vf2d(0.5f, 21.5f), room->id);
            }

            if (room->eastExits > 0) {
                CreateGateEntity(testpos + vf2d(39.5f, 18.5f), room->id);
                CreateGateEntity(testpos + vf2d(39.5f, 19.5f), room->id);
                CreateGateEntity(testpos + vf2d(39.5f, 20.5f), room->id);
                CreateGateEntity(testpos + vf2d(39.5f, 21.5f), room->id);
            }

            CreateRoomSensorEntity(testpos, room->id);
        }
    }

	CreatePlayerEntity(1, World::getStart());

	setupControls();

	vf2d camPos = { 0.0f, 0.0f };

	const auto playerFilter = ECS::getWorld().filter<PlayerIndex>();
	playerFilter.each([&](flecs::entity entity, PlayerIndex index){
		auto* rigidBody = entity.get<RigidBody2D>()->RigidBody;

		//rigidBody->SetTransform(World::getStart(), rigidBody->GetAngle());
		camPos += rigidBody->GetPosition();
	});

	Camera::SetTarget({ (camPos.x / static_cast<float>(playerFilter.count())) * static_cast<float>(Configuration::tileWidth), (camPos.y / static_cast<float>(playerFilter.count())) * static_cast<float>(Configuration::tileHeight) });

	accumulator += Window::GetFrameTime();
	while (accumulator >= physTime)
	{
		accumulator -= physTime;
		World::doStep(physTime, velocityIterations, positionIterations);
	}
}

void GameState::load()
{
    physTime = 1.f / static_cast<float>(Settings::getMonitorRefreshRate());

    dungeonTileset = Textures::GetTexture("assets/tilesets/dungeon.png");

    Fonts::GetFont("assets/fonts/APL386.ttf", 20);

    Audio::GetMusic("assets/music/boss-1-loop.wav");
    Audio::GetSound("assets/sfx/slowmo-enter.wav");
    Audio::GetSound("assets/sfx/slowmo-exit.wav");

	screenWidth = static_cast<float>(Window::GetWidth());
	screenHeight = static_cast<float>(Window::GetHeight());
	screenRatio = screenWidth / screenHeight;

	World::generateNewMap();

	resetGame();

	roomTitleLerp = Lerp::getLerp("roomTitleLerp", 0.0f, 10.0f, 4.0f);

    minimapTexture = Textures::CreateEmptyTexture({(float)miniMapSize, (float)miniMapSize});

}

void GameState::unload()
{}

void GameState::draw()
{

    Window::ClearBackground(World::getBackgroundColor());

	auto findRoomContainingPoint = [&](const vf2d& target) -> Room*
	{
		for (const auto& room : rooms)
		{
			// Check if the target point falls within the bounds of the room
			if (target.x >= room->pos.x && target.x <= room->pos.x + room->size.x &&
				target.y >= room->pos.y && target.y <= room->pos.y + room->size.y)
			{
				return room; // Return the room pointer
			}
		}

		return nullptr; // Return nullptr if no room contains the point
	};

	vf2d camPos = { 0.0f, 0.0f };

    const auto playerFilter = ECS::getWorld().filter<PlayerIndex>();
	playerFilter.each([&](flecs::entity entity, PlayerIndex index) {
		auto* rigidBody2d = entity.get<RigidBody2D>();
		auto* input = entity.get_mut<PlayerInput>();
		auto* playerClass = entity.get_mut<PlayerClass>();

		input->doMovement(&entity);
		input->doShoot(entity);

		if (Input::KeyPressed(SDLK_F2)) input->weaponUpgrades[input->selectedWeapon - 1]++;
		if (Input::KeyPressed(SDLK_F3)) input->weaponUpgrades[input->selectedWeapon - 1] += 10;
		if (Input::KeyPressed(SDLK_F4)) input->weaponUpgrades[input->selectedWeapon - 1] += 100;


		playerClass->update();

		auto newSpeed = speed;
		if (input->isRunning) newSpeed *= 1.2f;

		newSpeed /= Configuration::slowMotionFactor;

		rigidBody2d->RigidBody->SetLinearVelocity(input->vel * newSpeed);

		camPos += rigidBody2d->RigidBody->GetPosition();
	});


    vf2d averagePlayerPos = { (camPos.x / static_cast<float>(playerFilter.count())), // * static_cast<float>(Configuration::tileWidth),
                              (camPos.y / static_cast<float>(playerFilter.count())) // * static_cast<float>(Configuration::tileHeight)
                       };


	currentRoom = findRoomContainingPoint(averagePlayerPos);

	if (currentRoom != lastRoom) {
		roomTitleLerp->time = 0.0f;

		if (!currentRoom->visited) {
			currentRoom->visited = true;
			for (auto& s : currentRoom->spawners) {
				spawners.push_back(s);
			}
		}
		lastRoom = currentRoom;
	}

	if (World::getSlowMotionTimer() <= 0.0f) {
		Configuration::slowMotionFactor = 1.0f;
	}

	if (World::getSlowMotionTimer() <= 1.1f)
	{
		if (World::getPlaySlowMotionExit()) {
			//Audio::PlaySound("assets/sfx/slowmo-exit.wav");
			World::setPlaySlowMotionExit(false);
		}
	}

	handleInput();
	update();

    vf2d camTarget = { (camPos.x / static_cast<float>(playerFilter.count())) * static_cast<float>(Configuration::tileWidth),
                       (camPos.y / static_cast<float>(playerFilter.count())) * static_cast<float>(Configuration::tileHeight) };

    auto minimumCamX = static_cast<float>((currentRoom->pos.x) * Configuration::tileWidth + (20 * Configuration::tileWidth));
    auto maximumCamX = static_cast<float>((currentRoom->pos.x + currentRoom->size.x) * Configuration::tileWidth - (20 * Configuration::tileWidth));
    camTarget.x = std::clamp(camTarget.x, minimumCamX, maximumCamX);

    auto minimumCamY = static_cast<float>((currentRoom->pos.y) * Configuration::tileHeight + (11 * Configuration::tileHeight));
    auto maximumCamY = static_cast<float>((currentRoom->pos.y + currentRoom->size.y) * Configuration::tileHeight - (11 * Configuration::tileHeight));
    camTarget.y = std::clamp(camTarget.y, minimumCamY, maximumCamY);


	Camera::SetTarget(camTarget);

	const auto aiFilter = ECS::getWorld().filter<AIController>();
	aiFilter.each([&](flecs::entity entity, AIController aicontroller) {
		aicontroller.seek();
	});

    if (Input::KeyPressed(SDLK_c)) {
        std::vector<flecs::entity> gateOpenEntities;
        std::vector<flecs::entity> gateCloseEntities;

        const auto gates = ECS::getWorld().filter<GateEntity>();
        gates.each([&](flecs::entity entity, GateEntity gate) {
            if (gate.roomId == currentRoom->id) {
                if (gate.closed) {
                    gateOpenEntities.push_back(entity);
                } else {
                    gateCloseEntities.push_back(entity);
                }
            }
        });

        for (auto entity : gateOpenEntities) entity.get_mut<GateEntity>()->open(entity);
        for (auto entity : gateCloseEntities) entity.get_mut<GateEntity>()->close(entity);
    }



	/*if
X
		for (auto& gameEntity : entities) {
			gameEntity->update(dungeon);
			if (gameEntity->canDelete && State::randomChance(std::max(97.0f + ((0.0f - static_cast<float>(Configuration::gameTime)) / 20.f), 90.0f))) {
				powerup = new Powerup();
				powerup->pos.x = gameEntity->pos.x;
				powerup->pos.y = gameEntity->pos.y;
				powerup->load();
				powerups.emplace_back(powerup);
				powerup = nullptr;
			}
		}

	}*/

	accumulator += (float)Window::GetFrameTime();
	while (accumulator >= physTime)
	{
		accumulator -= physTime;
		World::doStep(physTime, velocityIterations, positionIterations);
	}

	Render2D::BeginMode2D();

    widthMargin = (int)(1.f + (((float)Window::GetWidth() / (float)Configuration::tileWidth) / 2.f) / Camera::GetScale());
    heightMargin = (int)(1.f + (((float)Window::GetHeight() /(float)Configuration::tileHeight) / 2.f) / Camera::GetScale());

    int leftMargin = std::max(static_cast<int>(Camera::GetTarget().x) / Configuration::tileWidth - widthMargin, 0);
    int rightMargin = std::min(leftMargin + widthMargin * 2 + 1, tileData.width);

    int topMargin = std::max(static_cast<int>(Camera::GetTarget().y) / Configuration::tileHeight - heightMargin, 0);
    int bottomMargin = std::min(topMargin + heightMargin * 2 + 1, tileData.height);

	auto localDungeon = World::getDungeon();
    auto localDungeonTiles = localDungeon.getTiles();

	for (int y = topMargin; y < bottomMargin; y++) {
		for (int x = leftMargin; x < rightMargin; x++) {
            int index = y * tileData.width + x;

            const rectf destRect = {
                    static_cast<float>(x * Configuration::tileWidth),
                    static_cast<float>(y * Configuration::tileHeight),
                    static_cast<float>(Configuration::tileWidth),
                    static_cast<float>(Configuration::tileHeight)
            };
            Tile tile = localDungeonTiles.at(index);

            if (tile == Tile::CLOSEDHDOOR || tile == Tile::CLOSEDVDOOR) {
                tile = Tile::FLOOR;
            }


            const rectf srcRect = getTile(tileData.tiles.at(index));

            Render2D::DrawTexturePart(dungeonTileset, {destRect.x, destRect.y}, {destRect.width, destRect.height}, srcRect, WHITE);


            if (tile == Tile::FLOOR) {
                Tile UPLEFTTILE = localDungeon.getTile(x - 1, y - 1);
                Tile LEFTTILE = localDungeon.getTile(x - 1, y);
                Tile UPTILE = localDungeon.getTile(x, y - 1);


                if (LEFTTILE == Tile::WALL && UPTILE == Tile::WALL) {
                    Render2D::DrawTexturePart(dungeonTileset, {destRect.x, destRect.y}, {destRect.width, destRect.height}, getTile(50), WHITE);
                } else if (LEFTTILE == Tile::WALL && (UPTILE == Tile::FLOOR || UPTILE == Tile::CLOSEDHDOOR) && UPLEFTTILE == Tile::WALL) {
                    Render2D::DrawTexturePart(dungeonTileset, {destRect.x, destRect.y}, {destRect.width, destRect.height}, getTile(52), WHITE);
                } else if (LEFTTILE == Tile::WALL && (UPTILE == Tile::FLOOR || UPTILE == Tile::CLOSEDHDOOR) && dungeon.isWalkable(UPLEFTTILE)) {
                    Render2D::DrawTexturePart(dungeonTileset, {destRect.x, destRect.y}, {destRect.width, destRect.height}, getTile(51), WHITE);
                } else if ((LEFTTILE == Tile::FLOOR || LEFTTILE == Tile::CLOSEDHDOOR) && UPTILE == Tile::WALL && UPLEFTTILE == Tile::WALL) {
                    Render2D::DrawTexturePart(dungeonTileset, {destRect.x, destRect.y}, {destRect.width, destRect.height}, getTile(48), WHITE);
                } else if ((LEFTTILE == Tile::FLOOR || LEFTTILE == Tile::CLOSEDHDOOR) && (UPTILE == Tile::FLOOR || UPTILE == Tile::CLOSEDHDOOR) &&
                           UPLEFTTILE == Tile::WALL) {
                    Render2D::DrawTexturePart(dungeonTileset, {destRect.x, destRect.y}, {destRect.width, destRect.height}, getTile(53), WHITE);
                } else if ((LEFTTILE == Tile::FLOOR || LEFTTILE == Tile::CLOSEDHDOOR) && UPTILE == Tile::WALL &&
                           (UPLEFTTILE == Tile::FLOOR || UPLEFTTILE == Tile::CLOSEDHDOOR)) {
                    Render2D::DrawTexturePart(dungeonTileset, {destRect.x, destRect.y}, {destRect.width, destRect.height}, getTile(49), WHITE);
                }

            }

        }
    }

	playerFilter.each([&](flecs::entity entity, PlayerIndex index) {
		auto* input = entity.get_mut<PlayerInput>();
		auto* playerClass = entity.get_mut<PlayerClass>();

		if (input->shooting)
		{
			playerClass->shoot(entity);
		}
	});
	drawEntities();

	World::draw();


	Render2D::EndMode2D();

	drawUI();

	const auto deleteFilter = ECS::getWorld().filter<DeleteMe>();
	std::vector<flecs::entity> entitiesToDelete;
	deleteFilter.each([&](flecs::entity entity, DeleteMe _dm) {
		entitiesToDelete.push_back(entity);

	});

	for (auto& entity: entitiesToDelete) ECS::removeEntity(&entity);

	if (Input::KeyPressed(SDLK_ESCAPE)) {
		//TODO: debug this in linux/VM produces a black screen
		//_th->setTextureFromImage("_pause", LoadImageFromScreen());
		State::SetState("pause");
	}

	if (World::getSlowMotionLerp()->started)
	{
		Configuration::slowMotionFactor = World::getSlowMotionLerp()->getValue();
	}

	if (World::getSlowMotionLerp()->isFinished())
	{
		World::getSlowMotionLerp()->started = false;
	}

	World::setSlowMotionTimer(World::getSlowMotionTimer() - (float)Window::GetFrameTime()); // / Configuration::slowMotionFactor
	Configuration::gameTime += Window::GetFrameTime();
	ECS::getWorld().progress((float)Window::GetFrameTime());
}

void GameState::drawEntities()
{
	for (const auto& _spawner : spawners) { _spawner->draw(); }

	std::vector<flecs::entity> renderableEntities;

	const auto renderFilter = ECS::getWorld().filter<Render2DComp>();
	renderFilter.each([&](flecs::entity entity, Render2DComp renderer) {
		renderableEntities.push_back(entity);
		renderer.draw(&entity);
	});

	std::sort(renderableEntities.begin(), renderableEntities.end(),
		[](const flecs::entity& entity1, const flecs::entity& entity2) {
			const vf2d pos1 = entity1.get<RigidBody2D>()->RigidBody->GetPosition();
			const vf2d pos2 = entity2.get<RigidBody2D>()->RigidBody->GetPosition();
			return pos1.y < pos2.y;
		});

	for (auto& entity : renderableEntities) {
		entity.get_mut<Render2DComp>()->draw(&entity);
		
	}

	std::vector<flecs::entity> entitiesHealth;

	const auto healthFilter = ECS::getWorld().filter<Health>();
	healthFilter.each([&](flecs::entity entity, Health health) {

		health.draw(entity);

		if (health.currentHealth <= 0.0f) {
			entitiesHealth.push_back(entity);
			
		}
	});

	for (auto entity: entitiesHealth)
	{
		entity.set<DeleteMe>({});
	}

}

void GameState::update()
{
	/*for (const auto& _player : players) {

		_player->update(dungeon);
		for (const auto& _powerup : powerups) {
			if (doCirclesOverlap(_powerup->pos.x, _powerup->pos.y, _powerup->radius,
				_player->pos.x, _player->pos.y, _player->radius)) {
				_powerup->handlePowerUp(_player);
			}
		}
	}*/

	for (const auto& _spawner : spawners) {
		_spawner->spawnTimer -= (float)(Window::GetFrameTime() / Configuration::slowMotionFactor);
		if (_spawner->spawnTimer <= 0.0f && _spawner->spawnsLeft > 0 && _spawner->spawnsLeft <= _spawner->maxSpawns) {
			CreateLizardEntity(_spawner->pos + 2.f, 5 * Helpers::getDifficultyModifier(static_cast<float>(Configuration::gameTime)));

			_spawner->spawnTimer = 2.0f;
			_spawner->spawnsLeft--;
		}
	}
}

void GameState::handleInput()
{
	//only for global input
	if (Input::KeyPressed(SDLK_LEFTBRACKET)) {
		Camera::SetScale( Camera::GetScale() / 2.0f);
		if (Camera::GetScale() < 0.5f) {
			Camera::SetScale( 0.5f);
		}
	}

	if (Input::KeyPressed(SDLK_RIGHTBRACKET)) {
		Camera::SetScale( Camera::GetScale() * 2.0f);
	}

	if (Input::KeyPressed(SDLK_m))
	{
		miniMapVisible = !miniMapVisible;
	}

	if (Input::KeyPressed(SDLK_F1)) {
		/*if (players.size() == 1) {
			players.emplace_back(new Wizard());

			players[1]->pos = players[0]->pos + vf2d({ 2.5f, 0.5f });
			players[1]->load();

			World::addObject(players[1]);

			setupControls();
		}*/
	}
}

void GameState::drawUI()
{
	// UI
	const auto playerFilter = ECS::getWorld().filter<PlayerIndex>();
	playerFilter.each([&](flecs::entity entity, PlayerIndex index) {
        UI::DrawPlayerBar(entity);
	});

	UI::drawTimerBar();

	if (miniMapVisible) UI::DrawMiniMap(minimapTexture);
	if (Configuration::showGameStats) { UI::drawDebugInfo(); }


    Log::resetLog();

	if (Input::KeyPressed(SDLK_F10)) {
		World::clearObjects();
		World::generateNewMap();

		resetGame();
	}

	if (Input::KeyPressed(SDLK_F11) && !Input::KeyDown(SDLK_LSHIFT)) {
        Configuration::showGameStats = !Configuration::showGameStats;
    }


	if (!roomTitleLerp->isFinished() && currentRoom) {

		Color titleColor = { 255,255,255,255 };


		int textWidth = Fonts::MeasureText("assets/fonts/APL386.ttf", 40, currentRoom->name);

		float newAlpha = 0.0f;

		if (roomTitleLerp->getValue() <= 1.0f) {
			newAlpha = roomTitleLerp->getValue();
		}
		else if (roomTitleLerp->getValue() < 7.0f) {
			newAlpha = 1.f;
		}
		else {
			newAlpha = (3.f - (roomTitleLerp->getValue() - 7.f)) / 3.f;
		}

		titleColor.a = static_cast<int>(newAlpha * 255.f);

		Fonts::DrawText("assets/fonts/APL386.ttf", 40, {((float)Window::GetWidth() / 2.f) - ((float)textWidth / 2.f), 100.f,}, currentRoom->name.c_str(), titleColor);
	}

}
