#ifndef LABORATORYWORK4_REMOVE_GENERATOR_TPP
#define LABORATORYWORK4_REMOVE_GENERATOR_TPP

#include <stdexcept>

template <class T>
std::size_t RemoveGenerator<T>::source_index() const {
  return position < remove_index ? position : position + remove_count;
}

template <class T>
RemoveGenerator<T>::RemoveGenerator(const Sequence<T> &source, std::size_t index)
  : RemoveGenerator(source, index, 1) {}

template <class T>
RemoveGenerator<T>::RemoveGenerator(const Sequence<T> &source, std::size_t index, std::size_t count)
  : source(),
    remove_index(index),
    remove_count(count),
    position(0) {
  if (index > static_cast<std::size_t>(source.get_length())) {
    throw std::out_of_range("Индекс удаления находится вне последовательности");
  }

  if (index + count > static_cast<std::size_t>(source.get_length())) {
    throw std::out_of_range("Диапазон удаления находится вне последовательности");
  }

  for (int current = 0; current < source.get_length(); current++) {
    this->source.append(source.get(current));
  }
}

template <class T>
T RemoveGenerator<T>::get_next() {
  if (!has_next()) {
    throw std::out_of_range("Достигнут конец remove-генератора");
  }

  T item = source.get(static_cast<int>(source_index()));
  position++;
  return item;
}

template <class T>
bool RemoveGenerator<T>::has_next() const {
  std::size_t result_length = static_cast<std::size_t>(source.get_length()) - remove_count;
  return position < result_length;
}

template <class T>
BaseGenerator<T>* RemoveGenerator<T>::clone() const {
  return new RemoveGenerator<T>(*this);
}

#endif // LABORATORYWORK4_REMOVE_GENERATOR_TPP
