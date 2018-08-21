#ifndef Sequence_HPP
#define Sequence_HPP

/** \file Sequence.hpp
 * Definizione del tipo Sequence
 * Definizione del tipo delle sequenze di un sistema FP like.
 * Al contrario del sistema descritto da Backus nel suo articolo i costruttori
 * non sono bottom preserving. Le sequenze sono implementate come struttre dati
 * ricorsive e immutabili.
 */

#include "Object.hpp"
#include "Atom.hpp"

#include <deque>
#include <memory>
#include <initializer_list>

namespace fp {

  typedef std::deque<ObjectPtr> ObjectPtrDeque;

  /*! \class Sequence
   *  \brief Tipo delle sequenze
   *
   *  Le sequenze sono definite ricorsivamente come una deque (della STL)
   *  di puntatori a oggetti e come oggetti stessi. La struttra è immutabile.
   */
  class Sequence : public Object, public ObjectPtrDeque {
  private:

    /* i costruttori sono privati, l'unico modo per utilizzare la struttura è
       chiamare i factory methods
    */
    Sequence () : ObjectPtrDeque() {
      _type = SEQUENCE;
    }

    Sequence (std::initializer_list<ObjectPtr> init)
      : ObjectPtrDeque(init) {
      _type = SEQUENCE;
    }

    template <typename InputIterator>
    Sequence (InputIterator first, InputIterator last)
      : ObjectPtrDeque(first, last) {
      _type = SEQUENCE;
    }

    Sequence (size_type n)
      : ObjectPtrDeque(n, Bottom) {
      _type = SEQUENCE;
    };

  public:

    /*! \brief Factory method sequenza vuota
     *  \return Puntatore a sequenza vuota
     */
    inline static std::shared_ptr<Sequence>
    make_sequence () {
      return std::make_unique<Sequence>( Sequence() );
    }

    inline static std::shared_ptr<Sequence>
    make_sequence (std::initializer_list<ObjectPtr> init) {
      return std::make_unique<Sequence>( Sequence(init) );
    }

    template <typename InputIterator>
    inline static std::shared_ptr<Sequence>
    make_sequence (InputIterator first, InputIterator last) {
      return std::make_unique<Sequence>( Sequence(first, last) );
    }

    inline static std::shared_ptr<Sequence>
    make_sequence (size_type n) {
      return std::make_unique<Sequence>( Sequence(n) );
    }

    /*! \brief Metdodo per la conversione a sequenza di un oggetto
     *  \param obj Oggetto da convertire
     *  \return Puntatore alla sequenza ottenuta dalla conversione (significativo
     *          solo se obj è effettivamente una sequenza)
     */
    inline static std::shared_ptr<Sequence>
    to_sequence (std::shared_ptr<Object> obj) {
      return std::static_pointer_cast<Sequence>(obj); // cast statico!!
    }


  };

  typedef std::shared_ptr<Sequence> SequencePtr;

}

#endif
