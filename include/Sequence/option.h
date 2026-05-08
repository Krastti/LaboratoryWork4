#ifndef LABORATORYWORK2_OPTION_H
#define LABORATORYWORK2_OPTION_H

#include <stdexcept>

template <class T>
class Option {
private:
  T value;
  bool hasValue;

public:
  Option() : value(), hasValue(false) {}

  Option(const T &value) : value(value), hasValue(true) {}

  bool has_value() const {return hasValue;}
  bool is_some() const {return hasValue;}
  bool is_none() const {return !hasValue;}

  const T& get_value() const {
    if (!hasValue) {
      throw std::runtime_error("Option не содержит значения");
    }

    return value;
  }

  static Option<T> some(const T &value) {
    return Option<T>(value);
  }

  static Option<T> none() {
    return Option<T>();
  }
};

#endif // LABORATORYWORK2_OPTION_H
