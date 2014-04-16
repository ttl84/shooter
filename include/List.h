// maps a constant value to a variable
namespace mapping{
	template<class key_t, class val_t, key_t k>
	struct Mapping{
		static constexpr key_t key = k;
		val_t val;
	};
}


// it's like a linked list
namespace list{
	template <class...Ts>
	struct List{
		static constexpr unsigned length = 0;
	};

	template <class T, class ... Ts>
	struct List<T, Ts...>{
		T head;
		List<Ts...> tail;
		static constexpr unsigned length = decltype(tail)::length + 1;
	};


	// return a ref to the first item with type T
	template<class T, class ... Ts>
	T& find(List<T, Ts...> & list)
	{
		return list.head;
	}
	template<class T, class ... Ts>
	T& find(List<Ts...> & list)
	{
		return find<T>(list.tail);
	}
}

#include <vector>
namespace ecs2{
	enum struct Component : unsigned{
		POSITION,
		VELOCITY,
		ACCELERATION,
		GUN,
		LAST
	};
	
	template<Component i, class val_t>
	struct Decl : public mapping::Mapping<Component, val_t, i>{};
		

	template<Component i, class T>
	struct getDeclType;
	
	template<Component i, class val_t, class...Ts>
	struct getDeclType<i, list::List<Decl<i, val_t>, Ts...>> {
		typedef val_t ValueType;
	};
	template<Component i, class T, class ... Ts>
	struct getDeclType<i, list::List<T, Ts...>> : getDeclType<i, list::List<Ts...>>{};
	

	// get reference to item mapped to a component
	template<Component i, class...Ts>
	auto ref(list::List<Ts...> & list) -> typename getDeclType<i, list::List<Ts...>>::ValueType&
	{
		typedef typename getDeclType<i, list::List<Ts...>>::ValueType val_t;
		typedef Decl<i, val_t> decl_t;
		return list::find< decl_t >(list).val;
	}
	

	
	// apply reserve to all vectors in the decl list
	void emplace_back(list::List< > & l)
	{
		static_assert(l.length == 0, "list length should be zero");
	}
	template<Component cpn, class T, class... Ts>
	void emplace_back(list::List< Decl< cpn, std::vector<T> >, Ts...> & l)
	{
		l.head.val.emplace_back();
		emplace_back(l.tail);
	}
	class World{
	private:
		std::stack<unsigned> holes;
	public:
		std::vector<uint16_t> masks;
		list::List<
			Decl<Component::POSITION, std::vector<Vec2> >,
			Decl<Component::VELOCITY, std::vector<Vec2>>,
			Decl<Component::ACCELERATION, std::vector<Vec2>>
		> cpn;
		unsigned claim()
		{
			unsigned next;
			if(holes.empty())
			{
				next = masks.size();
				masks.push_back(0); // advance one slot for mask vector
				emplace_back(cpn); // advance one slot for all component vectors
			}
			else
			{
				next = holes.top();
				holes.pop();
			}
			return next;
		}
		void addComponent(unsigned i, Component c)
		{
			unsigned old = masks.at(i);
			uint16_t m = 1 << (unsigned)c;
			masks.at(i) = old | m;
		}
	};
	
	
	static inline void test()
	{
		World world;
		
		ref<Component::POSITION>(world.cpn).push_back(Vec2(1.2, 3.4));
		ref<Component::VELOCITY>(world.cpn).push_back(Vec2(1.2, 3.4));
		ref<Component::ACCELERATION>(world.cpn).push_back(Vec2(1.2, 3.4));
		
		
	}
}