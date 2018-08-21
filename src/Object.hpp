#ifndef OBJECT_HPP
#define OBJECT_HPP

/** \file Object.hpp
 * Definizione dell'interfaccia Object
 * Definizione di un'interfaccia comune per un sistema di tipi FP like
 */

#include <memory>

namespace fp {

  /*! Enumerazione dei tipi */
  enum object_type {
    ATOM,
    SEQUENCE
  };

  /*! \class Object
   *  \brief Interfaccia comune dei tipi
   *
   */
  class Object {
  protected:
    object_type _type;
  public:
    virtual ~Object () = default;
    object_type type () const { return _type; }
  };

  typedef std::shared_ptr<Object> ObjectPtr;

  constexpr auto Bottom = nullptr;

}

#endif
