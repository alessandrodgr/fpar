#include "Backus.hpp"
#include <iostream>
#include <unistd.h>
#include <chrono>

using namespace fp;

// Stampa di atomi e/o sequenze (omogenee)
template <typename T>
ObjectPtr print (const ObjectPtr& x) {
  if(x == Bottom) return Bottom;
  if(x->type() == ATOM) {
    std::cout << *(Atom<T>::to_atom(x));
  } else if(x->type() == SEQUENCE) {
    auto s = Sequence::to_sequence(x);
    for(auto& el : *s) {
      print<T>(el);
      std::cout << ' ';
    }
    std::cout << std::endl;
  }
  return Bottom;
}

auto select1(const SequencePtr& x) {
  return select<Sequence>(1)(x);
}

auto select2AndTrans(const SequencePtr& x) {
  return compose(trans, select<Sequence>(2))(x);
}

int main(int argc, char const *argv[]) {
  // costruzione delle matrici
  auto M = Sequence::make_sequence();
  auto N = Sequence::make_sequence();
  for(int i = 0; i < 100; i++) {
    auto m = Sequence::make_sequence();
    auto n = Sequence::make_sequence();
    for(int j = 0; j < 100; j++) {
      m->push_back(Atom<int>::make_atom(i));
      n->push_back(Atom<int>::make_atom(i+j));
    }
    M->push_back(m);
    N->push_back(n);
  }
  auto MN = Sequence::make_sequence({M,N});

  // versione parallela di IP e MM
  auto IP = compose(insert(add<int>), compose(apply_to_all<Sequence>(mul<int>), trans));
  auto MM = compose(apply_to_all<Sequence>(apply_to_all<Sequence>(IP)),
                    compose(apply_to_all<Sequence>(distl()),
                            compose(distr(), construct({select1, select2AndTrans}))));

  // versione totalmente sequenziale di IP e MM
  auto seq_IP = compose(insert(add<int>),
                        compose(apply_to_all<Sequence>(mul<int>, false), trans));
  auto seq_MM = compose(apply_to_all<Sequence>(apply_to_all<Sequence>(IP, false), false),
                        compose(apply_to_all<Sequence>(distl(false), false),
                                compose(distr(false), construct({select1, select2AndTrans}, false))));

  // moltiplicazione sequenziale
  std::chrono::high_resolution_clock::time_point seq_t_i, seq_t_f;
  seq_t_i = std::chrono::high_resolution_clock::now();
  seq_MM(MN);
  seq_t_f = std::chrono::high_resolution_clock::now();
  int seq_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>
                    (seq_t_f-seq_t_i).count();

  // moltiplicazione parallela
  std::chrono::high_resolution_clock::time_point par_t_i, par_t_f;
  par_t_i = std::chrono::high_resolution_clock::now();
  MM(MN);
  par_t_f = std::chrono::high_resolution_clock::now();
  int par_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>
                    (par_t_f-par_t_i).count();

  // risultati
  std::cout << "Sequential runtime: " << seq_elapsed << " ms" << std::endl;
  std::cout << "Parallel runtime: " << par_elapsed << " ms" << std::endl;
  std::cout << "Speedup: " << seq_elapsed/par_elapsed << "x" << std::endl;

  return 0;
}
