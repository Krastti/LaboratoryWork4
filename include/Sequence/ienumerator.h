#ifndef LABORATORYWORK2_IENUMERATOR_H
#define LABORATORYWORK2_IENUMERATOR_H

template <class T>
class IEnumerator {
public:
  virtual bool move_next() = 0;
  virtual const T& get_current() const = 0;
  virtual void reset() = 0;

  virtual ~IEnumerator() {}
};

template <class T>
class EnumeratorWrapper {
private:
  IEnumerator<T>* iter;
  bool hasCurrent;

public:
  EnumeratorWrapper(IEnumerator<T>* iter) : iter(iter), hasCurrent(false) {}

  EnumeratorWrapper(const EnumeratorWrapper<T> &other) = delete;
  EnumeratorWrapper<T>& operator=(const EnumeratorWrapper<T> &other) = delete;

  EnumeratorWrapper(EnumeratorWrapper<T> &&other) : iter(other.iter), hasCurrent(other.hasCurrent) {
    other.iter = nullptr;
    other.hasCurrent = false;
  }

  EnumeratorWrapper<T>& operator=(EnumeratorWrapper<T> &&other) {
    if (this != &other) {
      delete iter;
      iter = other.iter;
      hasCurrent = other.hasCurrent;
      other.iter = nullptr;
      other.hasCurrent = false;
    }

    return *this;
  }

  bool move_next() {
    hasCurrent = iter->move_next();
    return hasCurrent;
  }

  const T& get_current() const {
    return iter->get_current();
  }

  void reset() {
    iter->reset();
    hasCurrent = false;
  }

  ~EnumeratorWrapper() {
    delete iter;
  }
};

#endif // LABORATORYWORK2_IENUMERATOR_H
