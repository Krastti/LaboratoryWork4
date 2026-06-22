#include "configurator.h"

#include <sstream>
#include <stdexcept>
#include <string>

#include "../include/Stream/file_in_stream.h"
#include "../include/Stream/file_out_stream.h"

namespace ConfiguratorFileFormat {
std::string write_line(const std::string &line) {
  return line;
}

std::string read_required(FileInStream<std::string> &input, const std::string &context) {
  if (input.is_end_of_stream()) {
    throw std::invalid_argument("Файл конфигурации закончился раньше: " + context);
  }

  return input.read();
}

int read_count(FileInStream<std::string> &input, const std::string &expected_word) {
  std::istringstream line(read_required(input, expected_word));
  std::string word;
  int count = 0;

  if (!(line >> word >> count) || word != expected_word) {
    throw std::invalid_argument("Некорректная строка количества: " + expected_word);
  }

  return count;
}

State read_state(FileInStream<std::string> &input, const std::string &expected_word) {
  std::istringstream line(read_required(input, expected_word));
  std::string word;
  int id = 0;
  std::string name;

  if (!(line >> word >> id >> name) || word != expected_word) {
    throw std::invalid_argument("Некорректная строка состояния: " + expected_word);
  }

  return State(id, name);
}

Transition read_transition(FileInStream<std::string> &input) {
  std::istringstream line(read_required(input, "transition"));
  std::string word;
  int from_id = 0;
  int event_id = 0;
  int to_id = 0;
  std::string from_name;
  std::string event_name;
  std::string to_name;
  std::string action_name;

  if (!(line >> word >> from_id >> from_name >> event_id >> event_name >> to_id >> to_name) ||
      word != "transition") {
    throw std::invalid_argument("Некорректная строка перехода");
  }

  line >> action_name;

  return Transition(
      State(from_id, from_name),
      Event(event_id, event_name),
      State(to_id, to_name),
      action_name);
}
}

void StateMachineConfigurator::export_to_file(const StateMachine &machine, const char* file_path) {
  FileOutStream<std::string> output(file_path, ConfiguratorFileFormat::write_line);
  output.open();

  const Sequence<State>* states = machine.get_states();
  const Sequence<Pair<Pair<int, int>, Transition>>* transitions = machine.get_transitions();

  output.write("states " + std::to_string(states->get_length()));

  for (int i = 0; i < states->get_length(); i++) {
    const State &state = states->get(i);
    output.write("state " + std::to_string(state.id) + " " + state.name);
  }

  const State &initial_state = machine.get_initial_state();
  output.write("initial " + std::to_string(initial_state.id) + " " + initial_state.name);
  output.write("transitions " + std::to_string(transitions->get_length()));

  for (int i = 0; i < transitions->get_length(); i++) {
    const Transition &transition = transitions->get(i).second();
    output.write(
        "transition " +
        std::to_string(transition.from_state.id) + " " + transition.from_state.name + " " +
        std::to_string(transition.event.id) + " " + transition.event.name + " " +
        std::to_string(transition.to_state.id) + " " + transition.to_state.name + " " +
        transition.action_name);
  }

  output.close();
}

StateMachine StateMachineConfigurator::import_from_file(const char* file_path) {
  FileInStream<std::string> input(file_path);
  input.open();

  StateMachine machine;
  int state_count = ConfiguratorFileFormat::read_count(input, "states");

  for (int i = 0; i < state_count; i++) {
    machine.add_state(ConfiguratorFileFormat::read_state(input, "state"));
  }

  machine.set_initial_state(ConfiguratorFileFormat::read_state(input, "initial"));
  int transition_count = ConfiguratorFileFormat::read_count(input, "transitions");

  for (int i = 0; i < transition_count; i++) {
    machine.add_transition(ConfiguratorFileFormat::read_transition(input));
  }

  input.close();
  return machine;
}
