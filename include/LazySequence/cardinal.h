#ifndef LABORATORYWORK4_CARDINAL_H
#define LABORATORYWORK4_CARDINAL_H

#include <cstddef>
#include <stdexcept>

class Cardinal {
private:
  // Для конечных чисел хранит само значение.
  // Для бесконечных индексов хранит конечное смещение после omega.
  std::size_t finite_value;
  bool infinite;

public:
  Cardinal() : finite_value(0), infinite(false) {}

  Cardinal(const std::size_t value) : finite_value(value), infinite(false) {}

  static Cardinal infinity() {
    return omega();
  }

  static Cardinal omega() {
    Cardinal value;
    value.infinite = true;
    value.finite_value = 0;
    return value;
  }

  static Cardinal omega_plus(const std::size_t offset) {
    Cardinal value;
    value.infinite = true;
    value.finite_value = offset;
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
      throw std::logic_error("Трансфинитный индекс не имеет конечного значения");
    }

    return finite_value;
  }

  std::size_t omega_offset() const {
    if (!infinite) {
      throw std::logic_error("Конечный индекс не имеет смещения после omega");
    }

    return finite_value;
  }

  Cardinal& operator+=(const Cardinal &other) {
    if (infinite && other.infinite) {
      finite_value = 0;
      return *this;
    }

    if (infinite) {
      finite_value += other.finite_value;
      return *this;
    }

    if (other.infinite) {
      infinite = true;
      finite_value = other.finite_value;
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
    return left.infinite == right.infinite && left.finite_value == right.finite_value;
  }

  return left.finite_value == right.finite_value;
}

inline bool operator!=(const Cardinal &left, const Cardinal &right) {
  return !(left == right);
}

inline bool operator<(const Cardinal &left, const Cardinal &right) {
  if (left.infinite && right.infinite) {
    return left.finite_value < right.finite_value;
  }

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
