#include "fpar.hpp"
#include <iostream>
#include <unistd.h>
#include <chrono>

using namespace fpar;

// istanzio il type system con il supporto per int, bool, float e std::string
using Basic = Object<int, bool, float, std::string>;

// per qualsiasi x aspetta 1 secondo e restituisce Bottom
inline Basic foo (const Basic& x) {
  std::this_thread::sleep_for (std::chrono::seconds(1));
  return Bottom;
}

int main(int argc, char const *argv[]) {
  omp_set_num_threads(atoi(argv[1]));
  auto in = Sequence<Basic>();
  for (int i = 0; i < atoi(argv[1]); i++) {
    std::move(in).push_back(i);
  }

  std::chrono::high_resolution_clock::time_point seq_t_i, seq_t_f;
  std::chrono::high_resolution_clock::time_point par_t_i, par_t_f;

  seq_t_i = std::chrono::high_resolution_clock::now();
  apply_to_all<seq_exec, Basic>(foo)((Basic)in);
  seq_t_f = std::chrono::high_resolution_clock::now();

  par_t_i = std::chrono::high_resolution_clock::now();
  apply_to_all<par_exec, Basic>(foo)((Basic)in);
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
