#ifndef LABORATORYWORK2_SEQUENCE_H
#define LABORATORYWORK2_SEQUENCE_H

#include "option.h"
#include "ienumerator.h"

template <class T>
class Sequence {
protected:
  virtual void sys_append(const T &item) = 0;
  virtual Sequence<T>* new_empty_instance() const = 0;

public:
  virtual const T& get_first() const = 0;
  virtual const T& get_last() const = 0;
  virtual const T& get(int index) const = 0;
  virtual const T& operator[](int index) const = 0;

  virtual Option<T> try_get_first() const = 0;
  virtual Option<T> try_get_last() const = 0;
  virtual Option<T> try_get(int index) const = 0;
  virtual Option<T> try_find(bool (*predicate)(const T &element)) const = 0;

  virtual int get_length() const = 0;

  virtual Sequence<T>* get_sub_sequence(int startIndex, int endIndex) const;

  virtual Sequence<T>* append(const T &item) = 0;
  virtual Sequence<T>* prepend(const T &item) = 0;
  virtual Sequence<T>* insert_at(const T &item, int index) = 0;

  virtual Sequence<T>* concat(const Sequence<T> &other) const;
  virtual Sequence<T>* map(T (*func)(const T &item)) const;
  virtual Sequence<T>* map(T (*func)(const T &item, int index)) const;
  virtual Sequence<T>* map(T (*func)(const T &item, const T &parameter), const T &parameter) const;
  template <class T2>
  Sequence<T2>* map(T2 (*func)(const T &item)) const;
  virtual Sequence<T>* where(bool (*predicate)(const T &element)) const;
  virtual T reduce(T (*func)(const T &accumulator, const T &current), const T &initialElement) const;

  virtual EnumeratorWrapper<T> get_enumerator() const = 0;

  virtual ~Sequence() {}
};

template <class T>
class MutableArraySequence;

template <class TFirst, class TSecond>
class Pair {
private:
  TFirst first_value;
  TSecond second_value;

public:
  Pair();
  Pair(const TFirst &first, const TSecond &second);

  const TFirst& first() const;
  const TSecond& second() const;

  bool operator==(const Pair<TFirst, TSecond> &other) const;
  bool operator!=(const Pair<TFirst, TSecond> &other) const;
};

template <class TFirst, class TSecond>
Sequence<Pair<TFirst, TSecond>>* zip(const Sequence<TFirst> &first, const Sequence<TSecond> &second);

#include "sequence.tpp"

#endif // LABORATORYWORK2_SEQUENCE_H
