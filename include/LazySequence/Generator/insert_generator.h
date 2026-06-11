#ifndef LABORATORYWORK4_INSERT_GENERATOR_H
#define LABORATORYWORK4_INSERT_GENERATOR_H

#include "base_generator.h"
#include "source_generator.h"
#include "Sequence/sequence.h"
#include "LazySequence/cardinal.h"

#include <cstddef>

/**
 * Генератор операции insert: выдает исходную последовательность с вставкой
 * одного или нескольких элементов, или другой генератор в указанную позицию.
 */
template <class T>
class InsertGenerator : public BaseGenerator<T> {
private:
  BaseGenerator<T>* source;
  BaseGenerator<T>* inserted;
  std::size_t insert_index;
  bool insert_at_infinity;
  bool inserted_finished;
  std::size_t source_position;

  void clear();
  void copy_from(const InsertGenerator<T> &other);

public:
  /**
   * Создает генератор insert для одного вставляемого элемента.
   */
  InsertGenerator(const Sequence<T> &source, std::size_t index, const T &item);

  /**
   * Создает генератор insert для конечной последовательности вставляемых элементов.
   */
  InsertGenerator(const Sequence<T> &source, std::size_t index, const Sequence<T> &items);

  /**
   * Создает ленивый insert для другого генератора.
   * Поддерживает бесконечные source и items.
   */
  InsertGenerator(const BaseGenerator<T> &source, std::size_t index, const BaseGenerator<T> &items);

  /**
   * Создает ленивый insert с индексом Cardinal.
   * Cardinal::infinity() означает вставку после всех конечных позиций source.
   */
  InsertGenerator(const BaseGenerator<T> &source, const Cardinal &index, const BaseGenerator<T> &items);

  InsertGenerator(const InsertGenerator<T> &other);

  InsertGenerator<T>& operator=(const InsertGenerator<T> &other);

  /**
   * Возвращает следующий элемент результата операции insert.
   */
  T get_next() override;

  /**
   * Проверяет, остались ли элементы исходной или вставленной части.
   */
  bool has_next() const override;

  /**
   * Создает копию insert-генератора с сохранением текущей позиции.
   */
  BaseGenerator<T>* clone() const override;

  ~InsertGenerator() override;
};

#include "insert_generator.tpp"

#endif // LABORATORYWORK4_INSERT_GENERATOR_H
