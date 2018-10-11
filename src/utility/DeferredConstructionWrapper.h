#pragma once

#include <tuple>

namespace openmittsu {
	namespace utility {
		// https://stackoverflow.com/questions/41439848/stdforward-as-tuple-to-pass-arguments-to-2-constructors#41439982
		namespace detail {
			template <class T, class Tuple, std::size_t... I>
			constexpr T make_from_tuple_impl(Tuple&& t, std::index_sequence<I...>) {
				return T(std::get<I>(std::forward<Tuple>(t))...);
			}
		} // namespace detail

		template <class T, class Tuple>
		constexpr T make_from_tuple(Tuple&& t) {
			return detail::make_from_tuple_impl<T>(std::forward<Tuple>(t), std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
		}


		template<typename C, typename ... args>
		struct DeferredConstructionWrapper {
			std::tuple<args...> data;
			DeferredConstructionWrapper(args... a) : data(std::forward<args>(a)...) {}

			C construct() {
				return make_from_tuple<C>(data);
			}
		};

		template<typename C, typename ... args>
		auto make_deferred(args&& ... a) {
			return DeferredConstructionWrapper<C, args...>(std::forward<args>(a)...);
		}
	}
}
