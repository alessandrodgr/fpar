#include "Backus.hpp"
#include <iostream>
#include <unistd.h>
#include <chrono>

using namespace fpar;

// istanzio il type system con il supporto per int e double
using Number = Object<int, double>;

inline Number select1 (const Number& x) {
  return select<Number>(1)(x);
}

inline Number select2AndTrans (const Number& x) {
  return trans(select<Number>(2)(x));
}

template <bool par>
inline Number IP (const Number& x) {
  auto mul = mul_op<int, Number>;
  auto add = add_op<int, Number>;
  return (insert<par>(add, Number(0)) *
            (apply_to_all<par, Number>(mul) * trans<Number>))(x);
}

template <bool par>
inline Number MM (const Number& x) {

  auto aIP = [=](const Number& y) {
    return apply_to_all<par, Number>(IP<par>)(y);
  };

  return (apply_to_all<par, Number>(aIP) *
            (apply_to_all<par, Number>(distl<par, Number>) *
              (distr<par, Number> *
                construct<par, Number>({select1, select2AndTrans}))))(x);
}

int main(int argc, char const *argv[]) {

  omp_set_num_threads(atoi(argv[1]));

  // costruisco l'input
  auto v = Sequence<Number>();
  for(size_t i = 0; i < 100; i++) {
    auto w = Sequence<Number>();
    for(size_t j = 0; j < 100; j++) {
      std::move(w).push_back((int)(i+j));
    }
    std::move(v).push_back(w);
  }
  Number in = Sequence<Number>({v,v});


  std::chrono::high_resolution_clock::time_point seq_t_i, seq_t_f;
  std::chrono::high_resolution_clock::time_point par_t_i, par_t_f;

  seq_t_i = std::chrono::high_resolution_clock::now();
  auto sr = MM<seq_exec>(in);
  seq_t_f = std::chrono::high_resolution_clock::now();


  par_t_i = std::chrono::high_resolution_clock::now();
  auto pr = MM<par_exec>(in);
  par_t_f = std::chrono::high_resolution_clock::now();

  int seq_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>
                    (seq_t_f-seq_t_i).count();
  int par_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>
                    (par_t_f-par_t_i).count();

  std::cout << "Sequential runtime: " << seq_elapsed << " ms" << std::endl;
  std::cout << "Parallel runtime: " << par_elapsed << " ms" << std::endl;
  std::cout << "Speedup: " << (float)seq_elapsed/par_elapsed << "x" << std::endl;

  return 0;
}
