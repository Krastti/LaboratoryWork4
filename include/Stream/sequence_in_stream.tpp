#ifndef LABORATORYWORK4_SEQUENCE_IN_STREAM_TPP
#define LABORATORYWORK4_SEQUENCE_IN_STREAM_TPP

#include "sequence_in_stream.h"

#include <stdexcept>

template <class T>
SequenceInStream<T>::SequenceInStream(Sequence<T>* source) : source(source) {
  if (source == nullptr) {
    throw std::invalid_argument("Нельзя создать поток из нулевой последовательности");
  }
}

template <class T>
bool SequenceInStream<T>::is_end_of_stream() const {
  return this->position >= static_cast<std::size_t>(source->get_length());
}

template <class T>
T SequenceInStream<T>::read() {
  if (!this->is_open()) {
    throw std::logic_error("Поток не открыт");
  }

  if (is_end_of_stream()) {
    throw std::out_of_range("Достигнут конец потока");
  }

  T item = source->get(static_cast<int>(this->position));
  this->position++;
  return item;
}

template <class T>
bool SequenceInStream<T>::is_can_seek() const {
  return true;
}

template <class T>
std::size_t SequenceInStream<T>::seek(std::size_t index) {
  if (index > static_cast<std::size_t>(source->get_length())) {
    throw std::out_of_range("Позиция вне допустимого диапазона");
  }

  this->position = index;
  return this->position;
}

template <class T>
bool SequenceInStream<T>::is_can_go_back() const {
  return true;
}

#endif // LABORATORYWORK4_SEQUENCE_IN_STREAM_TPP
