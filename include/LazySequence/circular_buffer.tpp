#ifndef LABORATORYWORK4_CIRCULAR_BUFFER_TPP
#define LABORATORYWORK4_CIRCULAR_BUFFER_TPP

#include "circular_buffer.h"

#include <limits>
#include <stdexcept>

template <class T>
int CircularBuffer<T>::checked_capacity(std::size_t capacity) {
  if (capacity == 0) {
    throw std::invalid_argument("Размер CircularBuffer должен быть больше нуля");
  }

  if (capacity > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
    throw std::out_of_range("Размер CircularBuffer слишком большой");
  }

  return static_cast<int>(capacity);
}

template <class T>
std::size_t CircularBuffer<T>::physical_index(std::size_t logical_index) const {
  if (logical_index >= count) {
    throw std::out_of_range("Индекс вне допустимого диапазона");
  }

  return (head + logical_index) % capacity;
}

template <class T>
CircularBuffer<T>::CircularBuffer(std::size_t capacity)
  : data(checked_capacity(capacity)),
    capacity(capacity),
    count(0),
    head(0) {}

template <class T>
CircularBuffer<T>::CircularBuffer(const CircularBuffer<T> &buffer)
  : data(buffer.data),
    capacity(buffer.capacity),
    count(buffer.count),
    head(buffer.head) {}

template <class T>
CircularBuffer<T>& CircularBuffer<T>::operator=(const CircularBuffer<T> &buffer) {
  if (this == &buffer) {
    return *this;
  }

  data = buffer.data;
  capacity = buffer.capacity;
  count = buffer.count;
  head = buffer.head;

  return *this;
}

template <class T>
std::size_t CircularBuffer<T>::get_capacity() const {
  return capacity;
}

template <class T>
std::size_t CircularBuffer<T>::get_count() const {
  return count;
}

template <class T>
bool CircularBuffer<T>::is_empty() const {
  return count == 0;
}

template <class T>
bool CircularBuffer<T>::is_full() const {
  return count == capacity;
}

template <class T>
const T& CircularBuffer<T>::get(std::size_t index) const {
  return data.get(static_cast<int>(physical_index(index)));
}

template <class T>
const T& CircularBuffer<T>::operator[](std::size_t index) const {
  return get(index);
}

template <class T>
const T& CircularBuffer<T>::get_first() const {
  if (is_empty()) {
    throw std::out_of_range("CircularBuffer пуст");
  }

  return get(0);
}

template <class T>
const T& CircularBuffer<T>::get_last() const {
  if (is_empty()) {
    throw std::out_of_range("CircularBuffer пуст");
  }

  return get(count - 1);
}

template <class T>
void CircularBuffer<T>::push_back(const T &item) {
  if (is_full()) {
    data.set(static_cast<int>(head), item);
    head = (head + 1) % capacity;
    return;
  }

  data.set(static_cast<int>((head + count) % capacity), item);
  count++;
}

template <class T>
void CircularBuffer<T>::pop_front() {
  if (is_empty()) {
    throw std::out_of_range("CircularBuffer пуст");
  }

  head = (head + 1) % capacity;
  count--;

  if (count == 0) {
    head = 0;
  }
}

template <class T>
void CircularBuffer<T>::clear() {
  count = 0;
  head = 0;
}

template <class T>
Sequence<T>* CircularBuffer<T>::to_sequence() const {
  MutableArraySequence<T>* sequence = new MutableArraySequence<T>();

  for (std::size_t index = 0; index < count; index++) {
    sequence->append(get(index));
  }

  return sequence;
}

#endif // LABORATORYWORK4_CIRCULAR_BUFFER_TPP
