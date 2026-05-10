#ifndef LABORATORYWORK4_GENERATOR_TPP
#define LABORATORYWORK4_GENERATOR_TPP

#include "generator.h"

#include <stdexcept>

template <class T>
Sequence<T>* Generator<T>::copy_sequence(const Sequence<T>* items) {
  MutableArraySequence<T>* copy = new MutableArraySequence<T>();

  if (items == nullptr) {
    return copy;
  }

  for (int index = 0; index < items->get_length(); index++) {
    copy->append(items->get(index));
  }

  return copy;
}

template <class T>
void Generator<T>::copy_to_context(const Sequence<T>* items) {
  if (items == nullptr) {
    return;
  }

  for (int index = 0; index < items->get_length(); index++) {
    generated_context.push_back(items->get(index));
  }
}

template <class T>
void Generator<T>::clear_change() {
  delete change_items;
  change_items = nullptr;
  has_change = false;
  change_index = 0;
  change_offset = 0;
  remove_count = 0;
}

template <class T>
T Generator<T>::get_source_next() {
  if (!has_rule) {
    throw std::out_of_range("Достигнут конец генератора");
  }

  // Правило генерации принимает Sequence, поэтому контекст из CircularBuffer
  // временно преобразуется в последовательность для вызова rule
  Sequence<T>* ctx = generated_context.to_sequence();

  try {
    T item = rule(ctx);
    delete ctx;
    generated_context.push_back(item);
    return item;
  } catch (...) {
    delete ctx;
    throw;
  }
}

template <class T>
bool Generator<T>::should_emit_change_item() const {
  if (!has_change || change_kind == GeneratorChangeKind::Remove) {
    return false;
  }

  if (change_items == nullptr || change_offset >= static_cast<std::size_t>(change_items->get_length())) {
    return false;
  }

  if (change_kind == GeneratorChangeKind::Append) {
    return !has_rule;
  }

  return position >= change_index;
}

template <class T>
bool Generator<T>::should_skip_source_item() const {
  return has_change
    && change_kind == GeneratorChangeKind::Remove
    && position >= change_index
    && change_offset < remove_count;
}

template <class T>
Generator<T>::Generator(LazySequence<T>* owner, Rule rule, std::size_t context_size)
  : Generator(owner, rule, nullptr, context_size) {}

template <class T>
Generator<T>::Generator(
  LazySequence<T>* owner,
  Rule rule,
  Sequence<T>* initial_context,
  std::size_t context_size
)
  : owner(owner),
    rule(rule),
    generated_context(context_size),
    position(0),
    has_rule(static_cast<bool>(rule)),
    has_change(false),
    change_kind(GeneratorChangeKind::Insert),
    change_index(0),
    change_items(nullptr),
    change_offset(0),
    remove_count(0) {
  if (!rule) {
    throw std::invalid_argument("Порождающее правило не может быть пустым");
  }

  copy_to_context(initial_context);
}

template <class T>
Generator<T>::Generator(
  LazySequence<T>* owner,
  std::size_t index,
  const T &item,
  GeneratorChangeKind change_kind
)
  : owner(owner),
    rule(),
    generated_context(1),
    position(0),
    has_rule(false),
    has_change(true),
    change_kind(change_kind),
    change_index(index),
    change_items(new MutableArraySequence<T>()),
    change_offset(0),
    remove_count(change_kind == GeneratorChangeKind::Remove ? 1 : 0) {
  if (change_kind != GeneratorChangeKind::Remove) {
    change_items->append(item);
  }
}

template <class T>
Generator<T>::Generator(
  LazySequence<T>* owner,
  std::size_t index,
  Sequence<T>* items,
  GeneratorChangeKind change_kind
)
  : owner(owner),
    rule(),
    generated_context(1),
    position(0),
    has_rule(false),
    has_change(true),
    change_kind(change_kind),
    change_index(index),
    change_items(copy_sequence(items)),
    change_offset(0),
    remove_count(0) {
  if (change_kind == GeneratorChangeKind::Remove && items != nullptr) {
    remove_count = static_cast<std::size_t>(items->get_length());
  } else {
    remove_count = 0;
  }
}

template <class T>
Generator<T>::Generator(const Generator<T> &generator)
  : Generator(generator, generator.owner) {}

template <class T>
Generator<T>::Generator(const Generator<T> &generator, LazySequence<T>* owner)
  : owner(owner),
    rule(generator.rule),
    generated_context(generator.generated_context),
    position(generator.position),
    has_rule(generator.has_rule),
    has_change(generator.has_change),
    change_kind(generator.change_kind),
    change_index(generator.change_index),
    change_items(copy_sequence(generator.change_items)),
    change_offset(generator.change_offset),
    remove_count(generator.remove_count) {}

template <class T>
Generator<T>& Generator<T>::operator=(const Generator<T> &generator) {
  if (this == &generator) {
    return *this;
  }

  delete change_items;

  owner = generator.owner;
  rule = generator.rule;
  generated_context = generator.generated_context;
  position = generator.position;
  has_rule = generator.has_rule;
  has_change = generator.has_change;
  change_kind = generator.change_kind;
  change_index = generator.change_index;
  change_items = copy_sequence(generator.change_items);
  change_offset = generator.change_offset;
  remove_count = generator.remove_count;

  return *this;
}

template <class T>
T Generator<T>::get_next() {
  while (should_skip_source_item()) {
    get_source_next();
    change_offset++;
  }

  if (should_emit_change_item()) {
    T item = change_items->get(static_cast<int>(change_offset));
    change_offset++;
    position++;
    return item;
  }

  T item = get_source_next();
  position++;
  return item;
}

template <class T>
bool Generator<T>::has_next() const {
  if (should_emit_change_item()) {
    return true;
  }

  if (has_rule) {
    return true;
  }

  return false;
}

template <class T>
Option<T> Generator<T>::try_get_next() {
  if (!has_next()) {
    return Option<T>::none();
  }

  try {
    return Option<T>::some(get_next());
  } catch (const std::out_of_range&) {
    return Option<T>::none();
  }
}

template <class T>
Generator<T>* Generator<T>::append(const T &item) const {
  Generator<T>* result = new Generator<T>(*this);
  result->clear_change();
  result->has_change = true;
  result->change_kind = GeneratorChangeKind::Append;
  result->change_items = new MutableArraySequence<T>();
  result->change_items->append(item);
  return result;
}

template <class T>
Generator<T>* Generator<T>::append(Sequence<T>* items) const {
  Generator<T>* result = new Generator<T>(*this);
  result->clear_change();
  result->has_change = true;
  result->change_kind = GeneratorChangeKind::Append;
  result->change_items = copy_sequence(items);
  return result;
}

template <class T>
Generator<T>* Generator<T>::insert_at(std::size_t index, const T &item) const {
  Generator<T>* result = new Generator<T>(*this);
  result->clear_change();
  result->has_change = true;
  result->change_kind = GeneratorChangeKind::Insert;
  result->change_index = index;
  result->change_items = new MutableArraySequence<T>();
  result->change_items->append(item);
  return result;
}

template <class T>
Generator<T>* Generator<T>::insert_at(std::size_t index, Sequence<T>* items) const {
  Generator<T>* result = new Generator<T>(*this);
  result->clear_change();
  result->has_change = true;
  result->change_kind = GeneratorChangeKind::Insert;
  result->change_index = index;
  result->change_items = copy_sequence(items);
  return result;
}

template <class T>
Generator<T>* Generator<T>::remove_at(std::size_t index) const {
  return remove_at(index, 1);
}

template <class T>
Generator<T>* Generator<T>::remove_at(std::size_t index, std::size_t count) const {
  Generator<T>* result = new Generator<T>(*this);
  result->clear_change();
  result->has_change = true;
  result->change_kind = GeneratorChangeKind::Remove;
  result->change_index = index;
  result->change_offset = 0;
  result->remove_count = count;
  return result;
}

template <class T>
Generator<T>::~Generator() {
  delete change_items;
}

#endif // LABORATORYWORK4_GENERATOR_TPP
