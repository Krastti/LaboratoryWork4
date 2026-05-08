#ifndef LABORATORYWORK2_ARRAY_SEQUENCE_H
#define LABORATORYWORK2_ARRAY_SEQUENCE_H

#include "sequence.h"
#include "dynamic_array.h"

template <class T>
class ArraySequence : public Sequence<T> {
protected:
  DynamicArray<T> array;
  int count;

  void sys_append(const T &item) override;
  Sequence<T>* new_empty_instance() const override;

  virtual ArraySequence<T>* instance() = 0;
  virtual ArraySequence<T>* empty_clone() const = 0;
public:
  ArraySequence();
  ArraySequence(const T* items, int count);
  ArraySequence(const DynamicArray<T> &other);
  ArraySequence(const ArraySequence<T> &other);

  const T& get_first() const override;
  const T& get_last() const override;
  const T& get(int index) const override;
  const T& operator[](int index) const override;

  Option<T> try_get_first() const override;
  Option<T> try_get_last() const override;
  Option<T> try_get(int index) const override;
  Option<T> try_find(bool (*predicate)(const T &element)) const override;

  int get_length() const override;

  Sequence<T>* append(const T &item) override;
  Sequence<T>* prepend(const T &item) override;
  Sequence<T>* insert_at(const T &item, int index) override;;

  EnumeratorWrapper<T> get_enumerator() const override {
    return array.get_enumerator(count);
  }

  ~ArraySequence() {}
};

template <class T>
class MutableArraySequence : public ArraySequence<T> {
protected:
  ArraySequence<T>* instance() override;
  ArraySequence<T>* empty_clone() const override;

public:
  using ArraySequence<T>::operator[];

  MutableArraySequence();
  MutableArraySequence(const T* items, int count);
  MutableArraySequence(const DynamicArray<T> &array);
  MutableArraySequence(const ArraySequence<T> &array);
  MutableArraySequence(const MutableArraySequence<T> &other);

  T& operator[](int index);

  ~MutableArraySequence() {}
};

template <class T>
class ImmutableArraySequence : public ArraySequence<T> {
protected:
  ArraySequence<T>* instance() override;
  ArraySequence<T>* empty_clone() const override;

public:
  ImmutableArraySequence();
  ImmutableArraySequence(const T* items, int count);
  ImmutableArraySequence(const DynamicArray<T> &array);
  ImmutableArraySequence(const ArraySequence<T> &array);
  ImmutableArraySequence(const ImmutableArraySequence<T> &other);

  ImmutableArraySequence<T>* clone() const;

  ~ImmutableArraySequence() {}
};

#include "array_sequence.tpp"

#endif // LABORATORYWORK2_ARRAY_SEQUENCE_H
