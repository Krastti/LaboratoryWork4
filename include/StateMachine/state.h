#ifndef LABORATORYWORK4_STATE_H
#define LABORATORYWORK4_STATE_H

#include <string>

struct State {
  int id;
  std::string name;

  State() : id(0), name() {}

  State(int id, const std::string &name) : id(id), name(name) {}

  bool operator==(const State &other) const {
    return id == other.id && name == other.name;
  }

  bool operator!=(const State &other) const {
    return !(*this == other);
  }
};

#endif // LABORATORYWORK4_STATE_H
