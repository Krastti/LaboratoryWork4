#ifndef LABORATORYWORK4_FILE_IN_STREAM_H
#define LABORATORYWORK4_FILE_IN_STREAM_H

#include <fstream>
#include <string>

#include "in_stream.h"

template <class T>
class FileInStream : public InStream<T> {
private:
  std::string file_path;
  typename InStream<T>::Deserializer deserializer; 
  mutable std::ifstream file; // Нужен для вызова peek внутри const-метода is_end_of_stream.

protected:
  void on_open() override;
  void on_close() override;

public:
  FileInStream(const char* file_path, typename InStream<T>::Deserializer deserializer);

  bool is_end_of_stream() const override;
  T read() override;
  bool is_can_seek() const override;
  std::size_t seek(std::size_t index) override;
  bool is_can_go_back() const override;

  ~FileInStream() override;
};

#include "file_in_stream.tpp"

#endif // LABORATORYWORK4_FILE_IN_STREAM_H
