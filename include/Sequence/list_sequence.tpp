#include "list_sequence.h"

template <class T>
void ListSequence<T>::sys_append(const T &item) {
  list.append(item);
}

template <class T>
Sequence<T>* ListSequence<T>::new_empty_instance() const {
  return empty_clone();
}

template <class T>
ListSequence<T>::ListSequence() : list() {}

template <class T>
ListSequence<T>::ListSequence(const T* items, int count) : list() {
  if (count < 0) throw std::out_of_range("Длина должна быть больше, либо равна нулю!");

  for (int i = 0; i < count; i++) {
    list.append(items[i]);
  }
}

template <class T>
ListSequence<T>::ListSequence(const LinkedList<T> &other) : list(other) {}

template <class T>
ListSequence<T>::ListSequence(const ListSequence<T> &other) : list(other.list) {}

template <class T>
const T& ListSequence<T>::get_first() const {
  return list.get_first();
}

template <class T>
const T& ListSequence<T>::get_last() const {
  return list.get_last();
}

template <class T>
const T& ListSequence<T>::get(int index) const {
  return *list.get(index);
}

template <class T>
const T& ListSequence<T>::operator[](int index) const {
  return list[index];
}

template <class T>
Option<T> ListSequence<T>::try_get_first() const {
  if (list.get_length() == 0) return Option<T>::none();

  return Option<T>::some(list.get_first());
}

template <class T>
Option<T> ListSequence<T>::try_get_last() const {
  if (list.get_length() == 0) return Option<T>::none();

  return Option<T>::some(list.get_last());
}

template <class T>
Option<T> ListSequence<T>::try_get(int index) const {
  if (index < 0) return Option<T>::none();
  if (index >= list.get_length()) return Option<T>::none();

  return Option<T>::some(*list.get(index));
}

template <class T>
Option<T> ListSequence<T>::try_find(bool (*predicate)(const T &element)) const {
  if (predicate == nullptr) throw std::invalid_argument("Нельзя выполнить try_find с нулевым предикатом");

  EnumeratorWrapper<T> iter = get_enumerator();

  while (iter.move_next()) {
    const T& item = iter.get_current();

    if (predicate(item)) {
      return Option<T>::some(item);
    }
  }

  return Option<T>::none();
}

template <class T>
int ListSequence<T>::get_length() const {
  return list.get_length();
}

template <class T>
Sequence<T>* ListSequence<T>::append(const T &item) {
  ListSequence<T>* currentInstance = instance();
  currentInstance->list.append(item);

  return currentInstance;
}

template <class T>
Sequence<T>* ListSequence<T>::prepend(const T &item) {
  ListSequence<T>* currentInstance = instance();
  currentInstance->list.prepend(item);

  return currentInstance;
}

template <class T>
Sequence<T>* ListSequence<T>::insert_at(const T &item, int index) {
  ListSequence<T>* currentInstance = instance();
  currentInstance->list.insert_at(item, index);

  return currentInstance;
}

template <class T>
ListSequence<T>* MutableListSequence<T>::instance() {
  return this;
}

template <class T>
ListSequence<T>* MutableListSequence<T>::empty_clone() const {
  return new MutableListSequence<T>();
}

template <class T>
MutableListSequence<T>::MutableListSequence() : ListSequence<T>() {}

template <class T>
MutableListSequence<T>::MutableListSequence(const T* items, int count) : ListSequence<T>(items, count) {}

template <class T>
MutableListSequence<T>::MutableListSequence(const LinkedList<T> &other) : ListSequence<T>(other) {}

template <class T>
MutableListSequence<T>::MutableListSequence(const ListSequence<T> &other) : ListSequence<T>(other) {}

template <class T>
MutableListSequence<T>::MutableListSequence(const MutableListSequence<T> &other) : ListSequence<T>(other) {}

template <class T>
T& MutableListSequence<T>::operator[](int index) {
  return this->list[index];
}

template <class T>
ListSequence<T>* ImmutableListSequence<T>::instance() {
  return clone();
}

template <class T>
ListSequence<T>* ImmutableListSequence<T>::empty_clone() const {
  return new ImmutableListSequence<T>();
}

template <class T>
ImmutableListSequence<T>::ImmutableListSequence() : ListSequence<T>() {}

template <class T>
ImmutableListSequence<T>::ImmutableListSequence(const T* items, int count) : ListSequence<T>(items, count) {}

template <class T>
ImmutableListSequence<T>::ImmutableListSequence(const LinkedList<T> &other) : ListSequence<T>(other) {}

template <class T>
ImmutableListSequence<T>::ImmutableListSequence(const ListSequence<T> &other) : ListSequence<T>(other) {}

template <class T>
ImmutableListSequence<T>::ImmutableListSequence(const ImmutableListSequence<T> &other) : ListSequence<T>(other) {}

template <class T>
ImmutableListSequence<T>* ImmutableListSequence<T>::clone() const {
  return new ImmutableListSequence<T>(*this);
}
