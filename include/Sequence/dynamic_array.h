#ifndef LABORATORYWORK2_DYNAMIC_ARRAY_H
#define LABORATORYWORK2_DYNAMIC_ARRAY_H

#include "ienumerator.h"

template <class T>
class DynamicArray {
private:
  T* data;
  int size;

  class Enumerator : public IEnumerator<T> {
  private:
    T* data;
    int count;
    int index;

  public:
    Enumerator(T* data, int count) : data(data), count(count), index(-1) {}

    bool move_next() override {
      index++;
      return index < count;
    }

    const T& get_current() const override {
      return data[index];
    }

    void reset() override {
      index = -1;
    }
  };

public:
  DynamicArray();
  DynamicArray(const T* items, int count);
  DynamicArray(int size);
  DynamicArray(const DynamicArray<T> &other);

  DynamicArray<T>& operator=(const DynamicArray<T> &other);

  const T& get(int index) const;
  const T& operator[](int index) const;
  T& operator[](int index);
  int get_size() const;

  void set(int index, const T& value);
  void resize(int newSize);

  EnumeratorWrapper<T> get_enumerator() const {
    return EnumeratorWrapper<T>(new Enumerator(data, size));
  }

  EnumeratorWrapper<T> get_enumerator(int count) const {
    return EnumeratorWrapper<T>(new Enumerator(data, count));
  }

  ~DynamicArray();
};

#include "dynamic_array.tpp"

#endif // LABORATORYWORK2_DYNAMIC_ARRAY_H
