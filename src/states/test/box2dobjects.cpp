#include "box2dobjects.h"

BoxObject::BoxObject(vf2d pos, vf2d size, Color c, float angle, bool isDynamic)
{
#ifndef M_PI
#define M_PI 3.1415629
#endif

    const float DEG2RAD = static_cast<float>(M_PI / 180.0);

    ShapeColor = c;
    Size = size;
    HalfSize.x = size.x / 2;
    HalfSize.y = size.y / 2;

    b2BodyDef bodyDef;
    bodyDef.type = isDynamic ? b2_dynamicBody : b2_staticBody;
    bodyDef.position.Set(pos.x, pos.y);
    bodyDef.angle = angle * DEG2RAD;
    bodyDef.userData.pointer = 0;
    RigidBody = World.CreateBody(&bodyDef);

    Box.SetAsBox(Size.x / 2, Size.y / 2);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &Box;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;

    RigidBody->CreateFixture(&fixtureDef);
}

void BoxObject::draw()
{
    //TODO: implement rotated rectangle
    //const float RAD2DEG = static_cast<float>(180.0 / M_PI);
    //Render2D::DrawRectangleFilledRotated(Rectangle{ RigidBody->GetPosition().x,RigidBody->GetPosition().y, Size.x, Size.y }, HalfSize, RigidBody->GetAngle() * RAD2DEG, ShapeColor);
}

BallObject::BallObject(vf2d pos, float radius, Color c)
    {
        Radius = radius;
        ShapeColor = c;

        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(pos.x, pos.y);
        bodyDef.userData.pointer = uintptr_t(this);
        RigidBody = World.CreateBody(&bodyDef);

        CircleShape.m_radius = radius;

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &CircleShape;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.3f;

        RigidBody->CreateFixture(&fixtureDef);
    }

void BallObject::draw()
{
    //DrawCircleV(vf2d{ RigidBody->GetPosition().x,RigidBody->GetPosition().y }, Radius, ShapeColor);
}
