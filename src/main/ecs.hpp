#ifndef ECS_HPP
#define ECS_HPP
#include <vector>
#include <tuple>
#include <unordered_map>

/* Interface for PackedArray.
 * PackedArray is not ordered, but packed.
 * Indexing is done by an ID.
 * Removal from the array will move the last item to the removed slot.
 */
template<class IDTYPE>
class IPackedArray{
public:
	// ID is the object ID. It is the key to the index array
	struct ID {
		IDTYPE value;
	};

	// Slot is the internal key to the data array.
	struct Slot {
		IDTYPE value;
	};

	// This is the max ID/Slot.
	static constexpr IDTYPE MAX = ~0;
	// Reserve MAX to mean invalid ID or Slot.
	static constexpr IDTYPE INVALID = MAX;

private:
	// Find the ID of the item at slot
	std::vector<IDTYPE> ids;
protected:
	// Find the Slot of the item with the ID.
	// This array can have gaps. use INVALID to show that it is one.
	std::vector<IDTYPE> index;
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
		IDTYPE slot = ids.size() - 1;
		if(id.value >= index.size()) {
			index.resize(id.value + 1, INVALID);
		}
		index[id.value] = slot;

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

	/* This is for components that refer to IDs.
	 * When an ID is deallocated, this is called for
	 * every comoponent array.
	 */
	virtual void invalidate(ID id)
	{
	}

};

// Basic implementation of PackedArray.
template<class IDTYPE, class T>
class PackedVector : public IPackedArray<IDTYPE> {
	typedef typename IPackedArray<IDTYPE>::Slot Slot;
	typedef typename IPackedArray<IDTYPE>::ID ID;
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

	T& itemAtID(ID id)
	{
		return itemAtSlot(slotAtID(id));
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


template<class IDTYPE, class ... Ts>
class PackedVectorTuple: public IPackedArray<IDTYPE> {
	typedef typename IPackedArray<IDTYPE>::Slot Slot;
	typedef typename IPackedArray<IDTYPE>::ID ID;
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
	typename std::tuple_element<i, std::tuple<Ts...>>::type&
	itemAtID(ID id)
	{
		return itemAtSlot<i>(slotAtID(id));
	}
};

// Collection of entities.
// Stores entities as set of components.
// An entity is an ID and all the components that has the same ID.
template<class IDTYPE>
class Entities{
	typedef typename IPackedArray<IDTYPE>::ID ID;

	// ID generator data.
	// Prioritize getting ID from the free list.
	// Increment when free list is empty.
	ID nextID;
	std::vector<ID> freedID;

	/* The collection of component arrays.
	 * Each array is referred to by a unique name.
	 * The container only keeps a reference to it.
	 */
	std::unordered_map<std::string, IPackedArray<IDTYPE>* > components;
public:
	Entities()
		:nextID{0}
	{

	}

	/* Attempt to add a new component array.
	 * Fails when an array of the same name is already in the table.
	 * Return false if it fails.
	 */
	bool registerComponent(std::string name, IPackedArray<IDTYPE>* comp)
	{
		auto result = components.emplace(name, comp);
		return result.second;
	}

	// Allocate new ID
	ID allocate()
	{
		if(freedID.empty()) {
			return ID{nextID.value++};
		} else {
			ID recycled = freedID.back();
			freedID.pop_back();
			return recycled;
		}
	}

	// Deallocate ID
	void deallocate(ID id)
	{
		if(id < nextID) {
			freedID.push_back(id);
			for(auto & it : components) {
				it.second->del(id);
				it.second->invalidate(id);
			}
		}
	}
};

class ISystem {
public:
	virtual void update(double dt) = 0;
};

#endif // ECS_HPP

