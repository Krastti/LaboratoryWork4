#include "LazySequence/Generator/append_generator.h"
#include "LazySequence/lazy_sequence.h"
#include "LazySequence/cardinal.h"
#include "LazySequence/cardinal_io.h"
#include "Sequence/array_sequence.h"

#include <iostream>

int main() {
  int left_start[] = {0};
  MutableArraySequence<int> left_initial(left_start, 1);
  LazySequence<int> left_lazy(
    [](Sequence<int>* context) {
      int last_index = context->get_length() - 1;
      return context->get(last_index) + 10;
    },
    left_initial
  );

  int right_start[] = {1};
  MutableArraySequence<int> right_initial(right_start, 1);
  LazySequence<int> right_lazy(
    [](Sequence<int>* context) {
      int last_index = context->get_length() - 1;
      return context->get(last_index) + 1;
    },
    right_initial
  );

  LazySequence<int>* inserted_lazy = left_lazy.insert_at(&right_lazy, Cardinal::omega());

  std::cout << "left[0] = " << inserted_lazy->get(0) << '\n';
  std::cout << "left[4] = " << inserted_lazy->get(4) << '\n';
  std::cout << "right[" << Cardinal::omega() << "] = " << inserted_lazy->get(Cardinal::omega()) << '\n';
  std::cout << "right[" << Cardinal::omega_plus(10) << "] = " << inserted_lazy->get(Cardinal::omega_plus(10)) << '\n';

  delete inserted_lazy;

  return 0;
}
