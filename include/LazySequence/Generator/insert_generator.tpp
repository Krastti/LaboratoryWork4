#ifndef LABORATORYWORK4_INSERT_GENERATOR_TPP
#define LABORATORYWORK4_INSERT_GENERATOR_TPP

#include "Sequence/array_sequence.h"

#include <stdexcept>

template <class T>
void InsertGenerator<T>::clear() {
  delete source;
  delete inserted;

  source = nullptr;
  inserted = nullptr;
}

template <class T>
void InsertGenerator<T>::copy_from(const InsertGenerator<T> &other) {
  BaseGenerator<T>* source_copy = other.source == nullptr ? nullptr : other.source->clone();
  BaseGenerator<T>* inserted_copy = nullptr;

  try {
    inserted_copy = other.inserted == nullptr ? nullptr : other.inserted->clone();
  } catch (...) {
    delete source_copy;
    throw;
  }

  source = source_copy;
  inserted = inserted_copy;
  insert_index = other.insert_index;
  insert_at_infinity = other.insert_at_infinity;
  inserted_finished = other.inserted_finished;
  source_position = other.source_position;
}

template <class T>
InsertGenerator<T>::InsertGenerator(const Sequence<T> &source, std::size_t index, const T &item)
  : source(nullptr),
    inserted(nullptr),
    insert_index(index),
    insert_at_infinity(false),
    inserted_finished(false),
    source_position(0) {
  if (index > static_cast<std::size_t>(source.get_length())) {
    throw std::out_of_range("Индекс вставки находится вне последовательности");
  }

  MutableArraySequence<T> item_sequence;
  item_sequence.append(item);

  try {
    this->source = new SourceGenerator<T>(source);
    inserted = new SourceGenerator<T>(item_sequence);
  } catch (...) {
    clear();
    throw;
  }
}

template <class T>
InsertGenerator<T>::InsertGenerator(const Sequence<T> &source, std::size_t index, const Sequence<T> &items)
  : source(nullptr),
    inserted(nullptr),
    insert_index(index),
    insert_at_infinity(false),
    inserted_finished(false),
    source_position(0) {
  if (index > static_cast<std::size_t>(source.get_length())) {
    throw std::out_of_range("Индекс вставки находится вне последовательности");
  }

  try {
    this->source = new SourceGenerator<T>(source);
    inserted = new SourceGenerator<T>(items);
  } catch (...) {
    clear();
    throw;
  }
}

template <class T>
InsertGenerator<T>::InsertGenerator(
  const BaseGenerator<T> &source,
  std::size_t index,
  const BaseGenerator<T> &items
)
  : source(nullptr),
    inserted(nullptr),
    insert_index(index),
    insert_at_infinity(false),
    inserted_finished(false),
    source_position(0) {
  try {
    this->source = source.clone();
    inserted = items.clone();
  } catch (...) {
    clear();
    throw;
  }
}

template <class T>
InsertGenerator<T>::InsertGenerator(
  const BaseGenerator<T> &source,
  const Cardinal &index,
  const BaseGenerator<T> &items
)
  : source(nullptr),
    inserted(nullptr),
    insert_index(index.is_finite() ? index.value() : 0),
    insert_at_infinity(index.is_infinite()),
    inserted_finished(false),
    source_position(0) {
  try {
    this->source = source.clone();
    inserted = items.clone();
  } catch (...) {
    clear();
    throw;
  }
}

template <class T>
InsertGenerator<T>::InsertGenerator(const InsertGenerator<T> &other)
  : source(nullptr),
    inserted(nullptr),
    insert_index(0),
    insert_at_infinity(false),
    inserted_finished(false),
    source_position(0) {
  copy_from(other);
}

template <class T>
InsertGenerator<T>& InsertGenerator<T>::operator=(const InsertGenerator<T> &other) {
  if (this == &other) {
    return *this;
  }

  clear();
  copy_from(other);

  return *this;
}

template <class T>
T InsertGenerator<T>::get_next() {
  if (!has_next()) {
    throw std::out_of_range("Достигнут конец insert-генератора");
  }

  if (insert_at_infinity) {
    if (source->has_next()) {
      source_position++;
      return source->get_next();
    }

    return inserted->get_next();
  }

  if (source_position < insert_index) {
    source_position++;
    return source->get_next();
  }

  if (!inserted_finished) {
    if (inserted->has_next()) {
      return inserted->get_next();
    }

    inserted_finished = true;
  }

  source_position++;
  return source->get_next();
}

template <class T>
bool InsertGenerator<T>::has_next() const {
  if (source == nullptr || inserted == nullptr) {
    return false;
  }

  if (insert_at_infinity) {
    return source->has_next() || inserted->has_next();
  }

  if (source_position < insert_index) {
    return source->has_next();
  }

  return (!inserted_finished && inserted->has_next()) || source->has_next();
}

template <class T>
BaseGenerator<T>* InsertGenerator<T>::clone() const {
  return new InsertGenerator<T>(*this);
}

template <class T>
InsertGenerator<T>::~InsertGenerator() {
  clear();
}

#endif // LABORATORYWORK4_INSERT_GENERATOR_TPP
