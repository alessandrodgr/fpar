#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include "Object.hpp"
#include <vector>
#include <cilk/cilk.h>

namespace fp {

  template <typename T>
  inline auto select (unsigned int i) {
    return [=](const T& x) -> T {
      if (x.isBottom() or !x.isSequence()) return Bottom;
      Sequence<T> s = x;
      if (i == 0 or s.size() < i) return Bottom;
      return *(s[i-1]);
    };
  }

  template <typename T>
  inline T tail (const T& x) {
    if (x.isBottom() or !x.isSequence()) return Bottom;
    Sequence<T> s = x;
    if (s.size() == 0) return Bottom;
    return s.drop(1);
  }

  template <typename T>
  inline auto id (const T& x) {
    return x;
  }

  template <typename T>
  inline T null (const T& x) {
    if (x.isBottom() or !x.isSequence()) return Bottom;
    Sequence<T> s = x;
    return (s.size() == 0);
  }

  template <typename T>
  inline T reverse (const T& x) {
    if (x.isBottom() or !x.isSequence()) return Bottom;
    Sequence<T> s = x;
    return Sequence<T>(s.rbegin(), s.rend());
  }

  template <typename T>
  inline auto distl (bool par) {
    return [=](const T& x) -> T {
      if (x.isBottom() or !x.isSequence()) return Bottom;
      Sequence<T> s = x;
      if (s.size() != 2) return Bottom; // controllo che in input ci sia una coppia
      auto y = *s.front();
      auto _zs = *s.back();
      // controllo che il secondo elemento sia una sequenza
      if (_zs.isBottom() or !_zs.isSequence()) return Bottom;
      Sequence<T> zs = _zs; // cast a sequenza
      auto res = Sequence<T>(zs.size());
      // auto res = std::vector<T>(zs.size());
      if (par) {
        cilk_for (size_t i = 0; i < zs.size(); i++) {
          std::move(res).set(i, Sequence<T>({y, zs[i]}));
          // res[i] = Sequence<T>({y, zs[i]});
        }
      } else {
        for (size_t i = 0; i < zs.size(); i++) {
          std::move(res).set(i, Sequence<T>({y, zs[i]}));
          // res[i] = Sequence<T>({y, zs[i]});
        }
      }
      return res;
      // return Sequence<T>(res.begin(), res.end());
    };
  }

  template <typename T>
  inline auto distr (bool par) {
    return [=](const T& x) -> T {
      if (x.isBottom() or !x.isSequence()) return Bottom;
      Sequence<T> s = x;
      if (s.size() != 2) return Bottom; // controllo che in input ci sia una coppia
      auto _ys = *s.front();
      auto z = *s.back();
      // controllo che il secondo elemento sia una sequenza
      if (_ys.isBottom() or !_ys.isSequence()) return Bottom;
      Sequence<T> ys = _ys; // cast a sequenza
      auto res = Sequence<T>(ys.size());
      // auto res = std::vector<T>(ys.size());
      if (par) {
        cilk_for (size_t i = 0; i < ys.size(); i++) {
          std::move(res).set(i, Sequence<T>({ys[i], z}));
          // res[i] = Sequence<T>({ys[i], z});
        }
      } else {
        for (size_t i = 0; i < ys.size(); i++) {
          std::move(res).set(i, Sequence<T>({ys[i], z}));
          // res[i] = Sequence<T>({ys[i], z});
        }
      }
      return res;
      // return Sequence<T>(res.begin(), res.end());
    };
  }

  template <typename T>
  inline T length (const T& x) {
    if (x.isBottom() or !x.isSequence()) return Bottom;
    Sequence<T> s = x;
    return s.size();
  }

  template <typename T>
  inline T atom (const T& x) {
    if (x.isBottom()) return Bottom;
    if (x.isSequence()) {
      Sequence<T> s = x;
      if (s.size() == 0) return true;
      return false;
    }
    return true;
  }

  template <typename O, typename T>
  inline T equals (const T& x) {
    if (x.isBottom() or !x.isSequence()) return Bottom;
    Sequence<T> s = x;
    if (s.size() != 2) return Bottom;
    auto _y = *s.front();
    auto _z = *s.back();
    if (_y.template is<O>() and _z.template is<O>()) {
      O y = _y;
      O z = _z;
      return (y == z);
    } else if (_y.isSequence() and _z.isSequence()) {
      Sequence<T> y = _y;
      Sequence<T> z = _z;
      if (y.size() != z.size()) return false;
      auto yit = y.begin();
      auto zit = z.begin();
      for (; yit != y.end(); yit++) {
        auto _eq = equals<O>((T)Sequence<T>({*yit, *zit}));
        if (_eq.isBottom()) return Bottom;
        bool eq = _eq;
        if (!eq) return false;
        zit++;
      }
      return true;
    }
    return false;
  }

  template <typename T>
  inline T trans (const T& x) {
    if (x.isBottom() or !x.isSequence()) return Bottom;
    using seq_t = Sequence<T>;
    seq_t s = x;
    using seq_it_t = typename seq_t::iterator;
    std::vector<seq_it_t> row_heads;
    row_heads.reserve(s.size());
    size_t els = 0;
    // salvo gli iteratori (teste) di ogni sequenza
    // e calcolo il numero minimo M di elementi
    // (le sequenze vengono trasposte fino ad M)
    // seq_t row;
    for (auto _row : s) {
      if (_row->isBottom() or !_row->isSequence()) return Bottom;
      seq_t* row = new seq_t(*_row); // problema è qua
      // std::cout << (int)*row.front() << std::endl;
      if (els == 0 or row->size() < els) els = row->size();
      // std::cout << (int)**row.begin() << std::endl;
      row_heads.push_back(row->begin());
    }

    // sequenza contenente le sequenze trasposte
    auto transd = seq_t();
    for (size_t i = 0; i < els; i++) {
      // per ogni i si traspone la i-esima "colonna"

      auto transd_row = seq_t();

      // prende la testa di ogni riga e la mette in quella trasposta
      // poi sposta gli iteratori di testa

      for (auto& row_head : row_heads) {
        std::move(transd_row).push_back(*row_head++);
      }

      std::move(transd).push_back(transd_row);
    }
    return transd;
  }

  template <typename T>
  inline T and_op (const T& x) {
    if (x.isBottom() or !x.isSequence()) return Bottom;
    Sequence<T> s = x;
    if (s.size() != 2) return Bottom;
    auto _y = *s.front();
    auto _z = *s.back();
    if (_y.isBottom() or _z.isBottom()) return Bottom;
    if (_y.template is<bool>() and _z.template is<bool>()) {
      bool y = _y;
      bool z = _z;
      return (y and z);
    } else return Bottom;
  }

  template <typename T>
  inline T or_op (const T& x) {
    if (x.isBottom() or !x.isSequence()) return Bottom;
    Sequence<T> s = x;
    if (s.size() != 2) return Bottom;
    auto _y = *s.front();
    auto _z = *s.back();
    if (_y.isBottom() or _z.isBottom()) return Bottom;
    if (_y.template is<bool>() and _z.template is<bool>()) {
      bool y = _y;
      bool z = _z;
      return (y or z);
    } else return Bottom;
  }

  template <typename T>
  inline T not_op (const T& x) {
    if (x.isBottom() or !x.template is<bool>()) return Bottom;
    bool y = x;
    return !y;
  }

  template <typename O, typename T>
  inline T add_op (const T& x) {
    if (x.isBottom() or !x.isSequence()) return Bottom;
    Sequence<T> s = x;
    if (s.size() != 2) return Bottom;
    auto _y = *s.front();
    auto _z = *s.back();
    if (_y.isBottom() or _z.isBottom()) return Bottom;
    if (_y.template is<O>() and _z.template is<O>()) {
      O y = _y;
      O z = _z;
      return (y + z);
    } else return Bottom;
  }

  template <typename O, typename T>
  inline T sub_op (const T& x) {
    if (x.isBottom() or !x.isSequence()) return Bottom;
    Sequence<T> s = x;
    if (s.size() != 2) return Bottom;
    auto _y = *s.front();
    auto _z = *s.back();
    if (_y.isBottom() or _z.isBottom()) return Bottom;
    if (_y.template is<O>() and _z.template is<O>()) {
      O y = _y;
      O z = _z;
      return (y - z);
    } else return Bottom;
  }

  template <typename O, typename T>
  inline T mul_op (const T& x) {
    if (x.isBottom() or !x.isSequence()) return Bottom;
    Sequence<T> s = x;
    if (s.size() != 2) return Bottom;
    auto _y = *s.front();
    auto _z = *s.back();
    if (_y.isBottom() or _z.isBottom()) return Bottom;
    if (_y.template is<O>() and _z.template is<O>()) {
      O y = _y;
      O z = _z;
      return (y * z);
    } else return Bottom;
  }

  template <typename O, typename T>
  inline T div_op (const T& x) {
    if (x.isBottom() or !x.isSequence()) return Bottom;
    Sequence<T> s = x;
    if (s.size() != 2) return Bottom;
    auto _y = *s.front();
    auto _z = *s.back();
    if (_y.isBottom() or _z.isBottom()) return Bottom;
    if (_y.template is<O>() and _z.template is<O>()) {
      O z = _z;
      if (z == 0) return Bottom;
      bool y = _y;
      return (y / z);
    } else return Bottom;
  }

  template <typename T>
  inline T apndl (const T& x) {
    if (x.isBottom() or !x.isSequence()) return Bottom;
    Sequence<T> s = x;
    if (s.size() != 2) return Bottom; // controllo che in input ci sia una coppia
    auto y = s.front();
    auto _zs = *s.back();
    // controllo che il secondo elemento sia una sequenza
    if (_zs.isBottom() or !_zs.isSequence()) return Bottom;
    Sequence<T> zs = _zs;
    return zs.push_front(y);
  }

  template <typename T>
  inline T apndr (const T& x) {
    if (x.isBottom() or !x.isSequence()) return Bottom;
    Sequence<T> s = x;
    if (s.size() != 2) return Bottom; // controllo che in input ci sia una coppia
    auto _ys = s.front();
    auto z = *s.back();
    // controllo che il secondo elemento sia una sequenza
    if (_ys.isBottom() or !_ys.isSequence()) return Bottom;
    Sequence<T> ys = _ys;
    return ys.push_back(z);
  }

  template <typename T>
  inline T rselect (unsigned int i, const T& x) {
    if (x.isBottom() or !x.isSequence()) return Bottom;
    Sequence<T> s = x;
    auto len = s.size();
    if (i == 0 or len < i) return Bottom;
    return *(s[len-i]);
  }

  template <typename T>
  inline T rtail (const T& x) {
    if (x.isBottom() or !x.isSequence()) return Bottom;
    Sequence<T> s = x;
    auto len = s.size();
    if (len == 0) return Bottom;
    return s.take(len-1);
  }

  template <typename T>
  inline T rotl (const T& x) {
    if (x.isBottom() or !x.isSequence()) return Bottom;
    Sequence<T> s = x;
    if (s.size() < 2) return x;
    return s.drop(1).push_back(s.front());
  }

  template <typename T>
  inline T rotr (const T& x) {
    if (x.isBottom() or !x.isSequence()) return Bottom;
    Sequence<T> s = x;
    auto len = s.size();
    if (len < 2) return x;
    return s.take(len-1).push_front(s.back());
  }

}

#endif
