#ifndef PHYSICS_HPP
#define PHYSICS_HPP
#include "ecs.hpp"
#include "math/Vec2.h"
#include "math/Rect.h"
#include "math/Circ.h"
#include <unordered_set>

// A struct representing the location and orientation of
// an object.
struct Space{
	Vec2 center;
	float angle;
};

// Used for circles.
struct Radius {
	float value;
};
// A struct representing the collision of 2 objects
struct CollisionRecord {
	IPackedArray::ID id1, id2;
};

class Entities;
class PhysicsSystem{
public:
	// Objects stored as a tuple of vectors.
	PackedVectorTuple<Space, Space, Space, Radius> data;
	// Constants to access a specific vector in the tuple.
	static constexpr char * name = "physics";
	static constexpr unsigned SPACE = 0; // Position and direction
	static constexpr unsigned SPACE1 = 1; // First derivative of space(velocity)
	static constexpr unsigned SPACE2 = 2; // Second derivative of space(acceleration)
	static constexpr unsigned RADIUS = 3; // Radius of object, used for collision.


	// Store the collision pairs in the current frame.
	// This is changed every frame.
	std::unordered_set<CollisionRecord> collisions;

	PhysicsSystem(Entities & ent);
	void update(double dt);

};


#endif // PHYSICS_HPP

