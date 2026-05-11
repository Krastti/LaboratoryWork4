#ifndef LABORATORYWORK4_TRANSITION_H
#define LABORATORYWORK4_TRANSITION_H

#include <functional>
#include <string>

#include "event.h"
#include "state.h"

using TransitionAction = std::function<void()>;

struct Transition {
  State from_state;
  Event event;
  State to_state;
  std::string action_name;
  TransitionAction action;

  /**
   * Создание пустого перехода
   */
  Transition() : from_state(), event(), to_state(), action_name(), action() {}

  /**
   * Создает переход из from_state в to_state по событию event
   */
  Transition(
      const State &from_state,
      const Event &event,
      const State &to_state,
      const std::string &action_name = "",
      TransitionAction action = TransitionAction())
      : from_state(from_state),
        event(event),
        to_state(to_state),
        action_name(action_name),
        action(action) {}

  /**
   * Проверяет, подходит ли переход для текущего состояния и входного события
   */
  bool matches(const State &state, const Event &incoming_event) const {
    return from_state.id == state.id && event.id == incoming_event.id;
  }

  /**
   * Выполняет действие перехода, если оно задано
   */
  void execute_action() const {
    if (action) {
      action();
    }
  }

  bool operator==(const Transition &other) const {
    return from_state == other.from_state &&
           event == other.event &&
           to_state == other.to_state &&
           action_name == other.action_name;
  }

  bool operator!=(const Transition &other) const {
    return !(*this == other);
  }
};

#endif // LABORATORYWORK4_TRANSITION_H
