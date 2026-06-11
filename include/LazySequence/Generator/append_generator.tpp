#ifndef LABORATORYWORK4_APPEND_GENERATOR_TPP
#define LABORATORYWORK4_APPEND_GENERATOR_TPP

#include <stdexcept>

template <class T>
AppendGenerator<T>::AppendGenerator(const Sequence<T> &source, const T &item)
  : source(),
    appended(),
    position(0) {
  for (int index = 0; index < source.get_length(); index++) {
    this->source.append(source.get(index));
  }

  appended.append(item);
}

template <class T>
AppendGenerator<T>::AppendGenerator(const Sequence<T> &source, const Sequence<T> &items)
  : source(),
    appended(),
    position(0) {
  for (int index = 0; index < source.get_length(); index++) {
    this->source.append(source.get(index));
  }

  for (int index = 0; index < items.get_length(); index++) {
    appended.append(items.get(index));
  }
}

template <class T>
T AppendGenerator<T>::get_next() {
  if (!has_next()) {
    throw std::out_of_range("Достигнут конец append-генератора");
  }

  std::size_t source_length = static_cast<std::size_t>(source.get_length());
  T item = position < source_length
    ? source.get(static_cast<int>(position))
    : appended.get(static_cast<int>(position - source_length));

  position++;
  return item;
}

template <class T>
bool AppendGenerator<T>::has_next() const {
  return position < static_cast<std::size_t>(source.get_length() + appended.get_length());
}

template <class T>
BaseGenerator<T>* AppendGenerator<T>::clone() const {
  return new AppendGenerator<T>(*this);
}

#endif // LABORATORYWORK4_APPEND_GENERATOR_TPP
