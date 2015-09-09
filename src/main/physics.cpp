#include "physics.hpp"
#include "world.h"
#include <unordered_map>
#include <unordered_set>
template<class T>
struct SpatialHash {
	// Each block map to a bucket fo things.
	std::unordered_map<Rect, std::vector<T>> buckets;

	// Dimensions of each block
	float const blockWidth;
	float const blockHeight;

	SpatialHash(float width, float height)
		:blockWidth(width), blockHeight(height)
	{
	}

	void add(Rect const & r, T const & t)
	{
		// First create a bounding rect that encloses r.
		// This bounding rect has is aligned to blockWidth and blockHeight.
		Rect bound = boundingBlock(r, blockWidth, blockHeight);

		// Create a block for iteration.
		Rect block(Vec2(bound.left, bound.bottom), Size(blockWidth, blockHeight));

		// Check block against r. If they intersect, r belongs in block.
		while(intersectX(block, r)) {
			while(intersectY(block, r)) {
				buckets[block].push_back(t);
				block += Vec(0, blockHeight);
			}
			block.setOrigin(Vec2(block.left + blockWidth, bound.bottom));
		}
	}
};

PhysicsSystem::PhysicsSystem(Entities &ent)
	:entities(ent)
{
	bool good = ent.registerSystem(*this);
	if(!good) {
		throw std::string("failed to register system \"") + name + '\"';
	}
}

void PhysicsSystem::update(double dt)
{
	struct ShapeRecord {
		ID id;
		Circ shape;
	};


	// move every object and put it in spatial hash table
	SpatialHash<ShapeRecord> spatialHash(160, 160);
	for(Slot slot{0}; slot.value < data.length(); slot.value++) {
		Space & space = data.itemAtSlot<SPACE>(slot);
		Space & space1 = data.itemAtSlot<SPACE1>(slot);
		Space & space2 = data.itemAtSlot<SPACE2>(slot);
		Radius & radius = data.itemAtSlot<RADIUS>(slot);

		space1.center += dt * space2.center;
		space1.angle += dt * space2.angle;

		space.center += dt * space1.center;
		space.angle += dt * space1.angle;

		Rect box(Vec(), Size(radius * 2, radius * 2));
		box.setCenter(space.center);
		ShapeRecord record = {
			data.idAtSlot(slot),
			Circ(space.center, radius.value)
		};
		spatialHash.add(box, record);
	}

	// check collision for every bucket in spatial hash table.
	// Store colliding pairs into a set.
	collisions.clear();
	for(auto & p : spatialHash.buckets) {
		auto & bucket = p.second;
		for(auto it1 = bucket.begin(); it1 != bucket.end(); ++it1) {
			for(auto it2 = it1 + 1; it2 != bucket.end(); ++it2) {
				if(intersect(it1->shape, it2->shape)) {
					CollisionRecord record{it1->id, it2->id};
					if(record.id1.value > record.id2.value) {
						std::swap(record.id1, record.id2);
					}
					collisions.insert(record);
				}
			}
		}
	}

}

/*
void ship_constraint(World& w)
{
	ShipStats const * stats = e.shipstats[i];

	float speed = e.velocity[i].norm();
	if(speed > stats->max_speed)
		e.velocity[i] *= (stats->max_speed / speed);
	else if(speed < stats.min_speed)
		e.velocity[i] *= (stats->min_speed / speed);

	float speed_ang = e.speed_ang[i];
	if(speed_ang > stats->max_speed_ang)
		e.speed_ang[i] = stats->max_speed_ang;
	else if(speed_ang < stats->min_speed_ang)
		e.speed_ang[i] = stats->min_speed_ang;
}
*/
