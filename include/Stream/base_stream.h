#ifndef LABORATORYWORK4_BASE_STREAM_H
#define LABORATORYWORK4_BASE_STREAM_H

#include <cstddef>

template <class T>
class BaseStream {
protected:
  std::size_t position = 0;
  bool opened = false;

  virtual void on_open() {}
  virtual void on_close() {}

public:
  std::size_t get_position() const {
    return position;
  }

  bool is_open() const {
    return opened;
  }

  void open() {
    if (opened) {
      return;
    }

    on_open();
    opened = true;
  }

  void close() {
    if (!opened) {
      return;
    }

    on_close();
    opened = false;
  }

  virtual ~BaseStream() = default;
};

#endif // LABORATORYWORK4_BASE_STREAM_H
