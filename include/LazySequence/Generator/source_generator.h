#ifndef LABORATORYWORK4_SOURCE_GENERATOR_H
#define LABORATORYWORK4_SOURCE_GENERATOR_H

#include "base_generator.h"
#include "Sequence/array_sequence.h"
#include "Sequence/sequence.h"

#include <cstddef>

/**
 * Генератор, который последовательно читает элементы из существующей Sequence.
 */
template <class T>
class SourceGenerator : public BaseGenerator<T> {
private:
  MutableArraySequence<T> source;
  std::size_t position;

public:
  /**
   * Создает генератор и копирует в него элементы source.
   */
  explicit SourceGenerator(const Sequence<T> &source);

  /**
   * Возвращает очередной элемент скопированной исходной последовательности.
   */
  T get_next() override;

  /**
   * Проверяет, остались ли непрочитанные элементы исходной последовательности.
   */
  bool has_next() const override;

  /**
   * Создает копию генератора с той же исходной последовательностью и текущей позицией.
   */
  BaseGenerator<T>* clone() const override;
};

#include "source_generator.tpp"

#endif // LABORATORYWORK4_SOURCE_GENERATOR_H
