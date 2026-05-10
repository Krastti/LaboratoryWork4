#ifndef LABORATORYWORK4_SEQUENCE_IN_STREAM_H
#define LABORATORYWORK4_SEQUENCE_IN_STREAM_H

#include "in_stream.h"

template <class T>
class SequenceInStream : public InStream<T> {
private:
  Sequence<T>* source;

public:
  SequenceInStream(Sequence<T>* source);

  bool is_end_of_stream() const override;
  T read() override;
  bool is_can_seek() const override;
  std::size_t seek(std::size_t index) override;
  bool is_can_go_back() const override;

  ~SequenceInStream() override {}
};

#include "sequence_in_stream.tpp"

#endif // LABORATORYWORK4_SEQUENCE_IN_STREAM_H
