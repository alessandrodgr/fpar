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

  template <typename T>
  inline auto select (unsigned int i) {
    return [=](const SequencePtr& x) -> AtomPtr<T> {
      if(x == Bottom) return Bottom;
      if(i == 0 || x->size() < i) return Bottom;
      return Atom<T>::to_atom((*x)[i-1]);
    };
  }

  template <>
  inline auto select<Sequence> (unsigned int i) {
    return [=](const SequencePtr& x) -> SequencePtr {
      if(x == Bottom) return Bottom;
      if(i == 0 || x->size() < i) return Bottom;
      return Sequence::to_sequence((*x)[i-1]);
    };
  }

  template <>
  inline auto select<Object> (unsigned int i) {
    return [=](const SequencePtr& x) -> ObjectPtr {
      if(x == Bottom) return Bottom;
      if(i == 0 || x->size() < i) return Bottom;
      return (*x)[i-1];
    };
  }

  inline SequencePtr tail (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    if(x->size() == 0) return Bottom;
    return Sequence::make_sequence(std::next(x->begin()), x->end());
  }

  template <typename T>
  inline auto id (const T& x) {
    return x;
  }

  inline AtomPtr<bool> null (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    return Atom<bool>::make_atom(x->size() == 0);
  }

  inline SequencePtr reverse (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    return Sequence::make_sequence(x->rbegin(), x->rend());
  }

  inline auto distl (bool par = true) {
    return [&](const SequencePtr& x) -> SequencePtr {
      if(x == Bottom) return Bottom;
      if(x->size() != 2) return Bottom;
      auto y = x->front();
      auto _zs = x->back();
      if(!_zs || _zs->type() != SEQUENCE) return Bottom;
      auto zs = Sequence::to_sequence(_zs);
      auto res = Sequence::make_sequence(zs->size());
      #pragma omp parallel for if(par)
      for(size_t i = 0; i < zs->size(); i++) {
        (*res)[i] = Sequence::make_sequence({y, (*zs)[i]});
      }
      return res;
    };
  }

  inline auto distr (bool par = true) {
    return [&](const SequencePtr& x) -> SequencePtr {
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

  inline AtomPtr<size_t> length (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    return Atom<size_t>::make_atom(x->size());
  }

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

  inline SequencePtr trans (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    std::vector<Sequence::const_iterator> row_heads;
    size_t els = 0;
    for(auto _row = x->begin(); _row < x->end(); _row++) {
      if((*_row)->type() != SEQUENCE) return Bottom;
      auto row = Sequence::to_sequence(*_row);
      if(els == 0 || row->size() < els) els = row->size();
      row_heads.push_back(row->cbegin());
    }
    auto transd = Sequence::make_sequence();
    for(size_t i = 0; i < els; i++) {
      auto transd_row = Sequence::make_sequence();

      for(auto& row_head : row_heads) {
        transd_row->push_back(*row_head);
        row_head++;
      }

      transd->push_back(transd_row);
    }
    return transd;
  }

  inline AtomPtr<bool> and_op (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    if(x->size() != 2) return Bottom;
    auto _y = x->front();
    auto _z = x->back();
    if(_y == Bottom || _z == Bottom) return Bottom;
    if(_y->type() != ATOM || _z->type() != ATOM) return Bottom;
    auto y = Atom<bool>::to_atom(_y);
    if(*y == false) return Atom<bool>::make_atom(false);
    auto z = Atom<bool>::to_atom(_z);
    return Atom<bool>::make_atom(*y && *z);
  }

  inline AtomPtr<bool> or_op (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    if(x->size() != 2) return Bottom;
    auto _y = x->front();
    auto _z = x->back();
    if(_y == Bottom || _z == Bottom) return Bottom;
    if(_y->type() != ATOM || _z->type() != ATOM) return Bottom;
    auto y = Atom<bool>::to_atom(_y);
    if(*y == true) return Atom<bool>::make_atom(true);
    auto z = Atom<bool>::to_atom(_z);
    return Atom<bool>::make_atom(*y || *z);
  }

  inline AtomPtr<bool> not_op (const AtomPtr<bool>& x) {
    if(x == Bottom) return Bottom;
    return Atom<bool>::make_atom(!(*x));
  }

  inline SequencePtr apndl (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    auto y = x->front();
    auto _zs = x->back();
    if(_zs == Bottom) return Bottom;
    if(_zs->type() != SEQUENCE) return Bottom;
    auto zs = Sequence::to_sequence(_zs);
    auto res = Sequence::make_sequence(zs->begin(), zs->end());
    res->push_front(y);
    return res;
  }

  inline SequencePtr apndr (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    auto z = x->front();
    auto _ys = x->back();
    if(_ys == Bottom) return Bottom;
    if(_ys->type() != SEQUENCE) return Bottom;
    auto ys = Sequence::to_sequence(_ys);
    auto res = Sequence::make_sequence(ys->begin(), ys->end());
    res->push_back(z);
    return res;
  }

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

  inline SequencePtr rtail (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    if(x->size() == 0) return Bottom;
    return Sequence::make_sequence(x->begin(), std::prev(x->end()));
  }

  inline SequencePtr rotl (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    if(x->size() < 2) return x;
    auto res = Sequence::make_sequence(std::next(x->begin()), x->end());
    res->push_back(x->front());
    return res;
  }

  inline SequencePtr rotr (const SequencePtr& x) {
    if(x == Bottom) return Bottom;
    if(x->size() < 2) return x;
    auto res = Sequence::make_sequence(x->begin(), std::prev(x->end()));
    res->push_front(x->back());
    return res;
  }

}

#endif
