#ifndef LABORATORYWORK4_IN_STREAM_H
#define LABORATORYWORK4_IN_STREAM_H

#include <cstddef>
#include <functional>
#include <string>

#include "base_stream.h"
#include "../LazySequence/lazy_sequence.h"
#include "../Sequence/sequence.h"

template <class T>
class InStream : public BaseStream<T> {
public:
  /**
   * Функция преобразования строкового представления элемента в объект типа T
   */
  using Deserializer = std::function<T(const std::string&)>;

  /**
   * Создаёт пустую базовую часть входного потока
   */
  InStream() = default;

  /**
   * Создаёт входной поток, читающий элементы из Sequence
   */
  InStream(Sequence<T>* source) {}

  /**
   * Создаёт входной поток, читающий элементы из LazySequence
   */
  InStream(LazySequence<T>* source) {}

  /**
   * Создаёт входной поток, читающий элементы из файла через десериализатор
   */
  InStream(const char* file_path, Deserializer deserializer) {}

  /**
   * Создаёт входной поток, читающий элементы из строки через десериализатор
   */
  InStream(const std::string &source, Deserializer deserializer) {}

  /**
   * Создаёт входной поток на основе другого входного потока
   */
  InStream(InStream<T>* source) {}

  /**
   * Возвращает флаг достижения конца потока
   */
  virtual bool is_end_of_stream() const = 0;

  /**
   * Считывает текущий элемент и переводит поток на следующую позицию
   */
  virtual T read() = 0;

  /**
   * Возвращает флаг возможности перейти к позиции без последовательного чтения
   */
  virtual bool is_can_seek() const = 0;

  /**
   * Переводит поток на указанную позицию и возвращает фактическую позицию
   */
  virtual std::size_t seek(std::size_t index) = 0;

  /**
   * Возвращает флаг возможности вернуться к ранее считанной позиции
   */
  virtual bool is_can_go_back() const = 0;

  virtual ~InStream() {}
};

#endif // LABORATORYWORK4_IN_STREAM_H
