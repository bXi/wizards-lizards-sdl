#include "test.h"

#include "state/state.h"
#include "box2dobjects.h"

#include "utils/vectors.h"
#include "utils/colors.h"

int lastDungeonSize = 0;

#include <ui/ui.h>

b2Vec2 Gravity(0.0f, 98.0f);    // Y+ is down, so gravity is not negative
b2World World(Gravity);

void TestState::load()
{
    Audio::GetMusic("assets/music/menu.mp3");

    Audio::GetSound("assets/sfx/slowmo-enter.wav");
    Audio::GetSound("assets/sfx/cursor.wav");
    dungeonTileset = Textures::GetTexture("assets/tilesets/dungeon.png");

	//Lerp test
	testlerp = Lerp::getLerp("Testlerp", 400.0f, 400.0f, 3.0f);

	//Quadtree test
	for (int i = 0; i < 200; i++)
	{
		entity = new Testpoint();
		entity->pos.x = static_cast<float>(Helpers::GetRandomValue(0,size));
		entity->pos.y = static_cast<float>(Helpers::GetRandomValue(0, size));
		testPoints.emplace_back(entity);
		entity = nullptr;
	}


	dg = new DungeonGen();

	Objects.push_back(new BoxObject(vf2d{ 0,-50 }, vf2d{ 10,10 }, RED, 45));
	Objects.push_back(new BoxObject(vf2d{ 20,-60 }, vf2d{ 12,8 }, GREEN, 30));
	Objects.push_back(new BoxObject(vf2d{ -20,-60 }, vf2d{ 8,12 }, BLUE, -30));

	Camera::SetTarget({((float)(Window::GetWidth() - 300) / 2.0f) + 300 , (float)(Window::GetHeight() - 20)});
    //Camera::SetScale(4.f);

	//ground = BoxObject({ 0, 0 }, { 200, 2 }, BROWN, 0, false);






    vecShapes.push_back({ Point{ { { 550.0f, 10.0f } } } });

		vecShapes.push_back({ Line{ { { 320.0f, 10.0f }, {350.0f, 70.0f} } } });
		vecShapes.push_back({ Line{ { { 380.0f, 10.0f }, {310.0f, 20.0f} } } });

		vecShapes.push_back({ Rect{ { { 380.0f, 10.0f }, {410.0f, 60.0f} } } });

		vecShapes.push_back({ Circle{ { { 430.0f, 20.0f }, {470.0f, 20.0f} } } });
		vecShapes.push_back({ Circle{ { { 630.0f, 300.0f }, {720.0f, 300.0f} } } });
		vecShapes.push_back({ Circle{ { { 630.0f, 300.0f }, {700.0f, 300.0f} } } });

		vecShapes.push_back({ Triangle{{ {350.0f, 100.0f}, {310.0f, 150.0f}, {390.0f, 150.0f}} }});
		vecShapes.push_back({ Triangle{{ {650.0f, 200.0f}, {800.0f, 150.0f}, {750.0f, 400.0f}} }});



}

void TestState::unload()
{
    Audio::StopMusic();
}

void TestState::draw()
{
	if (Input::KeyPressed(SDLK_ESCAPE)) State::SetState("menu");
	{
		constexpr int x = 10;
		int y = 10;

		const char* backText = " ESC: back to menu";
		maxWidth = Fonts::MeasureText("assets/fonts/APL386.ttf", 18, backText);

		Fonts::DrawText("assets/fonts/APL386.ttf", 18, {x, Window::GetHeight() - 30.f},backText, WHITE);

#ifndef MSVC
//        std::reverse(testList);
#endif

		for (const auto& item : testList)
		{
			const auto name = Helpers::TextFormat("%3u: %s", static_cast<int>(item.first) + 1, item.second.c_str());
			Fonts::DrawText("assets/fonts/APL386.ttf", 18, {(float)x, (float)y } , name, (item.first == selected) ? YELLOW : WHITE);
			maxWidth = std::max(maxWidth, Fonts::MeasureText("assets/fonts/APL386.ttf", 18, name));

            y += 24;
		}

		Render2D::DrawLine({maxWidth + 40.f, 0}, {maxWidth + 40.f, (float)Window::GetHeight()}, WHITE);
	}
	for (const auto &key : keys)
	{
		if (Input::KeyPressed(key.first)) selected = key.second;
	}

	switch (selected)
	{
	case Test::Quadtree:
	{
		if (Input::KeyPressed(SDLK_SPACE)) isCircle = !isCircle;

		const vi2d drawOffset = { 400, 50 };


		if (Input::MouseButtonDown(SDL_BUTTON_LEFT))
		{

			auto x = static_cast<float>(Input::GetMousePosition().x - drawOffset.x);
			auto y = static_cast<float>(Input::GetMousePosition().y - drawOffset.y);

			if (x >= 0.0f && x <= static_cast<float>(size) && y >= 0.0f && y <= static_cast<float>(size))
			{
				entity = new Testpoint();
				entity->pos.x = x;
				entity->pos.y = y;
				testPoints.emplace_back(entity);
				entity = nullptr;
			}
		}

		const rectf rect = {0.0f, 0.0f, static_cast<float>(size), static_cast<float>(size) };

		QuadTree quadtree = QuadTree(rect);

		for (const auto& testpoint : testPoints) {
			quadtree.insert({ testpoint->pos.x, testpoint->pos.y, testpoint });
		}

		int found = 0;

		for (const auto& testpoint : testPoints) {
			std::vector<void*> quadtreeEntities;

			constexpr float rectSize = 100.0f;
			if (isCircle)
			{
				QuadTree::AABBCircle searchRange = { static_cast<float>(Input::GetMousePosition().x), static_cast<float>(Input::GetMousePosition().y), rectSize / 2.0f };
				Render2D::DrawCircle({(searchRange._x), (searchRange._y)}, searchRange._r, { 255,255,255,127 });
				searchRange._x -= static_cast<float>(drawOffset.x);
				searchRange._y -= static_cast<float>(drawOffset.y);
				quadtree.query(searchRange, &quadtreeEntities);

			}
			else
			{
				QuadTree::AABB searchRange = { static_cast<float>(Input::GetMousePosition().x) - rectSize / 2.0f, static_cast<float>(Input::GetMousePosition().y) - rectSize / 2.0f, rectSize, rectSize };
				Render2D::DrawRectangle({ searchRange._left, searchRange._top}, {searchRange._width, searchRange._height }, { 255,255,255,127 });
				searchRange._left -= static_cast<float>(drawOffset.x);
				searchRange._top -= static_cast<float>(drawOffset.y);
				quadtree.query(searchRange, &quadtreeEntities);

			}

			testpoint->intersects = false;
			for (const auto& target : quadtreeEntities) {
				auto entity = reinterpret_cast<Testpoint*>(target);
				entity->intersects = true;
			}
			testpoint->draw(drawOffset.x, drawOffset.y);
			found = static_cast<int>(quadtreeEntities.size());
			quadtreeEntities.clear();
		}
		quadtree.draw(drawOffset.x, drawOffset.y, WHITE);
		quadtree.reset();
		Fonts::DrawText("assets/fonts/APL386.ttf", 18, {(float)drawOffset.x, Window::GetHeight() - 30.f}, Helpers::TextFormat("Points: %u | Found: %u", static_cast<int>(testPoints.size()), found), WHITE);
		Fonts::DrawText("assets/fonts/APL386.ttf", 18, {300.f, Window::GetHeight() - 30.f}, Helpers::TextFormat("%d", Window::GetFPS()), LIME);

	} break;
	case Test::Lerp:
	{
		Render2D::DrawCircleFilled({testlerp->getValue(), 100.f}, 10.0f, PINK);

		if (Input::KeyPressed(SDLK_SPACE))
		{
			testlerp->time = 0.0f;
		}

		if (Input::KeyPressed(SDLK_a))
		{
			testlerp->callback = EaseExpoInOut;
		}
		if (Input::KeyPressed(SDLK_s))
		{
			testlerp->callback = EaseBounceOut;
		}

	} break;
	case Test::Audio:
	{
		if (Input::KeyPressed(SDLK_a))
		{
            Audio::PlaySound("assets/sfx/cursor.wav");
		}
		if (Input::KeyPressed(SDLK_s))
		{
            Audio::PlaySound("assets/sfx/slowmo-enter.wav");
		}
		if (Input::KeyPressed(SDLK_d))
		{
			if (Audio::IsMusicPlaying())
			{
                Audio::StopMusic();
			} else
			{
                Audio::PlayMusic("assets/music/menu.mp3");
			}
		}
	} break;
	case Test::Dungeon:
	{
		if (Input::KeyPressed(SDLK_a)) {
			dg->generateLayout();
		}

		if (Input::KeyPressed(SDLK_s)) {
			showLines = !showLines;
		}


		const vi2d drawOffset = { 400, 50 };
		vi2d size = dg->getSize();
		int scalefactor = 512 / size.x;


		if (Input::KeyPressed(SDLK_m)) {
			auto mapString = dg->getMapString();

			auto getTile = [&](int x, int y) {
				if (x < 0 || y < 0 || x >= size.x * 18 || y >= size.y * 18)
					return '.';

				const auto tileId = x + (y * size.x * 18);
				return mapString[static_cast<size_t>(tileId)];
			};
			std::cout << std::endl;
			std::cout << std::endl;

			int _height = size.x * 18;
			int _width = size.y * 18;
				for (int y = 0; y < _height; ++y) {
					for (int x = 0; x < _width; ++x)
						std::cout << getTile(x, y);

					std::cout << std::endl;
				}

		}


		Render2D::DrawRectangleFilled({(float)drawOffset.x, (float)drawOffset.y}, {(float)size.x * scalefactor, (float)size.y * scalefactor}, WHITE);

		auto rooms = dg->getRooms();

		for (int y = 0; y < size.y; y++) {
			for (int x = 0; x < size.x; x++) {
				
				int xPos = drawOffset.x + (x * scalefactor);
				int yPos = drawOffset.y + (y * scalefactor);

				Render2D::DrawRectangleFilled({(float)xPos, (float)yPos}, {(float)scalefactor, (float)scalefactor}, DARKGRAY);
			}
		}

		for (auto& room : rooms) {
			int xPos = drawOffset.x + (room->pos.x * scalefactor);
			int yPos = drawOffset.y + (room->pos.y * scalefactor);

			Color color = WHITE;

			if (room->name == "Start") color = GREEN;
			else if (room->name == "End") color = RED;



			Render2D::DrawRectangleFilled({(float)xPos, (float)yPos}, {(float)scalefactor * room->size.x, (float)scalefactor * room->size.y}, BLACK);

			if (room->northExits == 0 && room->southExits == 0 && room->eastExits == 0 && room->westExits == 0) {
				Render2D::DrawRectangleFilled({(float)xPos, (float)yPos}, {(float)scalefactor * room->size.x, (float)scalefactor * room->size.y}, DARKRED);

			}

            auto CheckCollisionPointRec = [&](vf2d point, rectf rect){
                return (point.x >= rect.x && point.x <= rect.x + rect.width && point.y >= rect.y && point.y <= rect.y + rect.height);
            };


			if (CheckCollisionPointRec(Input::GetMousePosition(), rectf(static_cast<float>(xPos), static_cast<float>(yPos), static_cast<float>(scalefactor) * room->size.x, static_cast<float>(scalefactor) * room->size.y))) {
				color = PINK;

				Fonts::DrawText("assets/fonts/APL386.ttf", 18, {1000, 100}, Helpers::TextFormat("ID: %d/%d", room->id, rooms.size()),  LIME);
				Fonts::DrawText("assets/fonts/APL386.ttf", 18, {1000, 125}, Helpers::TextFormat("North: %d", room->northExits),  LIME);
				Fonts::DrawText("assets/fonts/APL386.ttf", 18, {1000, 150}, Helpers::TextFormat("South: %d", room->southExits),  LIME);
				Fonts::DrawText("assets/fonts/APL386.ttf", 18, {1000, 175}, Helpers::TextFormat("East: %d", room->eastExits),  LIME);
				Fonts::DrawText("assets/fonts/APL386.ttf", 18, {1000, 200}, Helpers::TextFormat("West: %d", room->westExits),  LIME);
			}



			Render2D::DrawRectangle({(float)xPos, (float)yPos}, {(float)scalefactor * room->size.x, (float)scalefactor * room->size.y}, color);
			Render2D::DrawRectangle({xPos + 1.f, yPos + 1.f}, {(scalefactor * room->size.x) - 2.f, (scalefactor * room->size.y) - 2.f}, color);

			int doorSize = scalefactor / 4; // Adjust the size of the doorways
			
			if (room->northExits > 0) {
				int doorX = xPos + (scalefactor / 2) - (doorSize / 2);
				int doorY = yPos;
				int doorBit = 1;

				for (int i = 0; i < room->size.x; i++) {
					if (room->northExits & doorBit) {
                        Render2D::DrawRectangleFilled({(float)doorX, (float)doorY}, {(float)doorSize, (float)doorSize}, showLines ? RED : BLACK);
					}
					doorX += scalefactor;
					doorBit <<= 1;
				}
			}

			if (room->southExits > 0) {
				int doorX = xPos + (scalefactor / 2) - (doorSize / 2);
				int doorY = yPos + (room->size.y * scalefactor) - doorSize;
				int doorBit = 1;

				for (int i = 0; i < room->size.x; i++) {
					if (room->southExits & doorBit) {
                        Render2D::DrawRectangleFilled({(float)doorX, (float)doorY}, {(float)doorSize, (float)doorSize}, showLines ? YELLOW : BLACK);
					}
					doorX += scalefactor;
					doorBit <<= 1;
				}
			}

			if (room->eastExits > 0) {
				int doorX = xPos + (room->size.x * scalefactor) - doorSize;
				int doorY = yPos + (scalefactor / 2) - (doorSize / 2);
				int doorBit = 1;

				for (int i = 0; i < room->size.y; i++) {
					if (room->eastExits & doorBit) {
                        Render2D::DrawRectangleFilled({(float)doorX, (float)doorY}, {(float)doorSize, (float)doorSize}, showLines ? BLUE : BLACK);
					}
					doorY += scalefactor;
					doorBit <<= 1;
				}
			}

			if (room->westExits > 0) {
				int doorX = xPos;
				int doorY = yPos + (scalefactor / 2) - (doorSize / 2);
				int doorBit = 1;

				for (int i = 0; i < room->size.y; i++) {
					if (room->westExits & doorBit) {
                        Render2D::DrawRectangleFilled({(float)doorX, (float)doorY}, {(float)doorSize, (float)doorSize}, showLines ? GREEN : BLACK);
					}
					doorY += scalefactor;
					doorBit <<= 1;
				}
			}


			if (showLines) {
				for (auto& edge : dg->edges)
				{
					const auto firstRoom = dg->getRoomById(edge.first.first);
					const auto secondRoom = dg->getRoomById(edge.first.second);

					vf2d firstPoint = (vf2d(firstRoom->size) / 2.f + firstRoom->pos) * static_cast<float>(scalefactor);
					vf2d secondPoint = (vf2d(secondRoom->size) / 2.f + secondRoom->pos) * static_cast<float>(scalefactor);

					Render2D::DrawCircleFilled(firstPoint + drawOffset, 3, PINK);
					Render2D::DrawCircleFilled(secondPoint + drawOffset, 3, LIME);

					Render2D::DrawLine(firstPoint + drawOffset, secondPoint + drawOffset, YELLOW);

				}
			}
		}

		
	} break;
	case Test::LineIntersect:
	{

		const vf2d drawOffset = { 400.0f, 50.0f };

		rectf rect = {100.0f, 100.0f, 400.0f, 400.0f };
		vf2d lineStart = { 300.0f, 300.0f };
		vf2d lineEnd = Input::GetMousePosition();
		lineEnd -= drawOffset;

		rectf drawRect = rect;
		drawRect.x += drawOffset.x;
		drawRect.y += drawOffset.y;

		//DrawRectangleLinesEx(drawRect, 1.0f, WHITE);

        Render2D::DrawRectangle({drawRect.x, drawRect.y}, {drawRect.width, drawRect.height}, WHITE);


		Color lineColor = RED;

		if (Helpers::lineIntersectsRectangle(lineStart, lineEnd, rect))
		{
			lineColor = GREEN;
		}

		//Render2D::DrawLine(lineStart + drawOffset, lineEnd + drawOffset, lineColor);

        Render2D::DrawLine(lineStart + drawOffset, lineEnd + drawOffset, lineColor);

	} break;
	case Test::Box2D: {
        //TODO: fix this
//		cam.offset.x = ((Window::GetWidth() - (maxWidth + 40)) / 2.0f) + (maxWidth + 40);

		scissorArea.x = static_cast<float>(maxWidth + 40);
		scissorArea.width = static_cast<float>(Window::GetWidth() - (maxWidth + 40));
	
		scissorArea.height = static_cast<float>(Window::GetHeight());
	
		Render2D::BeginScissorMode({scissorArea.x, scissorArea.y, scissorArea.width, scissorArea.height});

		vf2d pos = Camera::ToWorldSpace(Input::GetMousePosition());
		if (Input::MouseButtonPressed(SDL_BUTTON_LEFT))
		{

			if (Input::KeyDown(SDLK_LSHIFT)) {
				for (auto object : Objects) {
					if (object->RigidBody->GetUserData().pointer) {

						auto objPtr = object->RigidBody->GetUserData().pointer;

						auto obj = reinterpret_cast<BallObject*>(objPtr);


						b2Vec2 center = object->RigidBody->GetPosition();

						auto mouseScreen = Camera::ToWorldSpace(Input::GetMousePosition());

						b2Vec2 mouse = { (float)mouseScreen.x, (float)mouseScreen.y };
						b2Vec2 distance = mouse - center;

						if (distance.Length() < obj->Radius) {
							obj->Radius *= 2.0f;

							b2Fixture* fixtureA = obj->RigidBody->GetFixtureList();
							obj->RigidBody->DestroyFixture(fixtureA);

							b2CircleShape circleShape;
							circleShape.m_radius = obj->Radius;

							b2FixtureDef fixtureDef;
							fixtureDef.shape = &circleShape;
							fixtureDef.density = 1;
							fixtureDef.friction = 0.3f;
							obj->RigidBody->CreateFixture(&fixtureDef);


						}
					}

				}
			}
			else {
				Objects.push_back(new BallObject(pos, 3, PINK));

			}
		}
		if (Input::MouseButtonPressed(SDL_BUTTON_RIGHT))
		{
			Objects.push_back(new BoxObject(pos, vf2d{ float(Helpers::GetRandomValue(10,20)),float(Helpers::GetRandomValue(10,20)) }, DARKRED, float(Helpers::GetRandomValue(0, 180))));
		}

		// update the world for the new frame
		accumulator += Window::GetFrameTime();
		while (accumulator >= physTime)
		{
			accumulator -= physTime;
			World.Step(physTime, velocityIterations, positionIterations);
		}


		// set our camera 
		//BeginMode2D(cam);

		// draw a grid


		// and some axes
		Render2D::DrawLine({0, 0}, {100, 0}, RED);
		Render2D::DrawLine({0, 0}, {0, -100}, BLUE);

		ground.draw();

		for (auto object : Objects)
			object->draw();

		//EndMode2D();

		Render2D::EndScissorMode();

		Fonts::DrawText("assets/fonts/APL386.ttf", 18, {maxWidth + 60.f, 10.f}, "Left Click to add a ball - Right Click to add a box",  WHITE);
	} break;
    case Test::Render2D:{

        Render2D::DrawLine({300.f, 20.f}, {350.f, 30.f}, WHITE);
        Render2D::DrawThickLine({400.f, 20.f}, {450.f, 30.f}, WHITE, 3.f);


        Render2D::DrawCircle({350.f, 100.f}, 40.f, WHITE);
        Render2D::DrawCircleFilled({500.f, 100.f}, 40.f, WHITE);

        Render2D::DrawEllipse({350.f, 200.f}, 40.f, 25.f, WHITE);
        Render2D::DrawEllipseFilled({500.f, 200.f}, 40.f, 25.f, WHITE);

        vf2d v1 = {40.f, 0.f};
        vf2d v2 = {0.f, 80.f};
        vf2d v3 = {80.f, 80.f};
        Render2D::DrawTriangle(v1 + vf2d(300.f, 300.f), v2 + vf2d(300.f, 300.f), v3 + vf2d(300.f, 300.f), WHITE);
        Render2D::DrawTriangleFilled(v1 + vf2d(450.5, 300.f), v2 + vf2d(450.5, 300.f), v3 + vf2d(450.5, 300.f), WHITE);

        Render2D::DrawRectangle({300.f, 400.f}, {80.f,80.f}, WHITE);
        Render2D::DrawRectangleFilled({450.f, 400.f}, {80.f,80.f}, WHITE);


    } break;
	case Test::Random:{

        rectf destRect = {
                600.f,
                200.f,
                32.f,
                32.f
        };

        rectf srcRect = {
                64.f,
                32.f,
                32.f,
                32.f
        };



        Render2D::DrawTexturePart(dungeonTileset, {destRect.x, destRect.y}, {destRect.width,destRect.height}, srcRect, WHITE);


        if (Input::KeyDown(SDLK_a)) {
            testPercentage -= 0.0001f;
        }

        if (Input::KeyDown(SDLK_d)) {
            testPercentage += 0.0001f;
        }

        testPercentage = std::clamp(testPercentage, 0.f, 1.f);

        UI::drawSmallBar({18.75f,9.375f}, {1 ,0}, 50, 10, {0,0,255,255}, testPercentage);

        Fonts::DrawText("assets/fonts/APL386.ttf", 20, {600.f, 250.f}, Helpers::TextFormat("%f", testPercentage), WHITE);





        if (Input::KeyPressed(SDLK_s)) {

            Font font = Fonts::GetFont("assets/fonts/Rosarivo-Regular.ttf", 144);

            std::vector<std::string> characters;

            for (char c = 'A'; c <= 'Z'; ++c) { characters.push_back(std::string(1, c)); }
            for (char c = 'a'; c <= 'z'; ++c) { characters.push_back(std::string(1, c)); }
            for (char c = '0'; c <= '9'; ++c) { characters.push_back(std::string(1, c)); }

            int c = 0;
            for (auto ch : characters) {

                std::string temp = std::to_string(c);

                SDL_Surface *textSurface = TTF_RenderUTF8_Blended(font.font, ch.c_str(), WHITE);

                Texture t;

                t.surface = textSurface;
                t.width = textSurface->w;
                t.height = textSurface->h;

                temp += ch + ".png";

                Textures::SaveTextureAsPNG(t, temp.c_str());

                SDL_DestroySurface(textSurface);
                c++;
            }
        }



    } break;
    case Test::J9X_Geom: {
        olc::vf2d vMouseDelta = olc::vf2d(Input::GetMousePosition().x, Input::GetMousePosition().y) - vOldMousePos;
		vOldMousePos = {(int)Input::GetMousePosition().x, (int)Input::GetMousePosition().y};

		if (Input::MouseButtonReleased(SDL_BUTTON_LEFT))
			nSelectedShapeIndex = -1;

		// Check for mouse hovered shapes
		ShapeWrap mouse{ Point{olc::vf2d(Input::GetMousePosition().x, Input::GetMousePosition().y)} };


		if (nSelectedShapeIndex < vecShapes.size() && Input::MouseButtonDown(SDL_BUTTON_LEFT))
		{
			// Visit the selected shape and offset.
			std::visit([&](auto& shape)
			{
				for (auto& p : shape.points)
				{
					p += vMouseDelta;
				}
			}, vecShapes[nSelectedShapeIndex]);
		}

		size_t nMouseIndex = 0;
		for (const auto& shape : vecShapes)
		{
			if (CheckContains(shape, mouse))
			{
				break;
			}

			nMouseIndex++;
		}

		if (nMouseIndex < vecShapes.size() && Input::MouseButtonPressed(SDL_BUTTON_LEFT))
			nSelectedShapeIndex = nMouseIndex;

		// Check Contains
		std::vector<size_t> vContains;
		std::vector<size_t> vOverlaps;
		std::vector<olc::vf2d> vIntersections;
		if (nSelectedShapeIndex < vecShapes.size())
		{
			for (size_t i = 0; i < vecShapes.size(); i++)
			{
				if (i == nSelectedShapeIndex) continue; // No self check

				const auto& vTargetShape = vecShapes[i];

				const auto vPoints = CheckIntersects(vecShapes[nSelectedShapeIndex], vTargetShape);
				vIntersections.insert(vIntersections.end(), vPoints.begin(), vPoints.end());

				if(CheckContains(vecShapes[nSelectedShapeIndex], vTargetShape))
					vContains.push_back(i);

				if (CheckOverlaps(vecShapes[nSelectedShapeIndex], vTargetShape))
					vOverlaps.push_back(i);
			}
		}


		ShapeWrap  ray1, ray2;



		bool bRayMode = false;
		if (Input::MouseButtonDown(SDL_BUTTON_RIGHT))
		{
			// Enable Ray Mode
			bRayMode = true;

			ray1 = { Ray{{ { 10.0f, 10.0f }, olc::vf2d(Input::GetMousePosition().x, Input::GetMousePosition().y)} }};
			ray2 = { Ray{{ { float(Window::GetWidth() - 10), 10.0f }, olc::vf2d(Input::GetMousePosition().x, Input::GetMousePosition().y)} }};


			for (size_t i = 0; i < vecShapes.size(); i++)
			{
				const auto& vTargetShape = vecShapes[i];

				const auto vPoints1 = CheckIntersects(ray1, vTargetShape);
				vIntersections.insert(vIntersections.end(), vPoints1.begin(), vPoints1.end());

				const auto vPoints2 = CheckIntersects(ray2, vTargetShape);
				vIntersections.insert(vIntersections.end(), vPoints2.begin(), vPoints2.end());

			}

			const auto vPoints3 = CheckIntersects(ray2, ray1);
			vIntersections.insert(vIntersections.end(), vPoints3.begin(), vPoints3.end());


		}

		// Draw All Shapes
		for (const auto& shape : vecShapes)
			DrawShape(shape);


		// Draw Overlaps
		for (const auto& shape_idx : vOverlaps)
			DrawShape(vecShapes[shape_idx], YELLOW);

		// Draw Contains
		for (const auto& shape_idx : vContains)
			DrawShape(vecShapes[shape_idx], PURPLE);

		// Draw Manipulated Shape
		if(nSelectedShapeIndex < vecShapes.size())
			DrawShape(vecShapes[nSelectedShapeIndex], GREEN);

		// Draw Intersections
		for (const auto& intersection : vIntersections)
			Render2D::DrawCircleFilled({intersection.x, intersection.y}, 3.f, RED);

		if (bRayMode)
		{
			DrawShape(ray1, BLUE);
			DrawShape(ray2, BLUE);
		}

		// Laser beam
		ray<float> ray_laser{ {10.0f, 300.0f}, {1.0f, 0.0f} };
		bool ray_stop = false;
		int nBounces = 100;
		size_t last_hit_index = -1;


		ray<float> ray_reflected;

		while (!ray_stop && nBounces > 0)
		{
			// Find closest
			ray_stop = true;
			int closest_hit_index = -1;
			float fClosestDistance = 10000000.0f;

			for (size_t i = 0; i < vecShapes.size(); i++)
			{
				// Dont check against origin shape
				if (i == last_hit_index) continue;

				const auto& vTargetShape = vecShapes[i];
				auto hit = CheckReflect(ray_laser, vTargetShape);
				if (hit.has_value())
				{
					float d = (ray_laser.origin - hit.value().origin).mag();
					if (d < fClosestDistance)
					{
						fClosestDistance = d;
						closest_hit_index = i;
						ray_reflected = hit.value();
					}
				}
			}

			if (closest_hit_index != -1)
			{
				Render2D::DrawLine({ray_laser.origin.x, ray_laser.origin.y} , {ray_reflected.origin.x, ray_reflected.origin.y} , Color(rand() % 155 + 100, 0, 0, 255));
				ray_laser = ray_reflected;
				ray_stop = false;
				last_hit_index = closest_hit_index;
				nBounces--;
			}

			if (ray_stop)
			{
				// Ray didnt hit anything
				nBounces = 0;
                auto target = ray_laser.origin + ray_laser.direction * 1000.0f;
				Render2D::DrawLine({ray_laser.origin.x, ray_laser.origin.y}, {target.x, target.y}, Color(rand() % 155 + 100, 0, 0, 255));
			}
		}

    } break;
	}
}

void TestState::customDrawGrid(int slices, float spacing)
{

}