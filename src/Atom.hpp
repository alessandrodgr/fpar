#ifndef ATOM_HPP
#define ATOM_HPP

/** \file Atom.hpp
 * Definizione del tipo Atom
 * Definizione del tipo polimorfo Atom di un sistema FP like
 */

#include "Object.hpp"

#include <memory>
#include <iostream>

namespace fp {

  /*! \class Atom
   *  \brief Tipo polimorfo degli atomi
   *
   */
  template <typename T>
  class Atom : public Object {
  private:
    T _data;  /*!< valore incapsulato dall'atomo */

    Atom (const T& data) : _data(data) {
      _type = ATOM;
    }

  public:
    /*! \brief Factory method Atomo
     *  \param value Valore incapsulato dall'atomo
     *  \return Puntatore all'atomo creato
     */
    inline static std::shared_ptr<Atom<T>>
    make_atom (const T& value) {
      return std::make_unique<Atom<T>>( Atom<T>(value) );
    }

    /*! \brief Metdodo per la conversione ad atomo di un oggetto
     *  \param obj Oggetto da convertire
     *  \return Puntatore all'atomo ottenuto dalla conversione (significativo
     *          solo se obj è effettivamente un atomo)
     */
    inline static std::shared_ptr<Atom<T>>
    to_atom (const std::shared_ptr<Object> obj) {
      return std::static_pointer_cast<Atom<T>>(obj); // cast statico!
    }

    /*! \brief Conversion operator da Atom<T> a T
     *  \return Esplicita il valore incapsulato dall'atomo
     */
    operator T () const {
      return _data;
    }
  };

  template <typename T>
  using AtomPtr = std::shared_ptr<Atom<T>>;

}

#endif
