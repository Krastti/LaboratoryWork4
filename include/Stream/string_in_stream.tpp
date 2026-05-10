#ifndef LABORATORYWORK4_STRING_IN_STREAM_TPP
#define LABORATORYWORK4_STRING_IN_STREAM_TPP

#include "string_in_stream.h"

#include <sstream>
#include <stdexcept>

template <class T>
StringInStream<T>::StringInStream(const std::string &source) : StringInStream(source, InStream<T>::default_deserialize) {}

template <class T>
StringInStream<T>::StringInStream(
  const std::string &source,
  typename InStream<T>::Deserializer deserializer
) : deserializer(deserializer) {
  if (!deserializer) {
    throw std::invalid_argument("Десериализатор не может быть пустым");
  }

  std::istringstream input(source);
  std::string line;

  while (std::getline(input, line)) {
    lines.append(line);
  }
}

template <class T>
bool StringInStream<T>::is_end_of_stream() const {
  return this->position >= static_cast<std::size_t>(lines.get_length());
}

template <class T>
T StringInStream<T>::read() {
  if (!this->is_open()) {
    throw std::logic_error("Поток не открыт");
  }

  if (is_end_of_stream()) {
    throw std::out_of_range("Достигнут конец потока");
  }

  T item = deserializer(lines.get(static_cast<int>(this->position)));
  this->position++;
  return item;
}

template <class T>
bool StringInStream<T>::is_can_seek() const {
  return true;
}

template <class T>
std::size_t StringInStream<T>::seek(std::size_t index) {
  if (index > static_cast<std::size_t>(lines.get_length())) {
    throw std::out_of_range("Позиция вне допустимого диапазона");
  }

  this->position = index;
  return this->position;
}

template <class T>
bool StringInStream<T>::is_can_go_back() const {
  return true;
}

#endif // LABORATORYWORK4_STRING_IN_STREAM_TPP
