#ifndef ECS_HPP
#define ECS_HPP
#include <vector>
#include <tuple>
#include <cstring>

/* Interface for PackedArray.
 * PackedArray is not ordered, but packed.
 * Indexing is done by an ID.
 * Removal from the array will move the last item to the removed slot.
 */

class IPackedArray{
public:
	// ID is the object ID. It is the key to the index array
	struct ID {
		uint16_t value;
	};

	// Slot is the internal key to the data array.
	struct Slot {
		uint16_t value;
	};

	// This is the max ID/Slot.
	static constexpr uint16_t MAX = ~0;
	// Reserve MAX to mean invalid ID or Slot.
	static constexpr uint16_t INVALID = MAX;

private:
	// Find the ID of the item at slot
	std::vector<uint16_t> ids;
protected:
	// Find the Slot of the item with the ID.
	// This array can have gaps. use INVALID to show that it is one.
	std::vector<uint16_t> index;
	// Append new item to the end
	virtual void push()=0;
	// remove the last item
	virtual void pop()=0;
	// Remove item at position and
	// overwrite with the last item
	virtual void remove(Slot)=0;
public:


	/* Lookup the id => slot map. */
	bool has(ID id) const
	{
		return id.value < index.size() &&
				index[id.value] != INVALID;
	}

	/* Get the length of the array.
	 * It simply gets the length of the ids vector, because
	 * it is logically part of the data array.
	 * However, your implementation's data array must also have the same
	 * length.
	 */
	std::size_t length() const
	{
		return ids.size();
	}

	/* Add a new id.
	 * Fails if id is already in.
	 * Returns true for success.
	 */
	bool add(ID id)
	{
		if(has(id) || id.value == INVALID)
			return false;

		// Push ID and data to the same slot
		this->push();
		ids.push_back(id.value);

		// Get the new slot number and put it in id2slot.
		// If id2slot is not large enough, resize it.
		// Also fill uninitialized values to INVALID.
		Slot slot;
		slot.value = ids.size() - 1;
		if(id.value >= index.size()) {
			index.resize(id.value + 1, INVALID);
		}
		index[id.value] = slot.value;

		return true;
	}


	/* Delete object by ID.
	 * The array must be packed afterwards.
	 * This must be done by moving the last item of the array
	 * to the deleted slot.
	 * A lookup of id => slot is required.
	 */
	bool del(ID id)
	{
		// First find the slot
		if(!has(id))
			return false;

		Slot slot = {index[id.value]};
		if(slot.value == ids.size() - 1) {
			// pop if the slot is the last one
			ids.pop_back();
			this->pop();

			// invalidate id by setting slot for id to invalid
			index[id.value] = INVALID;
		} else {
			// replace the id at slot with the id at the last slot
			ids[slot.value] = ids.back();
			ids.pop_back();

			// this->remove should have the same behaviour as above
			this->remove(slot);

			// fix id=>slot mapping
			ID movedID = {ids[slot.value]};
			index[movedID.value] = slot.value;
		}
		return true;
	}

	/* Get id by slot.
	 * It is assumed that the input slot number is not out of range.
	 */
	ID idAtSlot(Slot slot)
	{
		return ID{ids[slot.value]};
	}

	/* Get slot by id.
	 * It is assumed that the input id is not out of range.
	 */
	Slot slotAtID(ID id)
	{
		return Slot{index[id.value]};
	}
};

// Basic implementation of PackedArray.
template<class T>
class PackedVector : public IPackedArray {
	typedef typename IPackedArray::Slot Slot;
	typedef typename IPackedArray::ID ID;
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
		data[i.value] = std::move(data.back());
		data.pop_back();
	}
public:
	/* Get the item at slot.
	 * It is assumed that the slot number is not out of range.
	 */
	T& itemAtSlot(Slot slot)
	{
		return data[slot.value];
	}

	/* Get the item with the ID.
	 * Returns null if item is not found.
	 */
	T* itemAtID(ID id)
	{
		if(has(id)) {
			T& ref = itemAtSlot(slotAtID(id));
			return &ref;
		} else {
			return nullptr;
		}
	}

};

template<class T, std::size_t I=0>
void tuple_emplace_back(T& tup)
{
	if(I < std::tuple_size<T>::value) {
		std::get<I>(tup).emplace_back();
		tuple_emplace_back<I+1>(tup);
	}
}

template<class T, std::size_t I=0>
void tuple_pop_back(T& tup)
{
	if(I < std::tuple_size<T>::value) {
		std::get<I>(tup).pop_back();
		tuple_pop_back<I+1>(tup);
	}
}

template<class T, std::size_t I=0>
void tuple_steal_last(T& tup, unsigned i)
{
	if(I < std::tuple_size<T>::value) {
		std::get<I>(tup)[i] = std::get<I>(tup).back();
		tuple_steal_last<I+1>(tup, i);
	}
}


template<class ... Ts>
class PackedVectorTuple: public IPackedArray {
	typedef typename IPackedArray::Slot Slot;
	typedef typename IPackedArray::ID ID;
	std::tuple<std::vector<Ts>...> data;
protected:
	virtual void push()
	{
		tuple_emplace_back(data);
	}
	virtual void pop()
	{
		tuple_pop_back(data);
	}
	virtual void remove(Slot i)
	{
		tuple_steal_last(data, i.value);
		tuple_pop_back(data);
	}
public:
	/* These functions are used for accessing elements of the
	 * vector at position i in the tuple
	 */
	template<std::size_t i>
	typename std::tuple_element<i, std::tuple<Ts...>>::type&
	itemAtSlot(Slot slot)
	{
		return std::get<i>(data)[slot.value];
	}

	template<std::size_t i>
	typename std::tuple_element<i, std::tuple<Ts...>>::type*
	itemAtID(ID id)
	{
		return &(itemAtSlot<i>(slotAtID(id)));
	}
};

class ISystem {
	typedef typename IPackedArray::ID ID;
public:
	// Update all components of this system
	virtual void update(double dt) = 0;

	// Get the component array
	virtual IPackedArray& getComponents() = 0;

	// Get the name of the system
	virtual char const * getName() const = 0;

	// Remove object by ID
	void del(ID id)
	{
		getComponents().del(id);
	}

	// Custom action to invalidate ID referenced by this system.
	virtual void invalidate(ID id)
	{
	}
};

// Collection of entities.
// Stores entities as set of components.
// An entity is an ID and all the components that has the same ID.
class Entities{
	typedef typename IPackedArray::ID ID;

	// ID generator data.
	// Prioritize getting ID from the free list.
	// Increment when free list is empty.
	ID nextID;
	std::vector<ID> freedID;

	/* The collection of systems.
	 * Each system has a unique name.
	 */
	std::vector<ISystem* > systems;

public:
	Entities()
		:nextID{0}
	{

	}

	/* Attempt to add a new system.
	 * Fails when an system of the same name is already in the collection.
	 * Return false if it fails.
	 */
	bool addSystem(ISystem& newSystem)
	{
		for(auto sys : systems) {
			if(strcmp(sys->getName(), newSystem.getName()) == 0) {
				return false;
			}
		}
		systems.push_back(&newSystem);
	}

	/* Finds the first system that matches name.
	 * Returns null if none is found.
	 */
	ISystem* findSystem(char const * name)
	{
		for(auto sys : systems) {
			if(strcmp(sys->getName(), name) == 0) {
				return sys;
			}
		}
		return nullptr;
	}

	// Allocate new ID
	ID allocate()
	{
		if(freedID.empty()) {
			ID newID = {nextID.value};
			nextID.value++;
			return newID;
		} else {
			ID recycled = freedID.back();
			freedID.pop_back();
			return recycled;
		}
	}

	// Deallocate ID.
	// Also tells all systems to remove the component with the same ID.
	void deallocate(ID id)
	{
		if(id.value < nextID.value) {
			freedID.push_back(id);
			for(auto & sys : systems) {
				sys->del(id);
				sys->invalidate(id);
			}
		}
	}
};

#endif // ECS_HPP

