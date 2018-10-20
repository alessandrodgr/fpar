#ifndef FUNCTIONALS_HPP
#define FUNCTIONALS_HPP

/** \file Functionals.hpp
 * Definizione dei funzionali
 * Definizione dei funzionali in un sistema FP like. Sono espressioni che
 * denotano funzioni, in input prendono oggetti (Atom o Sequence) e
 * funzioni (primitive o definite) e restituiscono oggetti
 */

#include "Object.hpp"

#include <initializer_list>
#include <vector>
#include <functional>
#include <numeric>
// #include <type_traits>
#include <future>
#include <thread>
// #include <omp.h>
#include <algorithm>
#include <cilk/cilk.h>
// #include <cilk/reducer.h>
#include <cilk/cilk_api.h>
#include <iostream>

namespace fp {

  template <typename F, typename G>
  inline auto compose (F f, G g) {
    return [=](const auto& x) {
      return f(g(x));
    };
  }

  template <typename T, typename F>
  inline auto construct (std::initializer_list<F> fs, bool par) {
    return [=](const T& x) -> T {
      auto fs_list = fs.begin();
      auto res = Sequence<T>(fs.size());
      // auto res = std::vector<T>(fs.size());
      if(par) {
        cilk_for (size_t i = 0; i < fs.size(); i++) {
          // res[i] = (fs_list[i])(x);
          std::move(res).set(i, (fs_list[i])(x));
        }
      } else {
        for(size_t i = 0; i < fs.size(); i++) {
          // res[i] = (fs_list[i])(x);
          std::move(res).set(i, (fs_list[i])(x));
        }
      }
      return res;
      // return Sequence<T>(res.begin(), res.end());
    };
  }

  template <typename T, typename P, typename F, typename G>
  inline auto condition (P p, F f, G g, bool par) {
    return [=](const T& x) -> T {
      T _px;
      if (par) { // calcolo in parallelo
        // utile solo se p(x) "costa" quanto f(x) e g(x)
        // sorta di valutazione eager ma in parallelo
        auto fpx = std::async(std::launch::async, [=](){return p(x);});
        auto ffx = std::async(std::launch::async, [=](){return f(x);});
        auto fgx = std::async(std::launch::async, [=](){return g(x);});
        _px = fpx.get(); // si attende il valore di p(x)
        if (_px.isBottom() or !_px.template is<bool>()) return Bottom;
        bool px = _px;
        if (px) {
          return ffx.get();
        } else {
          return fgx.get();
        }
      } else { // calcolo sequenziale
        _px = p(x);
        if (_px.isBottom() or !_px.template is<bool>()) return Bottom;
        bool px = _px;
        if (px) {
          return f(x);
        } else {
          return g(x);
        }
      }
    };
  }

  template <typename T>
  inline auto constant (T&& c) {
    return [=](const T& x) -> T {
      if(x.isBottom()) return Bottom;
      return c;
    };
  }

  template <typename T, typename F>
  inline auto insert (F f, bool par, const T& n) {
    return [=](const T& x) -> T {
      if (x.isBottom() or !x.isSequence()) return Bottom;
      Sequence<T> s = x;
      if (s.size() == 0) return Bottom;
      // std::accumulate si aspetta un operatore binario,
      // tuttavia in backus fp le operazioni binarie
      // sono operazioni unarie che prendono coppie in input
      using operand_t = immer::box<T>;
      auto combinator =
        [&](operand_t a, operand_t b) {
          return f(Sequence<T>({a, b}));
        };
      if (par) {
        auto els = s.size();
        auto n_threads = __cilkrts_get_nworkers();
        auto locals = std::vector<decltype(f(x))>(n_threads);
        auto local_acc = [&](auto first, auto last){
          return std::accumulate(first, last, operand_t(n), combinator);
        };
        auto firstel = s.begin();

        cilk_for (auto i = 0; i < n_threads; i++) {
          locals[i] = local_acc(firstel + els*i/n_threads, firstel + els*(i+1)/n_threads);
        }
        return std::accumulate(locals.begin(), locals.end(), operand_t(n), combinator);
      } else {
        // il primo elemento della sequenza è l'elemento neutro per l'operazione
        return std::accumulate(s.begin(),
                               s.end(),
                               operand_t(n),
                               combinator);
      }
    };
  }

  template <typename T, typename F>
  inline auto apply_to_all (F f, bool par) {
    return [=](const T& x) -> T {
      if (x.isBottom() or !x.isSequence()) return Bottom;
      Sequence<T> s = x;
      auto res = Sequence<T>(s.size());
      // auto res = std::vector<T>(s.size());
      if (par) {
        // #pragma omp parallel for
        cilk_for (size_t i = 0; i < s.size(); i++) {
          std::move(res).set(i, f(s[i]));
          // res[i] = f(s[i]);
        }
      } else {
        for(size_t i = 0; i < s.size(); i++) {
          // res[i] = f(s[i]);
          std::move(res).set(i, f(s[i]));
        }
      }
      return res;
      // return Sequence<T>(res.begin(), res.end());
    };
  }

  template <typename T, typename F>
  inline auto binary_to_unary (F f, const T& x) {
    return [=](const T& y) -> T {
      using box_t = immer::box<T>;
      return f(Sequence<T>({box_t(x), box_t(y)}));
    };
  }

  template <typename T, typename P, typename F>
  inline auto while_form (P p, F f) {
    return [=](const T& x) -> T {
      T px;
      auto _x(x);
      // Backus la definisce ricorsivamente
      // Per semplificare si sfrutta il costrutto while del C++
      do {
        if (_x.isBottom()) return Bottom;
        px = p(_x);
        if (px.isBottom()) return Bottom;
        _x = f(_x);
        if (!px.template is<bool>()) return Bottom;
      } while ((bool)px);
      return _x;
    };
  }

}

#endif
