#ifndef OBJECT_HPP
#define OBJECT_HPP

/** \file Object.hpp
 * Definizione del type system
 * Definizione del type system di un sistema FP like.
 */

#include <variant>
#include <immer/box.hpp>
#include <immer/flex_vector.hpp>

namespace fp {

  /*! \class Object
   *  \brief Tipo degli oggetti di un sistema FP like.
   *         E' un tipo generico e ricorsivo che comprende:
   *          - bottom
   *          - tipi di base specifici: bool e size_t
   *          - tipi di base generici
   *          - tipo delle sequenze
   *
   */
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

  /*
    Variabile globale costante che assume il tipo Bottom.
    Utile se lo si vuole ritornare da una funzione:
      return Bottom; invece di return std::monostate();
      ==> più leggibile
  */
  constexpr auto Bottom = std::monostate();

  /*
    Type alias per le sequenze, anche queste generiche
  */
  template <typename T>
  using Sequence = immer::flex_vector<immer::box<T>>;
}

#endif
