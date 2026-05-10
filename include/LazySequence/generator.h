#ifndef LABORATORYWORK4_GENERATOR_H
#define LABORATORYWORK4_GENERATOR_H

#include <cstddef>
#include <functional>

#include "../Sequence/array_sequence.h"
#include "../Sequence/option.h"
#include "../Sequence/sequence.h"
#include "circular_buffer.h"

template <class T>
class LazySequence;

/**
 * Данный класс показывает, какое именно изменение должен учитывать генератор
 */
enum class GeneratorChangeKind {
  Append,
  Insert,
  Remove
};

template <class T>
class Generator {
private:
  // Rule - это псевдоним, чтобы не загрязнять сигнатуру функций
  /**
   * Правило получает уже сгенерированный контекст в виде Sequence
   * и по нему вычисляет следующий элемент последовательности
   */
  using Rule = std::function<T(Sequence<T>*)>;

  LazySequence<T>* owner;
  Rule rule;
  CircularBuffer<T> generated_context;
  std::size_t position; // Текущая позиция генератора в результирующей последовательности

  bool has_rule;

  bool has_change;
  GeneratorChangeKind change_kind;
  std::size_t change_index;
  Sequence<T>* change_items;
  std::size_t change_offset; // Текущий сдвиг внутри change_items
  std::size_t remove_count;


  //Статический метод для копирования последовательности
  static Sequence<T>* copy_sequence(const Sequence<T>* items);

  // Метод, который копирует начальную последовательность в generated context
  void copy_to_context(const Sequence<T>* items);

  void clear_change();
  T get_source_next();

  // Метод, который проверяет, нужно ли выдавать элемент из change_items
  bool should_emit_change_item() const;

  // Метод, который проверяет, нужно ли пропустить исходный элемент из-за операции удаления
  bool should_skip_source_item() const;

public:
  // Конструктор создаёт новый генератор с правилом, но без начального контекста
  Generator(LazySequence<T>* owner, Rule rule, std::size_t context_size = 1);

  // Конструктор генератора по правилу
  Generator(
    LazySequence<T>* owner,
    Rule rule,
    Sequence<T>* initial_context,
    std::size_t context_size = 1
  );

  // Конструктор генератора с одним элементом
  Generator(
    LazySequence<T>* owner,
    std::size_t index,
    const T &item,
    GeneratorChangeKind change_kind
  );

  // Конструктор генератора с последовательностью элементов
  Generator(
    LazySequence<T>* owner,
    std::size_t index,
    Sequence<T>* items,
    GeneratorChangeKind change_kind
  );

  Generator(const Generator<T> &generator);
  Generator(const Generator<T> &generator, LazySequence<T>* owner);

  Generator<T>& operator=(const Generator<T> &generator);

  T get_next();
  bool has_next() const;
  Option<T> try_get_next();

  Generator<T>* append(const T &item) const;
  Generator<T>* append(Sequence<T>* items) const;

  Generator<T>* insert_at(std::size_t index, const T &item) const;
  Generator<T>* insert_at(std::size_t index, Sequence<T>* items) const;

  Generator<T>* remove_at(std::size_t index) const;
  Generator<T>* remove_at(std::size_t index, std::size_t count) const;

  ~Generator();
};

#include "generator.tpp"

#endif // LABORATORYWORK4_GENERATOR_H
