#ifndef LABORATORYWORK4_FILE_IN_STREAM_TPP
#define LABORATORYWORK4_FILE_IN_STREAM_TPP

#include "file_in_stream.h"

#include <stdexcept>

template <class T>
FileInStream<T>::FileInStream(const char* file_path)
  : FileInStream(file_path, InStream<T>::default_deserialize) {}

template <class T>
void FileInStream<T>::on_open() {
  file.open(file_path);

  if (!file.is_open()) {
    throw std::runtime_error("Не удалось открыть файл для чтения");
  }

  this->position = 0;
}

template <class T>
void FileInStream<T>::on_close() {
  if (file.is_open()) {
    file.close();
  }
}

template <class T>
FileInStream<T>::FileInStream(
  const char* file_path,
  typename InStream<T>::Deserializer deserializer
) : deserializer(deserializer) {
  if (file_path == nullptr) {
    throw std::invalid_argument("Путь к файлу не может быть нулевым");
  } else {
    this->file_path = file_path;
  }

  if (!deserializer) {
    throw std::invalid_argument("Десериализатор не может быть пустым");
  }
}

template <class T>
bool FileInStream<T>::is_end_of_stream() const {
  if (!this->is_open()) {
    return true;
  }

  return file.peek() == std::ifstream::traits_type::eof();
}

template <class T>
T FileInStream<T>::read() {
  if (!this->is_open()) {
    throw std::logic_error("Поток не открыт");
  }

  std::string line;

  if (!std::getline(file, line)) {
    throw std::out_of_range("Достигнут конец потока");
  }

  T item = deserializer(line);
  this->position++;
  return item;
}

template <class T>
bool FileInStream<T>::is_can_seek() const {
  return true;
}

template <class T>
std::size_t FileInStream<T>::seek(std::size_t index) {
  if (!this->is_open()) {
    throw std::logic_error("Поток не открыт");
  }

  file.clear();
  file.seekg(0);
  this->position = 0;

  std::string line;

  while (this->position < index) {
    if (!std::getline(file, line)) {
      throw std::out_of_range("Позиция вне допустимого диапазона");
    }

    this->position++;
  }

  return this->position;
}

template <class T>
bool FileInStream<T>::is_can_go_back() const {
  return true;
}

template <class T>
FileInStream<T>::~FileInStream() {
  this->close();
}

#endif // LABORATORYWORK4_FILE_IN_STREAM_TPP
