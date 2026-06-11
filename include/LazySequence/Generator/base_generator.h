#ifndef LABORATORYWORK4_BASE_GENERATOR_H
#define LABORATORYWORK4_BASE_GENERATOR_H

#include "Sequence/option.h"

/**
 * Базовый интерфейс генератора.
 */
template <class T>
class BaseGenerator {
public:
  /**
   * Возвращает следующий элемент генератора.
   * Если элементов больше нет, конкретная реализация должна бросить std::out_of_range.
   */
  virtual T get_next() = 0;

  /**
   * Проверяет, может ли генератор вернуть хотя бы один следующий элемент.
   */
  virtual bool has_next() const = 0;

  /**
   * Безопасно пытается получить следующий элемент генератора.
   * Возвращает Option::none(), если генератор завершен.
   */
  virtual Option<T> try_get_next();

  /**
   * Создает независимую копию генератора с сохранением текущей позиции.
   */
  virtual BaseGenerator<T>* clone() const = 0;

  /**
   * Виртуальный деструктор нужен для корректного удаления наследников через BaseGenerator.
   */
  virtual ~BaseGenerator() {}
};

#include "base_generator.tpp"

#endif // LABORATORYWORK4_BASE_GENERATOR_H
