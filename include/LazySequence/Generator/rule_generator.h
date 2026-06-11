#ifndef LABORATORYWORK4_RULE_GENERATOR_H
#define LABORATORYWORK4_RULE_GENERATOR_H

#include "base_generator.h"
#include "LazySequence/circular_buffer.h"
#include "Sequence/sequence.h"

#include <functional>

/**
 * Генератор, который вычисляет следующий элемент по правилу от уже
 * сгенерированного контекста.
 */
template <class T>
class RuleGenerator : public BaseGenerator<T> {
private:
  using Rule = std::function<T(Sequence<T>*)>;

  Rule rule;
  CircularBuffer<T> context;

public:
  /**
   * Создает генератор по функции-правилу без начального контекста.
   */
  RuleGenerator(Rule rule);

  /**
   * Создает генератор по функции-правилу и копирует initial_context
   * во внутренний кольцевой контекст генерации.
   */
  RuleGenerator(Rule rule, const Sequence<T> &initial_context);

  /**
   * Возвращает следующий элемент, вычисленный правилом, и добавляет его в контекст.
   */
  T get_next() override;

  /**
   * Проверяет, задано ли правило генерации.
   */
  bool has_next() const override;

  /**
   * Создает копию генератора с тем же правилом и текущим контекстом.
   */
  BaseGenerator<T>* clone() const override;
};

#include "rule_generator.tpp"

#endif // LABORATORYWORK4_RULE_GENERATOR_H
