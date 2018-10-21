#include "../Backus.hpp"
#include <iostream>
#include <unistd.h>
#include <chrono>

using namespace fp;

// istanzio il type system con il supporto per int e long
using Integer = Object<int, long>;

/*

Il programma mappa i numeri pari in booleani (pari->true, dispari->false),
mappa i booleani in interi (true->1, false->0),
esegue un'operazione di fold sulla lista di interi (somma)

risultato: numero di pari nella sequenza di input

def isEven = eq0 . mod2 -> true; false; Bottom

def toInt = eq . [id, true] -> 1; 0; Bottom

def mapEvens = a toInt . a isEven

def countEvens = /+ . mapEvens

*/

inline Integer tt (const Integer& x) {
  return constant<Integer>(Integer(true))(x);
}

int main(int argc, char const *argv[]) {

  auto in = Sequence<Integer>();
  for (long i = 0; i < 1000000; i++) {
    std::move(in).push_back(i);
  }

  auto toInt = [=](bool par) {
    return [=](const Integer& x) {
      auto y = construct<Integer>(
                {id<Integer>, tt},
                par
               )(x);
      bool eq = equals<bool>(y);
      if (eq) return Integer(1l);
      else return Integer(0l);
    };
  };

  auto isEven = [=](const Integer& x) {
    long mod2 = (long)x % 2;
    return equals<long>((Integer)Sequence<Integer>({mod2, 0l}));
  };

  auto mapEvens = [=](bool par) {
    return compose(
            apply_to_all<Integer>(toInt(par), par),
            apply_to_all<Integer>(isEven, par)
          );
  };

  auto countEvens = [=](bool par) {
    return compose(
            insert(add_op<long, Integer>, par, Integer(0l)),
            mapEvens(par)
          );
  };

  std::chrono::high_resolution_clock::time_point seq_t_i, seq_t_f;
  std::chrono::high_resolution_clock::time_point par_t_i, par_t_f;

  seq_t_i = std::chrono::high_resolution_clock::now();
  long sr = countEvens(false)(in);
  seq_t_f = std::chrono::high_resolution_clock::now();
  std::cout << sr << std::endl;

  par_t_i = std::chrono::high_resolution_clock::now();
  long pr = countEvens(true)(in);
  par_t_f = std::chrono::high_resolution_clock::now();
  std::cout << pr << std::endl;

  int seq_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>
                    (seq_t_f-seq_t_i).count();
  int par_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>
                    (par_t_f-par_t_i).count();

  std::cout << "Sequential runtime: " << seq_elapsed << " ms" << std::endl;
  std::cout << "Parallel runtime: " << par_elapsed << " ms" << std::endl;
  std::cout << "Speedup: " << (float)seq_elapsed/par_elapsed << "x" << std::endl;

  return 0;
}
