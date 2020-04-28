/*******************************
Copyright (c) 2016-2020 Gr�goire Angerand

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
**********************************/
#ifndef Y_ECS_COMPONENTVIEW_H
#define Y_ECS_COMPONENTVIEW_H

#include "ecs.h"

#include <y/core/Range.h>

#include <tuple>
#include <type_traits>

namespace y {
namespace ecs {

template<bool, typename...>
class ComponentIterator;

class ComponentEndIterator {
	public:
		using difference_type = usize;

		ComponentEndIterator(usize size = 0) : _index(size) {
		}

		template<bool T, typename... Args>
		difference_type operator-(const ComponentIterator<T, Args...>& other) const;

		template<bool T, typename... Args>
		bool operator==(const ComponentIterator<T, Args...>& other) const;

		template<bool T, typename... Args>
		bool operator!=(const ComponentIterator<T, Args...>& other) const;

	private:
		template<bool, typename...>
		friend class ComponentIterator;

		usize _index = 0;
};

template<bool Tuple, typename... Args>
class ComponentIterator {

	template<usize I, typename... A>
	static constexpr bool is_compatible() {
		if constexpr(I < sizeof...(A)) {
			using this_t = std::tuple_element_t<I, std::tuple<Args...>>;
			using other_t = std::tuple_element_t<I, std::tuple<A...>>;
			if constexpr(!std::is_same_v<this_t, other_t> && !std::is_same_v<this_t, const other_t>) {
				return false;
			}
			return is_compatible<I + 1, A...>();
		}
		return true;
	}

	using reference_tuple = std::tuple<Args&...>;

	public:
		static constexpr usize component_count = sizeof...(Args);

		using difference_type = usize;
		using iterator_category = std::random_access_iterator_tag;

		using reference = std::conditional_t<Tuple, reference_tuple, std::tuple_element_t<0, reference_tuple>>;
		using value_type = reference;
		using pointer = std::remove_reference_t<value_type>*;

		static_assert(!Tuple || component_count == 1);


		ComponentIterator() = default;
		ComponentIterator(const ComponentIterator&) = default;

		template<bool T, typename... A, typename = std::enable_if_t<is_compatible<0, A...>()>>
		ComponentIterator(const ComponentIterator<T, A...>& other) : _index(other._index), _chunks(other._chunks), _offsets(other._offsets) {
		}

		reference operator*() const {
			if constexpr(Tuple) {
				return make_refence_tuple<0>();
			} else {
				return std::get<0>(make_refence_tuple<0>());
			}
		}

		bool operator==(const ComponentEndIterator& other) const {
			return _index == other._index;
		}

		bool operator!=(const ComponentEndIterator& other) const {
			return !operator==(other);
		}

		bool operator==(const ComponentIterator& other) const {
			return _index == other._index && _chunks == other._chunks;
		}

		bool operator!=(const ComponentIterator& other) const {
			return !operator==(other);
		}

		ComponentIterator& operator++() {
			++_index;
			return *this;
		}

		ComponentIterator& operator--() {
			--_index;
			return *this;
		}

		ComponentIterator operator++(int) {
			const auto it = *this;
			++*this;
			return it;
		}

		ComponentIterator operator--(int) {
			const auto it = *this;
			--*this;
			return it;
		}

		difference_type operator-(const ComponentIterator& other) const {
			return _index - other._index;
		}

		ComponentIterator& operator+=(usize n) {
			_index += n;
			return *this;
		}

		ComponentIterator& operator-=(usize n) {
			_index -= n;
			return *this;
		}


		ComponentIterator operator+(usize n) const {
			auto it = *this;
			it += n;
			return it;
		}

		ComponentIterator operator-(usize n) const {
			auto it = *this;
			it -= n;
			return it;
		}

	private:
		friend class Archetype;
		friend class ComponentEndIterator;

		template<bool, typename...>
		friend class ComponentIterator;

		template<usize I = 0>
		auto make_refence_tuple() const {
			y_debug_assert(_chunks || !sizeof...(Args));
			const usize chunk_index = _index / entities_per_chunk;
			const usize item_index = _index % entities_per_chunk;
			using type = std::remove_reference_t<std::tuple_element_t<I, reference_tuple>>;

			void* offset_chunk = static_cast<u8*>(_chunks[chunk_index]) + _offsets[I];
			type* chunk = static_cast<type*>(offset_chunk);
			if constexpr(I + 1 == component_count) {
				return std::tie(chunk[item_index]);
			} else {
				return std::tuple_cat(std::tie(chunk[item_index]),
				                      make_refence_tuple<I + 1>());
			}
		}

		usize _index = 0;
		void** _chunks = nullptr;
		std::array<usize, component_count> _offsets;
};

static_assert(std::is_constructible_v<ComponentIterator<true, const int>, ComponentIterator<false, int>>);
static_assert(!std::is_constructible_v<ComponentIterator<true, int>, ComponentIterator<true, const int>>);


template<bool T, typename... Args>
ComponentEndIterator::difference_type ComponentEndIterator::operator-(const ComponentIterator<T, Args...>& other) const {
	return _index - other._index;
}

template<bool T, typename... Args>
bool ComponentEndIterator::operator==(const ComponentIterator<T, Args...>& other) const {
	return other == *this;
}

template<bool T, typename... Args>
bool ComponentEndIterator::operator!=(const ComponentIterator<T, Args...>& other) const {
	return other != *this;
}


template<typename T>
using SingleComponentViewRange = core::Range<ComponentIterator<false, T>, ComponentEndIterator>;

template<typename... Args>
using ComponentViewRange = core::Range<ComponentIterator<true, Args...>, ComponentEndIterator>;


template<typename T>
struct SingleComponentView : SingleComponentViewRange<T> {
	SingleComponentView() : SingleComponentView(ComponentIterator<false, T>({}), 0) {
	}

	SingleComponentView(ComponentIterator<false, T> beg, usize size) : SingleComponentViewRange<T>(std::move(beg), ComponentEndIterator(size)) {
	}
};

template<typename... Args>
struct ComponentView : ComponentViewRange<Args...> {
	ComponentView() : ComponentView(ComponentIterator<true, Args...>({}), 0) {
	}

	ComponentView(ComponentIterator<true, Args...> beg, usize size) : ComponentViewRange<Args...>(std::move(beg), ComponentEndIterator(size)) {
	}
};

}
}

#endif // Y_ECS_COMPONENTVIEW_H
