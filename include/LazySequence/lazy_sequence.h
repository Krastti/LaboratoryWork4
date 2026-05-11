#ifndef LABORATORYWORK4_LAZY_SEQUENCE_H
#define LABORATORYWORK4_LAZY_SEQUENCE_H

#include <cstddef>
#include <functional>
#include "cardinal.h"
#include "generator.h"
#include "../Sequence/array_sequence.h"
#include "../Sequence/sequence.h"

template <class T>
class LazySequence : public Sequence<T> {
private:
  mutable Sequence<T>* materialized;
  Cardinal length;
  mutable Generator<T>* generator;

  static Sequence<T>* make_from_array(const T* items, int count);

  /**
   * Проверяет рекуррентное правило и копирует начальный контекст
   */
  static Sequence<T>* make_initial_for_recurrence(
    T (*recurrence)(Sequence<T>*),
    const Sequence<T> &initial
  );

  /**
   * Проверяет std::function-рекуррентное правило и копирует начальный контекст
   */
  static Sequence<T>* make_initial_for_recurrence(
    const std::function<T(Sequence<T>*)> &recurrence,
    Sequence<T>* initial
  );

  /**
   * Задача данного метода заключается в гарантировании того, что некий элемент index
   * уже лежит в materialized, то есть если мы вызываем элемент, индекс которого
   * больше последнего индекса элемента в materialized, то данная функция должна
   * вычислить элемент до нужно индекса
   */
  void materialize_until(int index) const;

  /*
   * По аналогии с прошлой функции, но только для следующего элемента.
   */
  bool try_materialize_next() const;

  Option<T> try_find(bool (*predicate)(const T &element)) const override;
  EnumeratorWrapper<T> get_enumerator() const override;

  /**
   * Создает последовательность из уже подготовленных внутренних частей.
   * Конструктор забирает владение materialized и generator.
   */
  LazySequence(Sequence<T>* materialized, Cardinal length, Generator<T>* generator);

protected:
  void sys_append(const T &item) override;
  Sequence<T>* new_empty_instance() const override;

  static Sequence<T>* copy_sequence(const Sequence<T>* seq);

public:

  LazySequence();
  LazySequence(const T* items, int count);
  LazySequence(Sequence<T>* seq);

  /**
   * Создает бесконечную последовательность по рекуррентному правилу и начальному контексту
   */
  LazySequence(T (*recurrence)(Sequence<T>*), const Sequence<T> &initial);

  /**
   * Создает бесконечную последовательность с заданным размером контекстного окна генератора
   */
  LazySequence(std::function<T(Sequence<T>*)> recurrence, Sequence<T>* initial, std::size_t context_size);

  /**
   * Создает независимую копию другой ленивой последовательности
   */
  LazySequence(const LazySequence<T> &other);

  const T& get_first() const override;
  const T& get_last() const override;
  const T& get(int index) const override;
  const T& operator[](int index) const override;

  Option<T> try_get_first() const override;
  Option<T> try_get_last() const override;
  Option<T> try_get(int index) const override;

  Sequence<T>* get_sub_sequence(int startIndex, int endIndex) const override;

  int get_length() const override;
  Cardinal get_cardinal_length() const;
  std::size_t get_materialized_count() const;

  // Операции
  Sequence<T>* append(const T &item) override;
  Sequence<T>* prepend(const T &item) override;
  LazySequence<T>* insert_at(const T &item, int index) override;

  LazySequence<T>* remove_at(int index);
  LazySequence<T>* concat(LazySequence<T>* other);

  template <class T2>
  LazySequence<T2>* map(T2 (*func)(const T &item));

  template <class T2>
  T2 reduce(T2 (*func)(const T2 &accumulator, const T &current), const T2 &initial_element);

  template <class T2>
  T2 reduce(T2 (*func)(const T2 &accumulator, const T &current), const T2 &initial_element, std::size_t limit);

  LazySequence<T>* where(bool (*predicate)(const T &item));

  template <class T2>
  LazySequence<Pair<T, T2>>* zip(Sequence<T2>* seq);

  template <class T2>
  LazySequence<Pair<T, T2>>* zip(LazySequence<T2>* seq);

  ~LazySequence() override;
};

#include "lazy_sequence.tpp"

#endif // LABORATORYWORK4_LAZY_SEQUENCE_H
