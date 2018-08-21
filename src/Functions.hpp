#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

/** \file Functions.hpp
 * Definizione delle funzioni primitive
 * Definizione di funzioni primitive di un sistema FP lik{e. Le funzioni
 * sono tutte bottom preserving e prendono oggetti (Atom o Sequence) in input e
 * restituiscono oggetti in output
 */

#include "Objects.hpp"

#include <vector>

namespace fp {

  /*! \brief Operazione di accesso ad elementi di una sequenza
   *  \param i Indice (compreso tra 1 ed N) dell'elemento da accedere
   *  \return i -> x_i
   */
  template <typename T>
  inline auto select (unsigned int i) {
    return [=](const SequencePtr& x) -> AtomPtr<T> {
      if(x == Bottom) return Bottom;
      if(i == 0 || x->size() < i) return Bottom;
      // viene specificato il tipo T dell'atomo che si vuole ottenere
      return Atom<T>::to_atom((*x)[i-1]);
    };
  }

  // template specialization del precedente
  template <>
  inline auto select<Sequence> (unsigned int i) {
    return [=](const SequencePtr& x) -> SequencePtr {
      if(x == Bottom) return Bottom;
      if(i == 0 || x->size() < i) return Bottom;
      // se T = Sequence, si ottiene una sequenza
      return Sequence::to_sequence((*x)[i-1]);
    };
  }

  // template specialization del precedente
  template <>
  inline auto select<Object> (unsigned int i) {
    return [=](const SequencePtr& x) -> ObjectPtr {
      if(x == Bottom) return Bottom;
      if(i == 0 || x->size() < i) return Bottom;
      // se T = Object, nessun cast
      return (*x)[i-1];
    };
  }

  /*! \brief Restituisce una sequenza senza il primo elemento
   *  \param x Sequenza <x1, x2, ..., xN>
   *  \return <x2, ..., xN>
   */
  inline SequencePtr tail (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    if(x->size() == 0) return Bottom;
    // lascia x immutato, però complessità Θ(n) => da rivedere
    return Sequence::make_sequence(std::next(x->begin()), x->end());
  }

  /*! \brief Funzione identità
   *  \param x valore in ingresso/uscita
   *  \return x
   */
  template <typename T>
  inline auto id (const T& x) {
    return x;
  }

  /*! \brief Check sequenza vuota
   *  \param x Sequenza <x1, x2, ..., xN>
   *  \return true se #x = 0, false altrimenti
   */
  inline AtomPtr<bool> null (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    return Atom<bool>::make_atom(x->size() == 0);
  }

  /*! \brief Restituisce una sequenza al contrario
   *  \param x Sequenza <x1, x2, ..., xN>
   *  \return <xN, ..., x2, x1>
   */
  inline SequencePtr reverse (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    // lascia x immutato, però complessità Θ(n) => da rivedere
    return Sequence::make_sequence(x->rbegin(), x->rend());
  }

  /*! \brief Distribuzione di un oggetto in una sequenza
   *  \param par se true, eseguito in parallelo
   *  \return <y, <z1,z2,..,zN> -> <<y,z1>,<y,z2>,...,<y,zN>>
   */
  inline auto distl (bool par = true) {
    return [&](const SequencePtr& x) -> SequencePtr {
      if(x == Bottom) return Bottom;
      if(x->size() != 2) return Bottom; // controllo che in input ci sia una coppia
      auto y = x->front();
      auto _zs = x->back();
      // controllo che il secondo elemento sia una sequenza
      if(!_zs || _zs->type() != SEQUENCE) return Bottom;
      auto zs = Sequence::to_sequence(_zs); // cast a sequenza
      auto res = Sequence::make_sequence(zs->size());
      #pragma omp parallel for if(par)
      for(size_t i = 0; i < zs->size(); i++) {
        (*res)[i] = Sequence::make_sequence({y, (*zs)[i]});
      }
      return res;
    };
  }

  /*! \brief Distribuzione di un oggetto in una sequenza
   *  \param par se true, eseguito in parallelo
   *  \return <<y1,y2,..,yN>,z> -> <<y1,z>,<y2,z>,...,<yN,z>>
   */
  inline auto distr (bool par = true) {
    return [&](const SequencePtr& x) -> SequencePtr {
      // come distl
      if(x == Bottom) return Bottom;
      if(x->size() != 2) return Bottom;
      auto _ys = x->front();
      auto z = x->back();
      if(!_ys || _ys->type() != SEQUENCE) return Bottom;
      auto ys = Sequence::to_sequence(_ys);
      auto res = Sequence::make_sequence(ys->size());
      #pragma omp parallel for if(par)
      for(size_t i = 0; i < ys->size(); i++) {
        (*res)[i] = Sequence::make_sequence({(*ys)[i], z});
      }
      return res;
    };
  }

  /*! \brief Restituisce il numero di elementi di una sequenza
   *  \param x Sequenza <x1,x2,..,xN>
   *  \return N
   */
  inline AtomPtr<size_t> length (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    return Atom<size_t>::make_atom(x->size());
  }

  // le operazioni aritmetiche sono generalizzate a tutti i tipi T che
  // permettono operazioni di +,-,*,/

  /*! \brief Operazione di addizione generalizzata
   *  \param x Coppia <x1,x2>
   *  \return x1+x2
   */
  template <typename T>
  inline AtomPtr<T> add (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    if(x->size() != 2) return Bottom;
    auto _y = x->front();
    auto _z = x->back();
    if(_y == Bottom || _z == Bottom) return Bottom;
    if(_y->type() != ATOM || _z->type() != ATOM) return Bottom;
    auto y = Atom<T>::to_atom(_y);
    auto z = Atom<T>::to_atom(_z);
    return Atom<T>::make_atom(*y + *z);
  }

  /*! \brief Operazione di sottrazione generalizzata
   *  \param x Coppia <x1,x2>
   *  \return x1-x2
   */
  template <typename T>
  inline AtomPtr<T> sub (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    if(x->size() != 2) return Bottom;
    auto _y = x->front();
    auto _z = x->back();
    if(_y == Bottom || _z == Bottom) return Bottom;
    if(_y->type() != ATOM || _z->type() != ATOM) return Bottom;
    auto y = Atom<T>::to_atom(_y);
    auto z = Atom<T>::to_atom(_z);
    return Atom<T>::make_atom(*y - *z);
  }

  /*! \brief Operazione di moltiplicazione generalizzata
   *  \param x Coppia <x1,x2>
   *  \return x1*x2
   */
  template <typename T>
  inline AtomPtr<T> mul (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    if(x->size() != 2) return Bottom;
    auto _y = x->front();
    auto _z = x->back();
    if(_y == Bottom || _z == Bottom) return Bottom;
    if(_y->type() != ATOM || _z->type() != ATOM) return Bottom;
    auto y = Atom<T>::to_atom(_y);
    auto z = Atom<T>::to_atom(_z);
    return Atom<T>::make_atom(*y * *z);
  }

  /*! \brief Operazione di divisione generalizzata
   *  \param x Coppia <x1,x2>
   *  \return x1/x2
   */
  template <typename T>
  inline AtomPtr<T> div (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    if(x->size() != 2) return Bottom;
    auto _y = x->front();
    auto _z = x->back();
    if(_y == Bottom || _z == Bottom) return Bottom;
    if(_y->type() != ATOM || _z->type() != ATOM) return Bottom;
    auto z = Atom<T>::to_atom(_z);
    if(*z == 0) return Bottom;
    auto y = Atom<T>::to_atom(_y);
    return Atom<T>::make_atom(*y / *z);
  }

  /*! \brief Trasposizione di sequenze
   *  \param x Sequenze <<x1,...,xN>,<y1,...,yN>,...,<z1,...,zN>>
   *  \return <<x1,y1,...,z1>,<x2,y2,...,z2>,...,<xN,yN,...,zN>>
   */
  inline SequencePtr trans (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    std::vector<Sequence::const_iterator> row_heads;
    size_t els = 0;
    // salvo gli iteratori (teste) di ogni sequenza
    // e calcolo il numero minimo M di elementi
    // (le sequenze vengono trasposte fino ad M)
    for(auto _row = x->begin(); _row < x->end(); _row++) {
      if((*_row)->type() != SEQUENCE) return Bottom;
      auto row = Sequence::to_sequence(*_row);
      if(els == 0 || row->size() < els) els = row->size();
      row_heads.push_back(row->cbegin());
    }

    // sequenza contenente le sequenze trasposte
    auto transd = Sequence::make_sequence();
    for(size_t i = 0; i < els; i++) {
      // per ogni i si traspone la i-esima "colonna"

      auto transd_row = Sequence::make_sequence();

      // prende la testa di ogni riga e la mette in quella trasposta
      // poi sposta gli iteratori di testa
      for(auto& row_head : row_heads) {
        transd_row->push_back(*row_head);
        row_head++;
      }

      transd->push_back(transd_row);
    }
    return transd;
  }

  /*! \brief Operazione logica AND
   *  \param x Coppia <y,z>
   *  \return y AND z
   */
  inline AtomPtr<bool> and_op (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    if(x->size() != 2) return Bottom;
    auto _y = x->front();
    auto _z = x->back();
    if(_y == Bottom || _z == Bottom) return Bottom;
    if(_y->type() != ATOM || _z->type() != ATOM) return Bottom;
    auto y = Atom<bool>::to_atom(_y);
    // se y è false non valutare z
    if(*y == false) return Atom<bool>::make_atom(false);
    auto z = Atom<bool>::to_atom(_z);
    return Atom<bool>::make_atom(*y && *z);
  }

  /*! \brief Operazione logica OR
   *  \param x Coppia <y,z>
   *  \return y OR z
   */
  inline AtomPtr<bool> or_op (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    if(x->size() != 2) return Bottom;
    auto _y = x->front();
    auto _z = x->back();
    if(_y == Bottom || _z == Bottom) return Bottom;
    if(_y->type() != ATOM || _z->type() != ATOM) return Bottom;
    auto y = Atom<bool>::to_atom(_y);
    // se y è true non valutare z
    if(*y == true) return Atom<bool>::make_atom(true);
    auto z = Atom<bool>::to_atom(_z);
    return Atom<bool>::make_atom(*y || *z);
  }

  /*! \brief Operazione logica NOT
   *  \param x Elemento da negare
   *  \return NOT(x)
   */
  inline AtomPtr<bool> not_op (const AtomPtr<bool>& x) {
    if(x == Bottom) return Bottom;
    return Atom<bool>::make_atom(!(*x));
  }

  /*! \brief Operazione di append in testa ad una sequenza
   *  \param x Coppia <y,<z1,...,zN>>
   *  \return <y,z1,...,zN>
   */
  inline SequencePtr apndl (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    auto y = x->front();
    auto _zs = x->back();
    if(_zs == Bottom) return Bottom;
    if(_zs->type() != SEQUENCE) return Bottom;
    auto zs = Sequence::to_sequence(_zs);
    // lascia x immutato, però complessità Θ(n) => da rivedere
    auto res = Sequence::make_sequence(zs->begin(), zs->end());
    res->push_front(y);
    return res;
  }

  /*! \brief Operazione di append in coda ad una sequenza
   *  \param x Coppia <<y1,...,yN>,z>
   *  \return <y1,...,yN,z>
   */
  inline SequencePtr apndr (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    auto z = x->back();
    auto _ys = x->front();
    if(_ys == Bottom) return Bottom;
    if(_ys->type() != SEQUENCE) return Bottom;
    auto ys = Sequence::to_sequence(_ys);
    // lascia x immutato, però complessità Θ(n) => da rivedere
    auto res = Sequence::make_sequence(ys->begin(), ys->end());
    res->push_back(z);
    return res;
  }

  // rselect: come select ma da destra

  template <typename T>
  inline auto rselect (unsigned int i) {
    return [=](const SequencePtr& x) -> AtomPtr<T> {
      if(x == Bottom) return Bottom;
      auto len = x->size();
      if(i == 0 || len < i) return Bottom;
      return Atom<T>::to_atom((*x)[len-i]);
    };
  }

  template <>
  inline auto rselect<Sequence> (unsigned int i) {
    return [=](const SequencePtr& x) -> SequencePtr {
      if(x == Bottom) return Bottom;
      auto len = x->size();
      if(i == 0 || len < i) return Bottom;
      return Sequence::to_sequence((*x)[len-i]);
    };
  }

  template <>
  inline auto rselect<Object> (unsigned int i) {
    return [=](const SequencePtr& x) -> ObjectPtr {
      if(x == Bottom) return Bottom;
      auto len = x->size();
      if(i == 0 || len < i) return Bottom;
      return (*x)[len-i];
    };
  }

  // rselect: come tail ma da destra

  inline SequencePtr rtail (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    if(x->size() == 0) return Bottom;
    return Sequence::make_sequence(x->begin(), std::prev(x->end()));
  }

  /*! \brief Shift ciclico di una sequenza verso sx
   *  \param x Sequenza <x1,x2,...,xN>
   *  \return <x2,...,xN,x1>
   */
  inline SequencePtr rotl (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    if(x->size() < 2) return x;
    // lascia x immutato, però complessità Θ(n) => da rivedere
    auto res = Sequence::make_sequence(std::next(x->begin()), x->end());
    res->push_back(x->front());
    return res;
  }

  /*! \brief Shift ciclico di una sequenza verso dx
   *  \param x Sequenza <x1,x2,...,xN>
   *  \return <xN,x1,...,xN-1>
   */
  inline SequencePtr rotr (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    if(x->size() < 2) return x;
    // lascia x immutato, però complessità Θ(n) => da rivedere
    auto res = Sequence::make_sequence(x->begin(), std::prev(x->end()));
    res->push_front(x->back());
    return res;
  }

}

#endif
