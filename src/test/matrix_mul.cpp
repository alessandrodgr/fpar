#include "../Backus.hpp"
#include <iostream>
#include <unistd.h>
#include <chrono>

using namespace fp;

using Number = Object<int, double>;

inline Number select1 (const Number& x) {
  return select<Number>(1)(x);
}

inline Number select2AndTrans (const Number& x) {
  return trans(select<Number>(2)(x));
}

int main(int argc, char const *argv[]) {
  auto v = Sequence<Number>();
  for(size_t i = 0; i < 1000; i++) {
    auto w = Sequence<Number>();
    for(size_t j = 0; j < 100; j++) {
      std::move(w).push_back((int)(i+j));
    }
    std::move(v).push_back(w);
  }

  Number in = Sequence<Number>({v,v});

  auto IP = [=](bool par) {
    auto mul = mul_op<int, Number>;
    auto add = add_op<int, Number>;
    return compose(
            insert(add, par, Number(0)),
            compose(
              apply_to_all<Number>(mul, par),
              trans<Number>
            )
          );
  };

  auto MM = [=](bool par) {
    auto aIP = [=](const Number& y) {
      return apply_to_all<Number>(IP(par), par)(y);
    };

    return compose(
            apply_to_all<Number>(aIP, par),
            compose(
              apply_to_all<Number>(distl<Number>(par), par),
              compose(
                distr<Number>(par),
                construct<Number>({select1, select2AndTrans}, par)
              )
            )
          );
  };

  std::chrono::high_resolution_clock::time_point seq_t_i, seq_t_f;
  std::chrono::high_resolution_clock::time_point par_t_i, par_t_f;

  seq_t_i = std::chrono::high_resolution_clock::now();
  auto sr = MM(false)(in);
  seq_t_f = std::chrono::high_resolution_clock::now();


  par_t_i = std::chrono::high_resolution_clock::now();
  auto pr = MM(true)(in);
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
