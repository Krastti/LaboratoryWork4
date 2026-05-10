#ifndef LABORATORYWORK4_FILE_OUT_STREAM_H
#define LABORATORYWORK4_FILE_OUT_STREAM_H

#include <fstream>
#include <string>

#include "out_stream.h"

template <class T>
class FileOutStream : public OutStream<T> {
private:
  std::string file_path;
  typename OutStream<T>::Serializer serializer;
  std::ofstream file;

protected:
  void on_open() override;
  void on_close() override;

public:
  FileOutStream(const char* file_path, typename OutStream<T>::Serializer serializer);

  std::size_t write(const T &item) override;

  ~FileOutStream() override;
};

#include "file_out_stream.tpp"

#endif // LABORATORYWORK4_FILE_OUT_STREAM_H
