#ifndef LABORATORYWORK2_LIST_SEQUENCE_H
#define LABORATORYWORK2_LIST_SEQUENCE_H

#include "linked_list.h"
#include "sequence.h"

template <class T>
class ListSequence : public Sequence<T> {
protected:
  LinkedList<T> list;

  void sys_append(const T &item) override;
  Sequence<T>* new_empty_instance() const override;

  virtual ListSequence<T>* instance() = 0;
  virtual ListSequence<T>* empty_clone() const = 0;
public:
  ListSequence();
  ListSequence(const T* items, int count);
  ListSequence(const LinkedList<T> &other);
  ListSequence(const ListSequence<T> &other);

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
  Sequence<T>* insert_at(const T &item, int index) override;

  EnumeratorWrapper<T> get_enumerator() const override {
    return list.get_enumerator();
  }

  ~ListSequence() {}
};

template <class T>
class MutableListSequence : public ListSequence<T> {
protected:
  ListSequence<T>* instance() override;
  ListSequence<T>* empty_clone() const override;

public:
  using ListSequence<T>::operator[];

  MutableListSequence();
  MutableListSequence(const T* items, int count);
  MutableListSequence(const LinkedList<T> &other);
  MutableListSequence(const ListSequence<T> &other);
  MutableListSequence(const MutableListSequence<T> &other);

  T& operator[](int index);

  ~MutableListSequence() {}
};

template <class T>
class ImmutableListSequence : public ListSequence<T> {
protected:
  ListSequence<T>* instance() override;
  ListSequence<T>* empty_clone() const override;

public:
  ImmutableListSequence();
  ImmutableListSequence(const T* items, int count);
  ImmutableListSequence(const LinkedList<T> &other);
  ImmutableListSequence(const ListSequence<T> &other);
  ImmutableListSequence(const ImmutableListSequence<T> &other);

  ImmutableListSequence<T>* clone() const;

  ~ImmutableListSequence() {}
};

#include "list_sequence.tpp"

#endif // LABORATORYWORK2_LIST_SEQUENCE_H
