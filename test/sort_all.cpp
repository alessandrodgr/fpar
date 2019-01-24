#include "fpar.hpp"
#include <iostream>
#include <unistd.h>
#include <chrono>

using namespace fpar;

// istanzio il type system con il supporto per le stringhe
using String = Object<std::string>;

/*
  Costruisce una lista di T liste (dove T è il risultato di omp_get_max_threads()).
  Le T liste sono liste di stringhe.
  Il programma applica ad ognuna delle T liste la funzione sortAll, che ordina
  le stringhe contenute nelle liste.

  In sintassi FP:
    sortAll == a (a sort)
*/


std::string random_string( size_t length );

int main(int argc, char const *argv[]) {

  omp_set_num_threads(atoi(argv[1]));

  // costruisco l'input
  auto in = Sequence<String>();
  for(int i = 0; i < atoi(argv[2]); i++) {
    auto v = Sequence<String>();
    for(size_t j = 0; j < 10000; j++) {
      std::move(v).push_back(random_string(1000));
    }
    std::move(in).push_back(v);
  }

  auto sortAll =
    apply_to_all<seq_exec, String>([=](const String& x) {
      std::string s = x;
      std::sort(s.begin(), s.end());
      return s;
    });

  std::chrono::high_resolution_clock::time_point seq_t_i, seq_t_f;
  std::chrono::high_resolution_clock::time_point par_t_i, par_t_f;

  seq_t_i = std::chrono::high_resolution_clock::now();
  auto sr = apply_to_all<seq_exec, String>(sortAll)(in);
  seq_t_f = std::chrono::high_resolution_clock::now();


  par_t_i = std::chrono::high_resolution_clock::now();
  auto pr = apply_to_all<par_exec, String>(sortAll)(in);
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

std::string random_string( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}
