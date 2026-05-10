#ifndef LABORATORYWORK4_SEQUENCE_OUT_STREAM_H
#define LABORATORYWORK4_SEQUENCE_OUT_STREAM_H

#include "out_stream.h"

template <class T>
class SequenceOutStream : public OutStream<T> {
private:
  Sequence<T>* destination;

public:
  SequenceOutStream(Sequence<T>* destination);

  /**
   * Возвращает актуальную последовательность, в которую выполняется запись
   */
  Sequence<T>* get_destination() const;
  std::size_t write(const T &item) override;

  ~SequenceOutStream() override {}
};

#include "sequence_out_stream.tpp"

#endif // LABORATORYWORK4_SEQUENCE_OUT_STREAM_H
