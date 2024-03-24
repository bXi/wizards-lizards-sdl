#include "debugrigidbody2d.h"

#include "rigidbody2d.h"
#include "configuration/configuration.h"
#include "utils/vectors.h"

#include "render2d/render2dhandler.h"

void DebugRigidBody2D::draw(flecs::entity entity)
{

	if (entity.has<RigidBody2D>()) {

//		auto* rigidBody = entity.get_mut<RigidBody2D>()->RigidBody;
//
//
//
//
//    // Iterate over the fixtures associated with the body
//    for (b2Fixture* fixture = rigidBody->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
//
//        if (fixture->GetType() == b2Shape::e_polygon) {
//            // Cast the shape to b2PolygonShape
//            b2PolygonShape* polygonShape = static_cast<b2PolygonShape*>(fixture->GetShape());
//
//            // Get the size (assuming it's a box, so it has 4 vertices)
//            b2Vec2 size = polygonShape->m_vertices[2] - polygonShape->m_vertices[0];
//
//            // Get the position of the fixture
//            b2Vec2 position = fixture->GetAABB(0).GetCenter();
//
//            position.x -= size.x / 2.f;
//            position.y -= size.y / 2.f;
//
//            Render2D::DrawRectangleFilled(vi2d(position) * vi2d(Configuration::tileWidth, Configuration::tileHeight), vi2d(size) * vi2d(Configuration::tileWidth, Configuration::tileHeight), color);
//
//        }
//    }
//


//        Render2D::DrawTexturePart(sprite->sprite, {position.x, position.y}, {position.width, position.height},source);

	}
}
