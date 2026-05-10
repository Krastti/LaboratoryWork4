#ifndef LABORATORYWORK4_OUT_STREAM_H
#define LABORATORYWORK4_OUT_STREAM_H

#include <cstddef>
#include <functional>
#include <string>

#include "base_stream.h"
#include "../Sequence/sequence.h"

template <class T>
class OutStream : public BaseStream<T> {
public:
  /**
   * Функция преобразования объекта типа T в строковое представление
   */
  using Serializer = std::function<std::string(const T&)>;

  /**
   * Создаёт пустую базовую часть выходного потока
   */
  OutStream() = default;

  /**
   * Создаёт выходной поток, записывающий элементы в Sequence
   */
  OutStream(Sequence<T>* destination) {}

  /**
   * Создаёт выходной поток, записывающий элементы в файл через сериализатор
   */
  OutStream(const char* file_path, Serializer serializer) {}

  /**
   * Создаёт выходной поток на основе другого выходного потока
   */
  OutStream(OutStream<T>* destination) {}

  /**
   * Записывает элемент в поток и возвращает позицию следующей записи
   */
  virtual std::size_t write(const T &item) = 0;

  virtual ~OutStream() {}
};

#endif // LABORATORYWORK4_OUT_STREAM_H
