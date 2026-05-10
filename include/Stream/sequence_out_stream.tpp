#ifndef LABORATORYWORK4_SEQUENCE_OUT_STREAM_TPP
#define LABORATORYWORK4_SEQUENCE_OUT_STREAM_TPP

#include "sequence_out_stream.h"

#include <stdexcept>

template <class T>
SequenceOutStream<T>::SequenceOutStream(Sequence<T>* destination) : destination(destination) {
  if (destination == nullptr) {
    throw std::invalid_argument("Нельзя создать поток из нулевой последовательности");
  }

  this->position = static_cast<std::size_t>(destination->get_length());
}

template <class T>
Sequence<T>* SequenceOutStream<T>::get_destination() const {
  return destination;
}

template <class T>
std::size_t SequenceOutStream<T>::write(const T &item) {
  if (!this->is_open()) {
    throw std::logic_error("Поток не открыт");
  }

  destination = destination->append(item);
  this->position++;
  return this->position;
}

#endif // LABORATORYWORK4_SEQUENCE_OUT_STREAM_TPP
