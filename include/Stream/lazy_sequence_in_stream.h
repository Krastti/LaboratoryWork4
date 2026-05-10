#ifndef LABORATORYWORK4_LAZY_SEQUENCE_IN_STREAM_H
#define LABORATORYWORK4_LAZY_SEQUENCE_IN_STREAM_H

#include "in_stream.h"

template <class T>
class LazySequenceInStream : public InStream<T> {
private:
  LazySequence<T>* source;

  /**
   * Проверяет, что позицию потока можно безопасно передать в LazySequence::get(int).
   */
  void check_position_supported(std::size_t index) const;

public:
  LazySequenceInStream(LazySequence<T>* source);

  bool is_end_of_stream() const override;
  T read() override;
  bool is_can_seek() const override;
  std::size_t seek(std::size_t index) override;
  bool is_can_go_back() const override;

  ~LazySequenceInStream() override {}
};

#include "lazy_sequence_in_stream.tpp"

#endif // LABORATORYWORK4_LAZY_SEQUENCE_IN_STREAM_H
