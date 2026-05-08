#include <stdexcept>
#include "dynamic_array.h"

template <class T>
DynamicArray<T>::DynamicArray() : size(0) {
  data = new T[4];
}

template <class T>
DynamicArray<T>::DynamicArray(const T *items, int count) : size(count) {
  if (size < 0) throw std::out_of_range("Размер должен быть больше, либо равен нулю!");

  data = new T[size];

  for (int index = 0; index < size; index++) {
    data[index] = items[index];
  }
}

template <class T>
DynamicArray<T>::DynamicArray(int size) : size(size) {
  if (size < 0) throw std::out_of_range("Размер должен быть больше, либо равен нулю!");

  data = new T[size];
}

template <class T>
DynamicArray<T>::DynamicArray(const DynamicArray<T> &other) : size(other.size) {
  data = new T[size];

  for (int index = 0; index < size; index++) {
    data[index] = other.data[index];
  }
}

template <class T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T> &other) {
  if (this == &other) return *this;

  delete[] data;
  size = other.size;
  data = new T[size];

  for (int index = 0; index < size; index++) {
    data[index] = other.data[index];
  }

  return *this;
}

template <class T>
const T& DynamicArray<T>::get(int index) const {
  if (index >= size) throw std::out_of_range("Индекс вне допустимого диапазона");
  if (index < 0) throw std::out_of_range("Индекс вне допустимого диапазона");

  return data[index];
}

template <class T>
const T& DynamicArray<T>::operator[](int index) const {
  return get(index);
}

template <class T>
T& DynamicArray<T>::operator[](int index) {
  if (index >= size) throw std::out_of_range("Индекс вне допустимого диапазона");
  if (index < 0) throw std::out_of_range("Индекс вне допустимого диапазона");

  return data[index];
}

template <class T>
int DynamicArray<T>::get_size() const {
  return size;
}

template <class T>
void DynamicArray<T>::set(int index, const T &value) {
  if (index >= size) throw std::out_of_range("Индекс вне допустимого диапазона");
  if (index < 0) throw std::out_of_range("Индекс вне допустимого диапазона");

  data[index] = value;
}

template <class T>
void DynamicArray<T>::resize(int newSize) {
  if (newSize < 0) throw std::out_of_range("Размер должен быть больше, либо равен нулю!");

  T* newData = new T[newSize];

  int limit;
  if (newSize < size) {
    limit = newSize;
  } else {
    limit = size;
  }

  for (int index = 0; index < limit; index++) {
    newData[index] = data[index];
  }

  delete[] data;
  data = newData;
  size = newSize;
}

template <class T>
DynamicArray<T>::~DynamicArray() {
  delete[] data;
}
