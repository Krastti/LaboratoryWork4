#ifndef LABORATORYWORK4_CARDINAL_H
#define LABORATORYWORK4_CARDINAL_H

#include <cstddef>
#include <stdexcept>

class Cardinal {
private:
  std::size_t finite_value;
  bool infinite;

public:
  Cardinal() : finite_value(0), infinite(false) {}

  Cardinal(std::size_t value) : finite_value(value), infinite(false) {}

  static Cardinal infinity() {
    Cardinal value;
    value.infinite = true;
    return value;
  }

  bool is_finite() const {
    return !infinite;
  }

  bool is_infinite() const {
    return infinite;
  }

  std::size_t value() const {
    if (infinite) {
      throw std::logic_error("Бесконечное кардинальное число не имеет конечного значения");
    }

    return finite_value;
  }

  Cardinal& operator+=(const Cardinal &other) {
    if (infinite || other.infinite) {
      infinite = true;
      finite_value = 0;
      return *this;
    }

    finite_value += other.finite_value;
    return *this;
  }

  friend Cardinal operator+(Cardinal left, const Cardinal &right);
  friend bool operator==(const Cardinal &left, const Cardinal &right);
  friend bool operator!=(const Cardinal &left, const Cardinal &right);
  friend bool operator<(const Cardinal &left, const Cardinal &right);
  friend bool operator<=(const Cardinal &left, const Cardinal &right);
  friend bool operator>(const Cardinal &left, const Cardinal &right);
  friend bool operator>=(const Cardinal &left, const Cardinal &right);
};

inline Cardinal operator+(Cardinal left, const Cardinal &right) {
  left += right;
  return left;
}

inline bool operator==(const Cardinal &left, const Cardinal &right) {
  if (left.infinite || right.infinite) {
    return left.infinite == right.infinite;
  }

  return left.finite_value == right.finite_value;
}

inline bool operator!=(const Cardinal &left, const Cardinal &right) {
  return !(left == right);
}

inline bool operator<(const Cardinal &left, const Cardinal &right) {
  if (left.infinite) {
    return false;
  }

  if (right.infinite) {
    return true;
  }

  return left.finite_value < right.finite_value;
}

inline bool operator<=(const Cardinal &left, const Cardinal &right) {
  return left < right || left == right;
}

inline bool operator>(const Cardinal &left, const Cardinal &right) {
  return right < left;
}

inline bool operator>=(const Cardinal &left, const Cardinal &right) {
  return right <= left;
}

#endif // LABORATORYWORK4_CARDINAL_H
