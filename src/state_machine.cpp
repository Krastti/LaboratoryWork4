#include "../include/StateMachine/state_machine.h"

#include <stdexcept>

#include "../include/Sequence/array_sequence.h"

Pair<int, int> StateMachine::make_transition_key(int state_id, int event_id) const {
  return Pair<int, int>(state_id, event_id);
}

bool StateMachine::transition_keys_equal(const Pair<int, int> &left, const Pair<int, int> &right) const {
  return left.first() == right.first() && left.second() == right.second();
}

const Transition* StateMachine::find_transition(const State &state, const Event &event) const {
  Pair<int, int> key = make_transition_key(state.id, event.id);

  for (int index = 0; index < transitions->get_length(); index++) {
    const Pair<Pair<int, int>, Transition> &entry = transitions->get(index);

    if (transition_keys_equal(entry.first(), key)) {
      return &entry.second();
    }
  }

  return nullptr;
}

bool StateMachine::has_state(int state_id) const {
  for (int i = 0; i < states->get_length(); i++) {
    if (states->get(i).id == state_id) {
      return true;
    }
  }

  return false;
}

StateMachine::StateMachine()
    : states(new MutableArraySequence<State>()),
      transitions(new MutableArraySequence<Pair<Pair<int, int>, Transition>>()),
      current_state(),
      initial_state(),
      history(nullptr),
      has_initial_state(false),
      is_open(false) {}

StateMachine::StateMachine(OutStream<Transition>* history)
    : StateMachine() {
  this->history = history;
}

StateMachine::StateMachine(const StateMachine &other)
    : states(new MutableArraySequence<State>()),
      transitions(new MutableArraySequence<Pair<Pair<int, int>, Transition>>()),
      current_state(other.current_state),
      initial_state(other.initial_state),
      history(other.history),
      has_initial_state(other.has_initial_state),
      is_open(false) {
  for (int i = 0; i < other.states->get_length(); i++) {
    states = states->append(other.states->get(i));
  }

  for (int i = 0; i < other.transitions->get_length(); i++) {
    transitions = transitions->append(other.transitions->get(i));
  }
}

StateMachine& StateMachine::operator=(const StateMachine &other) {
  if (this == &other) {
    return *this;
  }

  Sequence<State>* copied_states = new MutableArraySequence<State>();
  Sequence<Pair<Pair<int, int>, Transition>>* copied_transitions =
      new MutableArraySequence<Pair<Pair<int, int>, Transition>>();

  for (int i = 0; i < other.states->get_length(); i++) {
    copied_states = copied_states->append(other.states->get(i));
  }

  for (int i = 0; i < other.transitions->get_length(); i++) {
    copied_transitions = copied_transitions->append(other.transitions->get(i));
  }

  delete states;
  delete transitions;
  states = copied_states;
  transitions = copied_transitions;
  current_state = other.current_state;
  initial_state = other.initial_state;
  history = other.history;
  has_initial_state = other.has_initial_state;
  is_open = false;

  return *this;
}

void StateMachine::set_history_stream(OutStream<Transition>* history) {
  this->history = history;
}

void StateMachine::add_state(const State &state) {
  if (has_state(state.id)) {
    throw std::invalid_argument("Состояние с таким id уже зарегистрировано");
  }

  states = states->append(state);
}

void StateMachine::add_transition(const Transition &transition) {
  if (!has_state(transition.from_state.id)) {
    throw std::invalid_argument("Начальное состояние перехода не зарегистрировано");
  }

  if (!has_state(transition.to_state.id)) {
    throw std::invalid_argument("Конечное состояние перехода не зарегистрировано");
  }

  Pair<int, int> key = make_transition_key(transition.from_state.id, transition.event.id);
  MutableArraySequence<Pair<Pair<int, int>, Transition>>* updated_transitions =
      new MutableArraySequence<Pair<Pair<int, int>, Transition>>();
  bool replaced = false;

  for (int index = 0; index < transitions->get_length(); index++) {
    const Pair<Pair<int, int>, Transition> &entry = transitions->get(index);

    if (transition_keys_equal(entry.first(), key)) {
      updated_transitions->append(Pair<Pair<int, int>, Transition>(key, transition));
      replaced = true;
    } else {
      updated_transitions->append(entry);
    }
  }

  if (!replaced) {
    updated_transitions->append(Pair<Pair<int, int>, Transition>(key, transition));
  }

  delete transitions;
  transitions = updated_transitions;
}

void StateMachine::set_initial_state(const State &state) {
  if (!has_state(state.id)) {
    throw std::invalid_argument("Начальное состояние не зарегистрировано");
  }

  initial_state = state;
  current_state = state;
  has_initial_state = true;
}

void StateMachine::open() {
  if (is_open) {
    return;
  }

  if (history != nullptr) {
    history->open();
  }

  is_open = true;
}

void StateMachine::close() {
  if (!is_open) {
    return;
  }

  if (history != nullptr) {
    history->close();
  }

  is_open = false;
}

void StateMachine::process_event(const Event &event) {
  if (!has_initial_state) {
    throw std::logic_error("Начальное состояние не задано");
  }

  const Transition* transition = find_transition(current_state, event);

  if (transition == nullptr) {
    throw std::out_of_range("Для текущего состояния и события нет перехода");
  }

  transition->execute_action();
  current_state = transition->to_state;

  if (history != nullptr) {
    history->write(*transition);
  }
}

void StateMachine::run(InStream<Event> &stream) {
  if (!stream.is_open()) {
    stream.open();
  }

  while (!stream.is_end_of_stream()) {
    process_event(stream.read());
  }
}

void StateMachine::reset() {
  if (!has_initial_state) {
    throw std::logic_error("Начальное состояние не задано");
  }

  current_state = initial_state;
}

const State& StateMachine::get_current_state() const {
  if (!has_initial_state) {
    throw std::logic_error("Начальное состояние не задано");
  }

  return current_state;
}

const Sequence<State>* StateMachine::get_states() const {
  return states;
}

int StateMachine::get_transition_count() const {
  return transitions->get_length();
}

bool StateMachine::get_is_open() const {
  return is_open;
}

StateMachine::~StateMachine() {
  if (is_open && history != nullptr) {
    history->close();
  }

  delete states;
  delete transitions;
}
