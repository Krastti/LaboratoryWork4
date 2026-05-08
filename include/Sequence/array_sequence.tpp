#include "array_sequence.h"

template <class T>
void ArraySequence<T>::sys_append(const T &item) {
  if (count >= array.get_size()) {
    int newSize = array.get_size() == 0 ? 4 : array.get_size() * 2;
    array.resize(newSize);
  }

  array.set(count, item);
  count++;
}

template <class T>
Sequence<T>* ArraySequence<T>::new_empty_instance() const {
  return empty_clone();
}

template <class T>
ArraySequence<T>::ArraySequence() : array(), count(0) {}

template <class T>
ArraySequence<T>::ArraySequence(const T* items, int count) : array(items, count), count(count) {}

template <class T>
ArraySequence<T>::ArraySequence(const DynamicArray<T> &other) : array(other), count(other.get_size()) {}

template <class T>
ArraySequence<T>::ArraySequence(const ArraySequence<T> &other) : array(other.array), count(other.count) {}

template <class T>
const T& ArraySequence<T>::get_first() const {
  if (count == 0) throw std::out_of_range("Индекс вне допустимого диапазона");

  return array.get(0);
}

template <class T>
const T& ArraySequence<T>::get_last() const {
  if (count == 0) throw std::out_of_range("Индекс вне допустимого диапазона");

  return array.get(count - 1);
}

template <class T>
const T& ArraySequence<T>::get(int index) const {
  if (index >= count) throw std::out_of_range("Индекс вне допустимого диапазона");
  if (index < 0) throw std::out_of_range("Индекс вне допустимого диапазона");

  return array.get(index);
}

template <class T>
const T& ArraySequence<T>::operator[](int index) const {
  return get(index);
}

template <class T>
Option<T> ArraySequence<T>::try_get_first() const {
  if (count == 0) return Option<T>::none();

  return Option<T>::some(array.get(0));
}

template <class T>
Option<T> ArraySequence<T>::try_get_last() const {
  if (count == 0) return Option<T>::none();

  return Option<T>::some(array.get(count - 1));
}

template <class T>
Option<T> ArraySequence<T>::try_get(int index) const {
  if (index < 0) return Option<T>::none();
  if (index >= count) return Option<T>::none();

  return Option<T>::some(array.get(index));
}

template <class T>
Option<T> ArraySequence<T>::try_find(bool (*predicate)(const T &element)) const {
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
int ArraySequence<T>::get_length() const {
  return count;
}

template <class T>
Sequence<T>* ArraySequence<T>::append(const T &item) {
  ArraySequence<T>* currentInstance = instance();
  currentInstance->sys_append(item);

  return currentInstance;
}

template <class T>
Sequence<T>* ArraySequence<T>::prepend(const T &item) {
  ArraySequence<T>* currentInstance = instance();

  if (currentInstance->count >= currentInstance->array.get_size()) {
    int newSize = currentInstance->array.get_size() == 0 ? 4 : currentInstance->array.get_size() * 2;
    currentInstance->array.resize(newSize);
  }

  for (int i = currentInstance->count; i > 0; i--) {
    currentInstance->array.set(i, currentInstance->array.get(i - 1));
  }

  currentInstance->array.set(0, item);
  currentInstance->count++;

  return currentInstance;
}

template <class T>
Sequence<T>* ArraySequence<T>::insert_at(const T &item, int index) {
  ArraySequence<T>* currentInstance = instance();

  if (index > currentInstance->count) throw std::out_of_range("Индекс вне допустимого диапазона");
  if (index < 0) throw std::out_of_range("Индекс вне допустимого диапазона");

  if (currentInstance->count >= currentInstance->array.get_size()) {
    int newSize = currentInstance->array.get_size() == 0 ? 4 : currentInstance->array.get_size() * 2;
    currentInstance->array.resize(newSize);
  }

  for (int i = currentInstance->count; i > index; i--) {
    currentInstance->array.set(i, currentInstance->array.get(i - 1));
  }

  currentInstance->array.set(index, item);
  currentInstance->count++;

  return currentInstance;
}

template <class T>
ArraySequence<T>* MutableArraySequence<T>::instance() {
  return this;
}

template <class T>
ArraySequence<T>* MutableArraySequence<T>::empty_clone() const {
  return new MutableArraySequence<T>();
}

template <class T>
MutableArraySequence<T>::MutableArraySequence() : ArraySequence<T>() {}

template <class T>
MutableArraySequence<T>::MutableArraySequence(const T* items, int count) : ArraySequence<T>(items, count) {}

template <class T>
MutableArraySequence<T>::MutableArraySequence(const DynamicArray<T> &array) : ArraySequence<T>(array) {}

template <class T>
MutableArraySequence<T>::MutableArraySequence(const ArraySequence<T> &array) : ArraySequence<T>(array) {}

template <class T>
MutableArraySequence<T>::MutableArraySequence(const MutableArraySequence<T> &other) : ArraySequence<T>(other) {}

template <class T>
T& MutableArraySequence<T>::operator[](int index) {
  if (index >= this->count) throw std::out_of_range("Индекс вне допустимого диапазона");
  if (index < 0) throw std::out_of_range("Индекс вне допустимого диапазона");

  return this->array[index];
}

template <class T>
ArraySequence<T>* ImmutableArraySequence<T>::instance() {
  return clone();
}

template <class T>
ArraySequence<T>* ImmutableArraySequence<T>::empty_clone() const {
  return new ImmutableArraySequence<T>();
}

template <class T>
ImmutableArraySequence<T>::ImmutableArraySequence() : ArraySequence<T>() {}

template <class T>
ImmutableArraySequence<T>::ImmutableArraySequence(const T* items, int count) : ArraySequence<T>(items, count) {}

template <class T>
ImmutableArraySequence<T>::ImmutableArraySequence(const DynamicArray<T> &array) : ArraySequence<T>(array) {}

template <class T>
ImmutableArraySequence<T>::ImmutableArraySequence(const ArraySequence<T> &array) : ArraySequence<T>(array) {}

template <class T>
ImmutableArraySequence<T>::ImmutableArraySequence(const ImmutableArraySequence<T> &other) : ArraySequence<T>(other) {}

template <class T>
ImmutableArraySequence<T>* ImmutableArraySequence<T>::clone() const {
  return new ImmutableArraySequence<T>(*this);
}
