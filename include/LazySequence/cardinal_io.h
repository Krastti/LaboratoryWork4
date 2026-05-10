#ifndef LABORATORYWORK4_CARDINAL_IO_H
#define LABORATORYWORK4_CARDINAL_IO_H

#include <ostream>

#include "cardinal.h"

inline std::ostream& operator<<(std::ostream &stream, const Cardinal &cardinal) {
  if (cardinal.is_infinite()) {
    return stream << "infinity";
  }

  return stream << cardinal.value();
}

#endif // LABORATORYWORK4_CARDINAL_IO_H
