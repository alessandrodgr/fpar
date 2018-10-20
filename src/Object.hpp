#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <variant>
#include <immer/box.hpp>
#include <immer/flex_vector.hpp>

namespace fp {

  template <typename... Ts>
  class Object {
  private:
    std::variant<std::monostate,
                 bool,
                 size_t,
                 Ts...,
                 immer::flex_vector<immer::box<Object<Ts...>>>> _obj;

  public:
    Object() {}

    template <typename T>
    Object(const T& obj) : _obj(obj) {}

    template <typename T>
    operator T () const {
      return std::get<T>(_obj);
    }

    constexpr bool isBottom () const noexcept {
      return std::holds_alternative<std::monostate>(_obj);
    }

    constexpr bool isSequence () const noexcept {
      return std::holds_alternative<immer::flex_vector<immer::box<Object<Ts...>>>>(_obj);
    }

    template <typename T>
    constexpr bool is () const noexcept {
      return std::holds_alternative<T>(_obj);
    }
  };

  constexpr auto Bottom = std::monostate();

  template <typename T>
  using Sequence = immer::flex_vector<immer::box<T>>;
}

#endif
