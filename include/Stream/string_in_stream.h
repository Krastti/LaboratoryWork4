#ifndef LABORATORYWORK4_STRING_IN_STREAM_H
#define LABORATORYWORK4_STRING_IN_STREAM_H

#include <string>

#include "in_stream.h"
#include "../Sequence/array_sequence.h"

template <class T>
class StringInStream : public InStream<T> {
private:
  MutableArraySequence<std::string> lines;
  typename InStream<T>::Deserializer deserializer;

public:
  /**
   * Создаёт поток из строки, используя стандартный десериализатор для T.
   */
  StringInStream(const std::string &source);

  StringInStream(const std::string &source, typename InStream<T>::Deserializer deserializer);

  bool is_end_of_stream() const override;
  T read() override;
  bool is_can_seek() const override;
  std::size_t seek(std::size_t index) override;
  bool is_can_go_back() const override;

  ~StringInStream() override {}
};

#include "string_in_stream.tpp"

#endif // LABORATORYWORK4_STRING_IN_STREAM_H
