#ifndef LABORATORYWORK4_IN_STREAM_H
#define LABORATORYWORK4_IN_STREAM_H

#include <cstddef>
#include <functional>
#include <sstream>
#include <stdexcept>
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
   * Преобразует строку в T через стандартный оператор чтения >>.
   */
  static T default_deserialize(const std::string &source) {
    std::istringstream input(source);
    T value;

    input >> value;

    if (input.fail()) {
      throw std::invalid_argument("Не удалось десериализовать значение");
    }

    return value;
  }

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

template <>
inline std::string InStream<std::string>::default_deserialize(const std::string &source) {
  return source;
}

#endif // LABORATORYWORK4_IN_STREAM_H
