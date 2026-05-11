#ifndef LABORATORYWORK4_LAZY_SEQUENCE_IN_STREAM_TPP
#define LABORATORYWORK4_LAZY_SEQUENCE_IN_STREAM_TPP

#include "lazy_sequence_in_stream.h"

#include <limits>
#include <stdexcept>

template <class T>
void LazySequenceInStream<T>::check_position_supported(std::size_t index) const {
  if (index > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
    throw std::out_of_range("Позиция слишком велика для текущего интерфейса LazySequence");
  }
}

template <class T>
LazySequenceInStream<T>::LazySequenceInStream(LazySequence<T>* source) : source(source) {
  if (source == nullptr) {
    throw std::invalid_argument("Нельзя создать поток из нулевой ленивой последовательности");
  }
}

template <class T>
bool LazySequenceInStream<T>::is_end_of_stream() const {
  Cardinal length = source->get_cardinal_length();
  return length.is_finite() && this->position >= length.value();
}

template <class T>
T LazySequenceInStream<T>::read() {
  if (!this->is_open()) {
    throw std::logic_error("Поток не открыт");
  }

  if (is_end_of_stream()) {
    throw std::out_of_range("Достигнут конец потока");
  }

  check_position_supported(this->position);
  T item = source->get(static_cast<int>(this->position));
  this->position++;
  return item;
}

template <class T>
bool LazySequenceInStream<T>::is_can_seek() const {
  return true;
}

template <class T>
std::size_t LazySequenceInStream<T>::seek(std::size_t index) {
  Cardinal length = source->get_cardinal_length();

  if (length.is_finite() && index > length.value()) {
    throw std::out_of_range("Позиция вне допустимого диапазона");
  }

  check_position_supported(index);
  this->position = index;
  return this->position;
}

template <class T>
bool LazySequenceInStream<T>::is_can_go_back() const {
  return true;
}

#endif // LABORATORYWORK4_LAZY_SEQUENCE_IN_STREAM_TPP
