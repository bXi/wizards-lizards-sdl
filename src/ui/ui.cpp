#include "ui.h"
#include "configuration/configuration.h"
#include "texture/texturehandler.h"
#include "log/loghandler.h"

#include "components/rigidbody2d.h"
#include "components/playerinput.h"
#include "components/playerclass.h"
#include "components/playerindex.h"

void UI::drawSmallBar(const vf2d &pos, const vi2d &offset, const int width, const int height, const Color color,
                      const float percentage) {
    const vf2d localOffset = offset;

    const auto tileWidth = static_cast<float>(Configuration::tileWidth);
    const auto tileHeight = static_cast<float>(Configuration::tileHeight);

    Render2D::DrawRectangleFilled({((pos.x * tileWidth) + localOffset.x    ), ((pos.y * tileHeight) + localOffset.y    )}, {(float)width,                    (float)height},         WHITE);
    Render2D::DrawRectangleFilled({((pos.x * tileWidth) + localOffset.x + 1), ((pos.y * tileHeight) + localOffset.y + 1)}, {(float)width - 2,              (float)height - 2}, BLACK);
    Render2D::DrawRectangleFilled({((pos.x * tileWidth) + localOffset.x + 1), ((pos.y * tileHeight) + localOffset.y + 1)}, {(float)(width - 2) * percentage, (float)height - 2}, color);
}

void UI::drawTimerBar() {

    int screenWidth = Window::GetWidth();
    int screenHeight = Window::GetHeight();

    int barThickness = 8;

    Render2D::DrawRectangleFilled({0, 0}, {(float)barThickness, (float)screenHeight}, BLACK); // Left
    Render2D::DrawRectangleFilled({(float)screenWidth - (float)barThickness, 0}, {(float)barThickness, (float)screenHeight}, BLACK); // Right
    Render2D::DrawRectangleFilled({0, 0}, {(float)screenWidth, (float)barThickness}, BLACK); // Top
    Render2D::DrawRectangleFilled({0, (float)screenHeight - (float)barThickness}, {(float)screenWidth, (float)barThickness}, BLACK); // Bottom


    Rectangle bars[5] = {
        {  (float)screenWidth / 2.f, 0, (float)screenWidth / 2.f, (float)barThickness }, // Top bar
        {  (float)screenWidth - (float)barThickness,  0.f, (float)barThickness,  (float)screenHeight }, // Right bar
        {  0.f,  (float)screenHeight - (float)barThickness,  (float)screenWidth, (float)barThickness }, // Bottom bar
        { 0.f,  0.f, (float)barThickness,  (float)screenHeight }, // Left bar
        { 0.f, 0.f,  (float)screenWidth / 2.f, (float)barThickness } // Top-left bar
    };

    double elapsed = Window::GetRunTime(); // Elapsed time in seconds
    double rate = 200.0; // Pixels per second

    float totalLength = bars[0].width + bars[1].height + bars[2].width + bars[3].height + bars[4].width;

    float totalBarLength = elapsed * rate; // na 1.0 seconde 100 pixels lang

    int rotations = std::floor(totalBarLength / totalLength);

    int barcolor = std::clamp(rotations, 0, 4);

    totalBarLength -= (float)rotations * totalLength;


    Color c[5] = {
            RED, GREEN, YELLOW, PURPLE, BLUE
    };

    if (bars[0].width > totalBarLength)
    {
        bars[0].width = totalBarLength;
    }

    if (bars[1].height > totalBarLength - bars[0].width)
    {
        bars[1].height = totalBarLength - bars[0].width;
    }

    if (bars[2].width > totalBarLength - (bars[0].width + bars[1].height))
    {
        bars[2].x = bars[2].width - (totalBarLength - (bars[0].width + bars[1].height));
    }

    if (bars[3].height > totalBarLength - (bars[0].width + bars[1].height + bars[2].width))
    {
        bars[3].y = bars[3].height - (totalBarLength - (bars[0].width + bars[1].height + bars[2].width));
    }

    if (bars[4].width > totalBarLength - (bars[0].width + bars[1].height + bars[2].width + bars[3].height))
    {
        bars[4].width = (totalBarLength - (bars[0].width + bars[1].height + bars[2].width + bars[3].height));
    }

    for (int i = 0; i < 5; ++i) {
        Render2D::DrawRectangleFilled({bars[i].x, bars[i].y}, {bars[i].width, bars[i].height}, c[barcolor]);
    }


}

[[maybe_unused]] void UI::DrawPlayerBar(flecs::entity entity) {
    const Texture uiElements = Textures::GetTexture("assets/ui/ui.png");
    const Texture items = Textures::GetTexture("assets/tilesets/items-1.png");

    Rectangle selectedPowerUpBox = {539, 460, 24, 24};

    auto player = entity.get_mut<PlayerInput>();

    auto rigidBody2d = entity.get_mut<RigidBody2D>()->RigidBody;
    auto playerClass = entity.get<PlayerClass>();

    std::vector<int> weaponIcon = {0, 0, 0, 0};

    switch (playerClass->classtype) {
        case PlayerClassType::WIZARD:
            weaponIcon.at(0) = 64;
            weaponIcon.at(1) = 79;
            weaponIcon.at(2) = 72;
            weaponIcon.at(3) = 101;
            break;
        case PlayerClassType::BERSERKER:
            weaponIcon.at(0) = 63;
            weaponIcon.at(1) = 79;
            weaponIcon.at(2) = 72;
            weaponIcon.at(3) = 101;
            break;
        case PlayerClassType::BOUNCER:
            weaponIcon.at(0) = 62;
            weaponIcon.at(1) = 79;
            weaponIcon.at(2) = 72;
            weaponIcon.at(3) = 101;
            break;
        case PlayerClassType::TRAPPER:
            weaponIcon.at(0) = 61;
            weaponIcon.at(1) = 79;
            weaponIcon.at(2) = 72;
            weaponIcon.at(3) = 101;
            break;

    }

    int weaponCount = 4;
    float circleRadius = 80.0f;

    Log::AddLine("Campos:", Helpers::TextFormat("X: %.2f Y: %.2f", Camera::GetTarget().x, Camera::GetTarget().y));

    vf2d worldPos = Camera::ToScreenSpace((vf2d) rigidBody2d->GetPosition() * 32);

    float selectedWeaponAngleOffset = -((static_cast<float>(player->selectedWeapon - 1) / static_cast<float>(weaponCount)) * 2 * PI) + (PI / 2) * 3;

    Log::AddLine("lerp:", Helpers::TextFormat("%.2f", player->weaponSelectLerp->getValue()));

    if (player->weaponSelectLerp->isFinished()) {
        player->weaponSelectVisibleLerp->started = true;
    }

    int previousWeapon = player->selectedWeapon - 1;

    if (player->moveWeaponLeft) {
        previousWeapon += 1;
    } else {
        previousWeapon -= 1;
    }

    if (previousWeapon < 0) previousWeapon = weaponCount - 1;
    if (previousWeapon > weaponCount - 1) previousWeapon = 0;


    for (int i = 0; i < weaponCount; ++i) {
        float angle = selectedWeaponAngleOffset + (static_cast<float>(i) / static_cast<float>(weaponCount)) * 2 * PI;

        if (!player->weaponSelectLerp->started) {
            angle -= (PI / 2);
        } else {
            if (player->moveWeaponLeft) {
                angle += (PI / 2) * 2 + (PI / 2) * player->weaponSelectLerp->getValue();
            } else {
                angle -= (PI / 2) * player->weaponSelectLerp->getValue();
            }

        }

        Rectangle itemPosition = {
                (worldPos.x + circleRadius * sin(angle)) - 8.f,
                (worldPos.y + circleRadius * cos(angle)) - 8.f,
                16.f,
                16.f,
        };

        Rectangle boxPosition = {
                itemPosition.x - 16,
                itemPosition.y - 18,
                48.f,
                48.f,
        };

        auto color = WHITE;

        if (player->selectedWeapon - 1 == i) {
            selectedPowerUpBox.y = 492;
            color.a = static_cast<unsigned char>(255.f * player->weaponSelectLerp->getValue());

            if (player->weaponSelectVisibleLerp->started) {
                color.a = static_cast<unsigned char>(255.f * (1.f - std::clamp(player->weaponSelectVisibleLerp->getValue(), 0.f, 1.f)));
            }

        } else if (previousWeapon == i) {
            if (player->weaponSelectLerp->started)
                color.a = static_cast<unsigned char>(255.f * (1.f - player->weaponSelectLerp->getValue()));
            else
                color.a = 0;

        } else {
            selectedPowerUpBox.y = 460;
            color.a = 0;
        }


        Render2D::DrawTexturePart(uiElements, {boxPosition.x, boxPosition.y}, {boxPosition.width, boxPosition.height}, selectedPowerUpBox, color);
        auto weaponIconPos = Textures::GetTile(weaponIcon[i]);

        Render2D::DrawTexturePart(items, {itemPosition.x, itemPosition.y}, {itemPosition.width, itemPosition.height}, weaponIconPos, color);


    }
}

void UI::DrawMiniMap(Texture texture) {

    int miniMapSize = 200;

    auto data = World::getDungeon().getDungeonTileData();
    auto rooms = World::getDungeon().getRooms();

    SDL_SetRenderTarget(Window::GetRenderer(), texture.texture);

    int offsetLeft = (Window::GetWidth() / 2) - miniMapSize / 2;
    int offsetTop = Window::GetHeight() - miniMapSize;


    auto borderColor = GRAY;

//    ClearBackground(borderColor);

    for (const Room *room: rooms) {
        int miniMapX = room->pos.x * miniMapSize / data.width;
        int miniMapY = room->pos.y * miniMapSize / data.height;

        int miniMapWidth = room->size.x * miniMapSize / data.width;
        int miniMapHeight = room->size.y * miniMapSize / data.height;

        auto bgColor = BLACK;

        if (room->start) {
            bgColor = DARKGREEN;
        } else if (room->end) {
            bgColor = {115, 21, 27, 255};
        }

        if (room->specialRoom) {
            bgColor = PURPLE;
        }

        Render2D::DrawRectangleFilled({(float)miniMapX, (float)miniMapY + 1.f},
                                {(float)miniMapWidth, (float)miniMapHeight},
                      bgColor);
    }

    for (const Room *room: rooms) {
        int miniMapX = room->pos.x * miniMapSize / data.width;
        int miniMapY = room->pos.y * miniMapSize / data.height;

        int miniMapWidth = room->size.x * miniMapSize / data.width;
        int miniMapHeight = room->size.y * miniMapSize / data.height;

        Render2D::DrawRectangle({(float)miniMapX, (float)miniMapY},
                                {(float)miniMapWidth + 1, (float)miniMapHeight + 1},
                           borderColor);

    }

    for (const Room *room: rooms) {
        int miniMapX = room->pos.x * miniMapSize / data.width;
        int miniMapY = room->pos.y * miniMapSize / data.height;

        int roomSizeX = room->size.x / 34;
        int roomSizeY = room->size.y / 34;

        auto bgColor = BLACK;

        int doorSize = 6; // Adjust the size of the doorways
        int totalRoomSize = miniMapSize / 8;

        if (room->northExits > 0) {
            int doorX = miniMapX + (totalRoomSize / 2) - (doorSize / 2);
            int doorY = miniMapY;
            int doorBit = 1;

            for (int i = 0; i < roomSizeX; i++) {
                if (room->northExits & doorBit) {
                    Render2D::DrawRectangleFilled({(float)doorX, (float)doorY - 1}, {(float)doorSize, 1}, bgColor);
                }
                doorX += totalRoomSize;
                doorBit <<= 1;
            }
        }


        if (room->southExits > 0) {
            int doorX = miniMapX + (totalRoomSize / 2) - (doorSize / 2);
            int doorY = miniMapY + (roomSizeY * totalRoomSize);
            int doorBit = 1;

            for (int i = 0; i < roomSizeX; i++) {
                if (room->southExits & doorBit) {
                    Render2D::DrawRectangleFilled({(float)doorX, (float)doorY}, {(float)doorSize, 1}, bgColor);

                }
                doorX += totalRoomSize;
                doorBit <<= 1;
            }
        }

        if (room->eastExits > 0) {
            int doorX = miniMapX + (roomSizeX * totalRoomSize);
            int doorY = miniMapY + (totalRoomSize / 2) - (doorSize / 2);
            int doorBit = 1;

            for (int i = 0; i < roomSizeY; i++) {
                if (room->eastExits & doorBit) {
                    Render2D::DrawRectangleFilled({(float)doorX, (float)doorY}, {1, (float)doorSize}, bgColor);

                }
                doorY += totalRoomSize;
                doorBit <<= 1;
            }
        }

        if (room->westExits > 0) {
            int doorX = miniMapX;
            int doorY = miniMapY + (totalRoomSize / 2) - (doorSize / 2);
            int doorBit = 1;

            for (int i = 0; i < roomSizeY; i++) {
                if (room->westExits & doorBit) {
                    Render2D::DrawRectangleFilled({(float)doorX, (float)doorY}, {1, (float)doorSize}, bgColor);

                }
                doorY += totalRoomSize;
                doorBit <<= 1;
            }
        }
    }

    const auto playerFilter = ECS::getWorld().filter<PlayerIndex>();
    playerFilter.each([&](flecs::entity entity, PlayerIndex index) {
        auto *rigidBody2D = entity.get_mut<RigidBody2D>()->RigidBody;

        vf2d pos = rigidBody2D->GetPosition();

        pos.x *= static_cast<float>(miniMapSize) / static_cast<float>(data.width);
        pos.y *= static_cast<float>(miniMapSize) / static_cast<float>(data.height);

        auto playerColor = WHITE;

        playerColor.a = static_cast<unsigned char>(255.f - (std::fmod(Window::GetRunTime(), 0.4f) * 600.f));

        Render2D::DrawCircle(pos, 2.f, playerColor);


    });


    SDL_SetRenderTarget(Window::GetRenderer(), nullptr);


    //Rectangle srcRect = {0.f, 0.f, static_cast<float>(texture.width), -static_cast<float>(texture.height)};
    //Rectangle dstRect = {static_cast<float>(offsetLeft), static_cast<float>(offsetTop + 1), static_cast<float>(texture.width), static_cast<float>(texture.height)};

    Render2D::DrawTexture(texture, {static_cast<float>(offsetLeft), static_cast<float>(offsetTop + 1)}, {static_cast<float>(texture.width), static_cast<float>(texture.height)}, {255, 255, 255, 200});

}

[[maybe_unused]] void UI::drawDebugInfo() {
    constexpr int baseX = 30;
    constexpr int baseY = 270;
    int margin = 20;

    //auto font = Fonts::GetFont("assets/fonts/APL386.ttf", 20);

    int y = 0;


    Log::AddLine("FPS:", Helpers::TextFormat("%d", Window::GetFPS()));

    Log::AddLine("GameTime:", Helpers::TextFormat("%.2f", Configuration::gameTime));
    Log::AddLine("Bodies:", Helpers::TextFormat("%d", World::getPhysicsWorld().GetBodyCount()));
    Log::AddLine("SlowMoF:", Helpers::TextFormat("%f", Configuration::slowMotionFactor));


    Log::AddLine("");
    Log::AddLine("-- Players --");

    const auto playerFilter = ECS::getWorld().filter<PlayerIndex, RigidBody2D>();
    playerFilter.each([&](flecs::entity entity, PlayerIndex playerIndex, RigidBody2D rigidBody2D) {
        Log::AddLine(
                Helpers::TextFormat("Player %d pos:", playerIndex.index),
                Helpers::TextFormat("X: %.2f Y: %.2f", rigidBody2D.RigidBody->GetPosition().x,
                           rigidBody2D.RigidBody->GetPosition().y)
        );
    });

    auto lines = Log::getLines();

    Rectangle backgroundRect = {
            static_cast<float>(baseX - margin),
            static_cast<float>(baseY - margin),
            static_cast<float>(Log::getHeaderWidth() + Log::getLongestLineWidth() + (margin * 3)),
            static_cast<float>((lines.size() * 25) + (margin * 2) - 10)
    };

    Render2D::DrawRectangleRoundedFilled({backgroundRect.x, backgroundRect.y}, {backgroundRect.width, backgroundRect.height}, 10.f, {0, 0, 0, 127});


    for (const auto &line: lines) {
        Fonts::DrawText("assets/fonts/APL386.ttf", 18, vi2d(baseX, baseY + y), line.first.c_str(), LIME);
        Fonts::DrawText("assets/fonts/APL386.ttf", 18, vi2d(baseX + Log::getHeaderWidth() + margin, baseY + y), line.second.c_str(), LIME);

        y += 25;
    }


}
