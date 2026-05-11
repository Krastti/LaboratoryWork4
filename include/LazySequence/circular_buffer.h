#ifndef LABORATORYWORK4_CIRCULAR_BUFFER_H
#define LABORATORYWORK4_CIRCULAR_BUFFER_H

#include <cstddef>

#include "../Sequence/array_sequence.h"
#include "../Sequence/dynamic_array.h"
#include "../Sequence/sequence.h"

template <class T>
class CircularBuffer {
private:
  DynamicArray<T> data;
  std::size_t capacity; // максимальный размер кольца
  std::size_t count; // текущее количество
  std::size_t head; // индекс первого элемента кольца

  static int checked_capacity(std::size_t capacity);
  std::size_t physical_index(std::size_t logical_index) const;

public:
  CircularBuffer(std::size_t capacity);
  CircularBuffer(const CircularBuffer<T> &buffer);

  CircularBuffer<T>& operator=(const CircularBuffer<T> &buffer);

  std::size_t get_capacity() const;
  std::size_t get_count() const;
  bool is_empty() const;
  bool is_full() const;

  const T& get(std::size_t index) const;
  const T& operator[](std::size_t index) const;
  const T& get_first() const;
  const T& get_last() const;

  void push_back(const T &item); // Добавляет элемент в конец логической последовательности
  void pop_front(); // Удаляет первый логический элемент
  void clear(); // Сбрасывает состояние буфера

  /**
   * Создаёт новый MutableArraySequence<T>, копирует в него
   * элементы буфера в логическом порядке и возвращает указатель на базовый тип Sequence<T>
   */
  Sequence<T>* to_sequence() const;
};

#include "circular_buffer.tpp"

#endif // LABORATORYWORK4_CIRCULAR_BUFFER_H
