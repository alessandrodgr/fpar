#include "../Backus.hpp"
#include <iostream>
#include <unistd.h>
#include <chrono>

using namespace fp;

using Basic = Object<int, bool, float, std::string>;

inline Basic foo (const Basic& x) {
  std::this_thread::sleep_for (std::chrono::seconds(1));
  return Bottom;
}

int main(int argc, char const *argv[]) {
  auto in = Sequence<Basic>();
  for (int i = 0; i < 8; i++) {
    std::move(in).push_back(i);
  }

  std::chrono::high_resolution_clock::time_point seq_t_i, seq_t_f;
  std::chrono::high_resolution_clock::time_point par_t_i, par_t_f;

  seq_t_i = std::chrono::high_resolution_clock::now();
  apply_to_all<Basic>(foo, false)((Basic)in);
  seq_t_f = std::chrono::high_resolution_clock::now();

  par_t_i = std::chrono::high_resolution_clock::now();
  apply_to_all<Basic>(foo, true)((Basic)in);
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
