#ifndef LABORATORYWORK4_SOURCE_GENERATOR_TPP
#define LABORATORYWORK4_SOURCE_GENERATOR_TPP

#include <stdexcept>

template <class T>
SourceGenerator<T>::SourceGenerator(const Sequence<T> &source)
  : source(),
    position(0) {
  for (int index = 0; index < source.get_length(); index++) {
    this->source.append(source.get(index));
  }
}

template <class T>
T SourceGenerator<T>::get_next() {
  if (!has_next()) {
    throw std::out_of_range("Достигнут конец исходного генератора");
  }

  T item = source.get(static_cast<int>(position));
  position++;
  return item;
}

template <class T>
bool SourceGenerator<T>::has_next() const {
  return position < static_cast<std::size_t>(source.get_length());
}

template <class T>
BaseGenerator<T>* SourceGenerator<T>::clone() const {
  return new SourceGenerator<T>(*this);
}

#endif // LABORATORYWORK4_SOURCE_GENERATOR_TPP
