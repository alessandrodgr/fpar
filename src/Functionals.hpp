#ifndef FUNCTIONALS_HPP
#define FUNCTIONALS_HPP

/** \file Functionals.hpp
 * Definizione dei funzionali
 * Definizione dei funzionali in un sistema FP like. Sono espressioni che
 * denotano funzioni, in input prendono oggetti (Atom o Sequence) e
 * funzioni (primitive o definite) e restituiscono oggetti
 */

#include "Objects.hpp"

#include <initializer_list>
#include <vector>
#include <functional>
#include <variant>
#include <numeric>
#include <type_traits>
#include <future>
#include <thread>
#include <omp.h>
#include <algorithm>

namespace fp {

  /*
   I funzionali (o functional forms) restituiscono funzioni.
   Nello specifico, in questa implementazione, restituiscono lambda
  */


  template <typename F, typename G>
  inline auto compose (F f, G g) {
    return [=](const auto& x) {
      return f(g(x));
    };
  }

  template <typename F>
  inline auto construct (std::initializer_list<F> fs, bool par = true) {
    return [=](const auto& x) {
      auto fs_list = fs.begin();
      auto res = Sequence::make_sequence(fs.size());
      #pragma omp parallel for if(par)
      for(size_t i = 0; i < fs.size(); i++) {
        (*res)[i] = (fs_list[i])(x);
      }
      return res;
    };
  }

  template <typename P, typename F, typename G>
  inline auto condition (P p, F f, G g, bool par = true) {
    return [=](const auto& x) -> ObjectPtr {
      AtomPtr<bool> px;
      if(par) {
        auto fpx = std::async(std::launch::async, [=](){return p(x);});
        auto ffx = std::async(std::launch::async, [=](){return f(x);});
        auto fgx = std::async(std::launch::async, [=](){return g(x);});
        px = fpx.get();
        if(px == Bottom) return Bottom;
        if(*px) {
          return ffx.get();
        } else {
          return fgx.get();
        }
      } else {
        px = p(x);
        if(px == Bottom) return Bottom;
        if(*px) {
          return f(x);
        } else {
          return g(x);
        }
      }
    };
  }

  template <typename T>
  inline auto constant (T&& c) {
    return [&](const auto& x) -> AtomPtr<T> {
      if(x == Bottom) return Bottom;
      return Atom<T>::make_atom(c);
    };
  }

  template <>
  inline auto constant (ObjectPtr&& c) {
    return [&](const auto& x) -> ObjectPtr {
      if(x == Bottom) return x;
      return std::move(c);
    };
  }

  template <typename A>
  inline auto constant (AtomPtr<A>&& c) {
    return [&](const auto& x) -> AtomPtr<A> {
      if(x == Bottom) return x;
      return std::move(c);
    };
  }

  template <>
  inline auto constant (SequencePtr&& c) {
    return [&](const auto& x) -> SequencePtr {
      if(x == Bottom) return x;
      return std::move(c);
    };
  }

  template <typename F>
  inline auto insert (F f, bool par = true, const ObjectPtr& uf = Bottom) {
    return [=](const SequencePtr& x) -> ObjectPtr {
      if(x == Bottom) return Bottom;
      if(x->size() == 0) return uf;
      auto combinator =
        [&](ObjectPtr& a, ObjectPtr& b) {
          return f(Sequence::make_sequence({a, b}));
        };
      if(uf == Bottom)
        return std::accumulate(std::next(x->begin()),
                               x->end(),
                               x->front(),
                               combinator);
      return std::accumulate(x->begin(), x->end(), uf, combinator);
    };
  }

  template <typename T, typename F>
  inline auto apply_to_all (F f, bool par = true) {
    return [=](const SequencePtr& x) -> SequencePtr {
      if(x == Bottom) return Bottom;
      auto res = Sequence::make_sequence(x->size());
      #pragma omp parallel for if(par)
      for(size_t i = 0; i < x->size(); i++) {
        if constexpr (std::is_same_v<T, Sequence>) {
          auto s = Sequence::to_sequence((*x)[i]);
          (*res)[i] = f(s);
        } else if constexpr (std::is_same_v<T, Object>) {
          (*res)[i] = f((*x)[i]);
        } else {
          auto a = Atom<T>::to_atom((*x)[i]);
          (*res)[i] = f(a);
        }
      }
      return res;
    };
  }

  template <typename F>
  inline auto binary_to_unary (F f, const ObjectPtr& x) {
    return [=](const auto& y) -> ObjectPtr {
      return f(Sequence::make_sequence({x, y}));
    };
  }

  template <typename P, typename F>
  inline auto while_form (P p, F f) {
    return [=](const auto& x) -> decltype(f(x)) {
      AtomPtr<bool> px;
      auto _x = x;
      // Backus la definisce ricorsivamente
      // Per semplificare si sfrutta il costrutto while del C++
      do {
        if(_x == Bottom) return Bottom;
        px = p(_x);
        if(px == Bottom) return Bottom;
        _x = f(_x);
      } while(*px);
      return _x;
    };
  }

}

#endif
