#ifndef WORLD_H
#define WORLD_H
#include <array>
#include <bitset>
#include <unordered_map>
#include <set>
#include <queue>
#include <string>
#include <memory>
#include "components.h"
#include "stats.h"



template<class ID=uint16_t, class Slot=uint16_t>
class IPackedArray{
	std::vector<ID> slot2id; // ID of component
	std::unordered_map<ID, Slot> id2slot; // Component of ID
public:
	typedef ID IDType;
	typedef Slot SlotType;

	/* Ref describes the position of an item within the packed array.
	 * The ID is there for integrity check.
	 * The Slot is the position of the item.
	 */
	class Ref{
		ID id;
		Slot slot;
	};
	static constexpr Ref invalid_ref{-1, -1};

	/* Check integrity of ref.
	 * Iff the slot is within the array range,
	 * and the id matches, then it is valid.
	 * Return true for valid.
	 */
	bool checkRef(Ref ref)
	{
		return ref.slot < ids.size() &&
				slot2id[ref.slot] == ref.id;
	}
	/* Lookup the id => slot map. */
	bool has(ID id)
	{
		auto it = id2slot.find(id);
		return it != id2slot.end();
	}

	/* Find item by id.
	 * A lookup of id => slot is required, so performance
	 * isn't so good.
	 * Returns a Ref to the item.
	 */
	Ref find(ID id)
	{
		auto it = id2slot.find(id);
		if(it != id2slot.end()) {
			Ref ref;
			ref.id = id;
			ref.slot = it->second;
			return ref;
		} else {
			return invalid_ref;
		}
	}

	/* Add a new id.
	 * Returns a Ref to the new item.
	 */
	Ref add(ID id)
	{
		if(has(id))
			return invalid_ref;

		this->push();
		slot2id.push_back(id);

		Slot slot = slot2id.size() - 1;
		id2slot.emplace(id, slot);

		Ref ref;
		ref.id = id;
		ref.slot = slot;
		return ref;
	}


	/* Delete object by ID.
	 * The array must be packed afterwards.
	 * This must be done by moving the last item of the array
	 * to the deleted slot.
	 * A lookup of id => slot is required.
	 * Ref pointing to the last item and the
	 * deleted item will become invalid.
	 */
	void del(ID id)
	{
		// First find the slot
		auto it = id2slot.find(id);
		if(it == id2slot.end())
			return;
		Slot slot = it->second;

		if(slot == slot2id.size() - 1) {
			// pop if the slot is the last one
			this->pop();
			slot2id.pop_back();
			id2slot.erase(it);
		} else {
			// replace the id and slot with the id at the last slot
			slot2id[slot] = slot2id.back();
			slot2id.pop_back();

			// this->remove should have the same behaviour as above
			this->remove(slot);

			// fix id=>slot mapping
			id2slot.erase(it);
			id2slot[slot2id[slot]] = slot;
		}

	}

protected:
	// Append new item to the end
	virtual void push()=0;
	// remove the last item
	virtual void pop()=0;
	// Remove item at position and
	// overwrite with the last item
	virtual void remove(Slot)=0;
	// Name of the array type.
};

template<class T>
class PackedVector : PackedArray {
	std::vector<T> data;
protected:
	virtual void push()
	{
		data.emplace_back();
	}
	virtual void pop()
	{
		data.pop_back();
	}
	virtual void remove(Slot i)
	{
		data[i] = std::move(data.back());
		data.pop_back();
	}
};

class Entities{
	// An incrementing unique id
	IPackedArray::IDType id;

	// The set of IDs in use.
	// Useful for avoiding ID conflicts.
	std::set<IPackedArray::IDType> idInUse;

	// The collection of component arrays.
	// Each array is referred to by a name.
	// Make sure this name is unique for different arrays.
	// The container only keeps a reference to it.
	std::unordered_map<std::string, IPackedArray*> components;
public:
	Entities()
		:id(0)
	{

	}

	// Attempt to add a new component array.
	// Fails when an array of the same name is already in the table.
	// Return false if it fails.
	bool registerComponent(std::string name, IPackedArray* comp)
	{
		auto result = components.emplace(name, comp);
		return result.second;
	}


};



struct State;
// world keeps all the game objects
struct World{
	std::unordered_map<std::string, Stat> stats;

	Ships<256> ships;



	void update(State & s, double dt);
};

#endif
