#ifndef LABORATORYWORK4_APPEND_GENERATOR_H
#define LABORATORYWORK4_APPEND_GENERATOR_H

#include "base_generator.h"
#include "Sequence/array_sequence.h"
#include "Sequence/sequence.h"

#include <cstddef>

/**
 * Генератор операции append: сначала выдает исходную последовательность,
 * затем добавленные элементы.
 */
template <class T>
class AppendGenerator : public BaseGenerator<T> {
private:
  MutableArraySequence<T> source;
  MutableArraySequence<T> appended;
  std::size_t position;

public:
  /**
   * Создает генератор append для одного добавляемого элемента.
   */
  AppendGenerator(const Sequence<T> &source, const T &item);

  /**
   * Создает генератор append для последовательности добавляемых элементов.
   */
  AppendGenerator(const Sequence<T> &source, const Sequence<T> &items);

  /**
   * Возвращает следующий элемент результата операции append.
   */
  T get_next() override;

  /**
   * Проверяет, остались ли элементы в исходной или добавленной части.
   */
  bool has_next() const override;

  /**
   * Создает копию append-генератора с сохранением текущей позиции.
   */
  BaseGenerator<T>* clone() const override;
};

#include "append_generator.tpp"

#endif // LABORATORYWORK4_APPEND_GENERATOR_H
