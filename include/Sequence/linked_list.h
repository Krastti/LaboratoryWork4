#ifndef LABORATORYWORK2_LINKED_LIST_H
#define LABORATORYWORK2_LINKED_LIST_H

#include "ienumerator.h"

template <class T>
class LinkedList {
private:
  struct Node {
    T data;
    Node* next;
  };

  Node* head;
  Node* tail;
  int length;

  Node* node_at(int index);
  const Node* node_at(int index) const;
  
  class Enumerator : public IEnumerator<T> {

  private:
    Node* head;
    Node* current;
    bool started;

  public:
    Enumerator(Node* head) : head(head), current(nullptr), started(false) {}

    bool move_next() override {
      if (!started) {
        current = head;
        started = true;
      } else {
        current = current->next;
      }

      return current != nullptr;
    }

    const T& get_current() const override {
      return current->data;
    }

    void reset() override {
      current = nullptr;
      started = false;
    }
  };

public:
  LinkedList();
  LinkedList(T* items, int count);
  LinkedList(const LinkedList<T> &other);

  LinkedList<T>& operator=(const LinkedList<T> &other);

  const T& get_first() const;
  const T& get_last() const;
  const T* get(int index) const;
  const T& operator[](int index) const;
  T& operator[](int index);
  int get_length() const;

  LinkedList<T>* get_sub_list(int startIndex, int endIndex);

  void append(const T& item);
  void prepend(const T& item);
  void insert_at(const T& item, int index);

  LinkedList<T>* concat(const LinkedList<T>* other);

  EnumeratorWrapper<T> get_enumerator() const {
    return EnumeratorWrapper<T>(new Enumerator(head));
  }

  ~LinkedList();
};

#include "linked_list.tpp"

#endif //LABORATORYWORK2_LINKED_LIST_H
