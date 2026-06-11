#ifndef LABORATORYWORK4_REMOVE_GENERATOR_H
#define LABORATORYWORK4_REMOVE_GENERATOR_H

#include "base_generator.h"
#include "Sequence/array_sequence.h"
#include "Sequence/sequence.h"

#include <cstddef>

/**
 * Генератор операции remove: выдает исходную последовательность, пропуская удаляемый диапазон.
 */
template <class T>
class RemoveGenerator : public BaseGenerator<T> {
private:
  MutableArraySequence<T> source;
  std::size_t remove_index;
  std::size_t remove_count;
  std::size_t position;

  /**
   * Переводит позицию результата в позицию исходной последовательности.
   */
  std::size_t source_index() const;

public:
  /**
   * Создает генератор remove для удаления одного элемента.
   */
  RemoveGenerator(const Sequence<T> &source, std::size_t index);

  /**
   * Создает генератор remove для удаления count элементов, начиная с index.
   */
  RemoveGenerator(const Sequence<T> &source, std::size_t index, std::size_t count);

  /**
   * Возвращает следующий элемент результата операции remove.
   */
  T get_next() override;

  /**
   * Проверяет, остались ли элементы после удаления заданного диапазона.
   */
  bool has_next() const override;

  /**
   * Создает копию remove-генератора с сохранением текущей позиции.
   */
  BaseGenerator<T>* clone() const override;
};

#include "remove_generator.tpp"

#endif // LABORATORYWORK4_REMOVE_GENERATOR_H
