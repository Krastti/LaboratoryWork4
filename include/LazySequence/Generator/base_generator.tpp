#ifndef LABORATORYWORK4_BASE_GENERATOR_TPP
#define LABORATORYWORK4_BASE_GENERATOR_TPP

#include "../../Sequence/option.h"
#include "base_generator.h"
#include <stdexcept>

template <class T>
Option<T> BaseGenerator<T>::try_get_next() {
  if (!has_next()) {
    return Option<T>::none();
  }

  try {
    return Option<T>::some(get_next());
  } catch (const std::out_of_range&) {
    return Option<T>::none();
  }
}

#endif // LABORATORYWORK4_BASE_GENERATOR_TPP
