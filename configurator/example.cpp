#include "configurator.h"

#include <iostream>

int main() {
  const char* config_path = "turnstile_machine.cfg";

  State locked(1, "Locked");
  State unlocked(2, "Unlocked");
  Event coin(1, "Coin");
  Event push(2, "Push");

  StateMachine original;
  original.add_state(locked);
  original.add_state(unlocked);
  original.set_initial_state(locked);
  original.add_transition(Transition(locked, coin, unlocked, "unlock"));
  original.add_transition(Transition(unlocked, push, locked, "lock"));
  original.add_transition(Transition(locked, push, locked, "deny"));

  StateMachineConfigurator::export_to_file(original, config_path);
  StateMachine restored = StateMachineConfigurator::import_from_file(config_path);

  restored.open();
  restored.process_event(coin);
  std::cout << "Current state: " << restored.get_current_state().name << '\n';
  restored.process_event(push);
  std::cout << "Current state: " << restored.get_current_state().name << '\n';
  restored.close();

  return 0;
}
