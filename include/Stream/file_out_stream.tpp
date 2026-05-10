#ifndef LABORATORYWORK4_FILE_OUT_STREAM_TPP
#define LABORATORYWORK4_FILE_OUT_STREAM_TPP

#include "file_out_stream.h"

#include <stdexcept>

template <class T>
void FileOutStream<T>::on_open() {
  file.open(file_path);

  if (!file.is_open()) {
    throw std::runtime_error("Не удалось открыть файл для записи");
  }

  this->position = 0;
}

template <class T>
void FileOutStream<T>::on_close() {
  if (file.is_open()) {
    file.close();
  }
}

template <class T>
FileOutStream<T>::FileOutStream(
  const char* file_path,
  typename OutStream<T>::Serializer serializer
) : serializer(serializer) {
  if (file_path == nullptr) {
    throw std::invalid_argument("Путь к файлу не может быть нулевым");
  } else {
    this->file_path = file_path;
  }

  if (!serializer) {
    throw std::invalid_argument("Сериализатор не может быть пустым");
  }
}

template <class T>
std::size_t FileOutStream<T>::write(const T &item) {
  if (!this->is_open()) {
    throw std::logic_error("Поток не открыт");
  }

  file << serializer(item) << '\n';

  if (!file) {
    throw std::runtime_error("Не удалось записать элемент в файл");
  }

  this->position++;
  return this->position;
}

template <class T>
FileOutStream<T>::~FileOutStream() {
  this->close();
}

#endif // LABORATORYWORK4_FILE_OUT_STREAM_TPP
