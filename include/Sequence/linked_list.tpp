#include <stdexcept>
#include "linked_list.h"

template <class T>
LinkedList<T>::LinkedList() : head(nullptr), tail(nullptr), length(0) {}

template <class T>
LinkedList<T>::LinkedList(T *items, int count) : head(nullptr), tail(nullptr), length(count) {
  if (length < 0) throw std::out_of_range("Длина должна быть больше, либо равна нулю!");
  if (length == 0) return;

  head = new Node{items[0], nullptr};
  tail = head;

  for (int index = 1; index < length; index++) {
    tail->next = new Node{items[index], nullptr};
    tail = tail->next;
  }
}

template <class T>
LinkedList<T>::LinkedList(const LinkedList<T> &other) : head(nullptr), tail(nullptr), length(other.length) {
  if (other.head == nullptr) return;

  head = new Node{other.head->data, nullptr};
  tail = head;

  Node* current = other.head->next;

  while (current != nullptr) {
    tail->next = new Node{current->data, nullptr};
    tail = tail->next;
    current = current->next;
  }
}

template <class T>
LinkedList<T>& LinkedList<T>::operator=(const LinkedList<T> &other) {
  if (this == &other) return *this;

  Node* current = head;

  while (current != nullptr) {
    Node* temp = current;
    current = current->next;
    delete temp;
  }

  head = nullptr;
  tail = nullptr;
  length = 0;

  current = other.head;

  while (current != nullptr) {
    append(current->data);
    current = current->next;
  }

  return *this;
}

template <class T>
typename LinkedList<T>::Node* LinkedList<T>::node_at(int index) {
  if (index >= length) throw std::out_of_range("Индекс вне допустимого диапазона");
  if (index < 0) throw std::out_of_range("Индекс вне допустимого диапазона");

  Node* current = head;

  for (int idx = 0; idx < index; idx++) {
    current = current->next;
  }

  return current;
}

template <class T>
const typename LinkedList<T>::Node* LinkedList<T>::node_at(int index) const {
  if (index >= length) throw std::out_of_range("Индекс вне допустимого диапазона");
  if (index < 0) throw std::out_of_range("Индекс вне допустимого диапазона");

  const Node* current = head;

  for (int idx = 0; idx < index; idx++) {
    current = current->next;
  }

  return current;
}

template <class T>
const T& LinkedList<T>::get_first() const {
  if (length == 0) throw std::out_of_range("Индекс вне допустимого диапазона");

  return head->data;
}

template <class T>
const T& LinkedList<T>::get_last() const {
  if (length == 0) throw std::out_of_range("Индекс вне допустимого диапазона");

  return tail->data;
}

template <class T>
const T* LinkedList<T>::get(int index) const {
  return &node_at(index)->data;
}

template <class T>
const T& LinkedList<T>::operator[](int index) const {
  return node_at(index)->data;
}

template <class T>
T& LinkedList<T>::operator[](int index) {
  return node_at(index)->data;
}

template <class T>
int LinkedList<T>::get_length() const {
  return length;
}

template <class T>
LinkedList<T>* LinkedList<T>::get_sub_list(int startIndex, int endIndex) {
  if (startIndex < 0) throw std::out_of_range("Индекс вне допустимого диапазона");
  if (endIndex < 0) throw std::out_of_range("Индекс вне допустимого диапазона");
  if (startIndex >= length) throw std::out_of_range("Индекс вне допустимого диапазона");
  if (endIndex >= length) throw std::out_of_range("Индекс вне допустимого диапазона");
  if (startIndex > endIndex) throw std::out_of_range("Индекс вне допустимого диапазона");

  LinkedList<T>* subList = new LinkedList<T>();
  Node* current = head;

  for (int index = 0; index < startIndex; index++) {
    current = current->next;
  }

  for (int index = startIndex; index <= endIndex; index++) {
    subList->append(current->data);
    current = current->next;
  }

  return subList;
}

template <class T>
void LinkedList<T>::append(const T &item) {
  Node* newNode = new Node{item, nullptr};

  if (length == 0) {
    head = newNode;
    tail = newNode;
  } else {
    tail->next = newNode;
    tail = newNode;
  }

  length++;
}

template <class T>
void LinkedList<T>::prepend(const T &item) {
  Node* newNode = new Node{item, head};

  head = newNode;

  if (length == 0) {
    tail = newNode;
  }

  length++;
}

template <class T>
void LinkedList<T>::insert_at(const T &item, int index) {
  if (index > length) throw std::out_of_range("Индекс вне допустимого диапазона");
  if (index < 0) throw std::out_of_range("Индекс вне допустимого диапазона");

  if (index == 0) {
    prepend(item);
    return;
  }

  if (index == length) {
    append(item);
    return;
  }

  Node* current = head;

  for (int idx = 0; idx < index - 1; idx++) {
    current = current->next;
  }

  Node* newNode = new Node{item, current->next};
  current->next = newNode;
  length++;
}

template <class T>
LinkedList<T>* LinkedList<T>::concat(const LinkedList<T>* other) {
  if (other == nullptr) throw std::invalid_argument("Нельзя выполнить concat с нулевым указателем");

  LinkedList<T>* result = new LinkedList<T>();
  Node* current = head;

  while (current != nullptr) {
    result->append(current->data);
    current = current->next;
  }

  current = other->head;

  while (current != nullptr) {
    result->append(current->data);
    current = current->next;
  }

  return result;
}

template <class T>
LinkedList<T>::~LinkedList() {
  Node* current = head;

  while (current != nullptr) {
    Node* temp = current;
    current = current->next;
    delete temp;
  }
}
