#ifndef LABORATORYWORK4_EVENT_H
#define LABORATORYWORK4_EVENT_H

#include <string>

struct Event {
  int id;
  std::string name;

  Event() : id(0), name() {}

  Event(int id, const std::string &name) : id(id), name(name) {}

  bool operator==(const Event &other) const {
    return id == other.id && name == other.name;
  }

  bool operator!=(const Event &other) const {
    return !(*this == other);
  }
};

#endif // LABORATORYWORK4_EVENT_H
