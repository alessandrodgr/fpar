#ifndef FUNCTIONALS_HPP
#define FUNCTIONALS_HPP

/** \file Functionals.hpp
 * Definizione dei funzionali
 * Definizione dei funzionali in un sistema FP like. Sono i costrutti necessari
 * alla costruzione di programmi a partire da altri programmi. In altri termini
 * sono le operazioni definite sull'algebra di programmi.
 */

#include "Object.hpp"

#include <initializer_list>
#include <vector>
#include <functional>
#include <numeric>
#include <future>
#include <thread>
#include <omp.h>
#include <algorithm>

namespace fp {

  /*
   I funzionali (o functional forms) restituiscono funzioni.
   Nello specifico, in questa implementazione, restituiscono lambda expressions
  */

  /*! \brief Composizione di funzioni (operatore)
   *  \param f Funzione più esterna
   *  \param g Funzione più interna
   *  \return x -> f(g(x))
   */
  template <typename F, typename G>
  auto operator*(F f, G g) {
    return [=](const auto& x) {
      return f(g(x));
    };
  }

  /*! \brief Composizione di funzioni (funzione)
   *  \param f Funzione più esterna
   *  \param g Funzione più interna
   *  \return x -> f(g(x))
   */
  template <typename F, typename G>
  inline auto compose (F f, G g) {
    return [=](const auto& x) {
      return f(g(x));
    };
  }

  /*! \brief Costruisce sequenze a partire da funzioni
   *  \param fs Insieme di funzioni da applicare per costruire la sequenza
   *  \param par se true eseguito su più thread
   *  \return x -> <f1(x), f2(x), ..., fN(x)>
   */
  template <bool par, typename T, typename F>
  inline auto construct (std::initializer_list<F> fs) {
    return [=](const T& x) -> T {
      auto fs_list = fs.begin();
      auto res = Sequence<T>(fs.size());
      if constexpr (par) {
        #pragma omp parallel for
        for (size_t i = 0; i < fs.size(); i++) {
          std::move(res).set(i, (fs_list[i])(x));
        }
      } else {
        for(size_t i = 0; i < fs.size(); i++) {
          std::move(res).set(i, (fs_list[i])(x));
        }
      }
      return res;
    };
  }

  /*! \brief Costrutto "if-then-else"
   *  \param p Funzione di "guardia"
   *  \param f Applicazione ramo then
   *  \param g Applicazione ramo else
   *  \param par se true eseguito su più thread
   *  \return x -> (p(x) ? f(x) : g(x))
   */
  template <bool par, typename T, typename P, typename F, typename G>
  inline auto condition (P p, F f, G g) {
    return [=](const T& x) -> T {
      T _px;
      if constexpr (par) { // calcolo in parallelo
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

  /*! \brief Funzione costante
   *  \param c Valore da incapsulare in un atomo costante
   *  \return x -> c
   */
  template <typename T>
  inline auto constant (T&& c) {
    return [=](const T& x) -> T {
      if(x.isBottom()) return Bottom;
      return c;
    };
  }

  /*! \brief Operazione di "fold"
   *  \param f Funzione di riduzione
   *  \param par se true eseguito su più thread
   *  \param uf Valore di accumulazione di default
   *  \return <x1, x2, .., xN> -> f(uf, f(x1, f(x2, ...f(xN-1, xN))))
   */
  template <bool par, typename T, typename F>
  inline auto insert (F f, const T& n) {
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
      if constexpr (par) {
        auto els = s.size();
        auto n_threads = omp_get_max_threads();
        auto locals = std::vector<decltype(f(x))>(n_threads);
        auto local_acc = [&](auto first, auto last){
          return std::accumulate(first, last, operand_t(n), combinator);
        };
        auto firstel = s.begin();

        #pragma omp parallel for num_threads(n_threads)
        for (auto i = 0; i < n_threads; i++) {
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

  /*! \brief Operazione di "map"
   *  \param f Funzione da applicare agli elementi di una sequenza
   *  \param par se true eseguito su più thread
   *  \return <x1, x2, .., xN> -> <f(x1), f(x2), ..., f(xN)>
   */
  template <bool par, typename T, typename F>
  inline auto apply_to_all (F f) {
    return [=](const T& x) -> T {
      if (x.isBottom() or !x.isSequence()) return Bottom;
      Sequence<T> s = x;
      auto res = Sequence<T>(s.size());
      if constexpr (par) {
        #pragma omp parallel for
        for (size_t i = 0; i < s.size(); i++) {
          std::move(res).set(i, f(s[i]));
        }
      } else {
        for(size_t i = 0; i < s.size(); i++) {
          std::move(res).set(i, f(s[i]));
        }
      }
      return res;
    };
  }

  /*! \brief Rende f una funzione unaria (simile al currying)
   *  \param f Funzione da rendere unaria
   *  \param x Primo parametro da passare a f
   *  \return y -> f(<x,y>)
   */
  template <typename T, typename F>
  inline auto binary_to_unary (F f, const T& x) {
    return [=](const T& y) -> T {
      using box_t = immer::box<T>;
      return f(Sequence<T>({box_t(x), box_t(y)}));
    };
  }

  /*! \brief Costrutto di iterazione
   *  \param p Funzione di "guardia"
   *  \param f "Corpo" del while
   *  \return x -> {while(p(x)) {x = f(x)}; return x;}
   */
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
