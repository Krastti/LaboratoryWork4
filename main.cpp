#include <windows.h>

#include <iostream>
#include <random>
#include <string>

#include "include/LazySequence/lazy_sequence.h"
#include "include/Sequence/array_sequence.h"
#include "include/Stream/file_out_stream.h"
#include "include/Stream/lazy_sequence_in_stream.h"
#include "include/StateMachine/state_machine.h"

std::string serialize_transition(const Transition &transition) {
  return transition.from_state.name + " -- " +
         transition.event.name + " / " +
         transition.action_name + " -> " +
         transition.to_state.name;
}

int main() {
  SetConsoleCP(CP_UTF8);
  SetConsoleOutputCP(CP_UTF8);

  State locked(1, "Закрыт");
  State unlocked(2, "Открыт");
  Event coin(1, "Монета");
  Event push(2, "Толкнуть");

  FileOutStream<Transition> history("test.txt", serialize_transition);

  StateMachine turnstile(&history);
  turnstile.add_state(locked);
  turnstile.add_state(unlocked);
  turnstile.set_initial_state(locked);

  turnstile.add_transition(Transition(
      locked,
      coin,
      unlocked,
      "Разблокировать турникет",
      []() {
        std::cout << "Монета принята. Турникет открыт.\n";
      }));

  turnstile.add_transition(Transition(
      unlocked,
      push,
      locked,
      "Пропустить человека и заблокировать турникет",
      []() {
        std::cout << "Проход выполнен. Турникет снова закрыт.\n";
      }));

  turnstile.add_transition(Transition(
      locked,
      push,
      locked,
      "Отказать в проходе",
      []() {
        std::cout << "Проход запрещен. Сначала нужна монета.\n";
      }));

  turnstile.add_transition(Transition(
      unlocked,
      coin,
      unlocked,
      "Оставить турникет открытым",
      []() {
        std::cout << "Турникет уже открыт. Монета не меняет состояние.\n";
      }));

  Event initial_events[] = {push, coin};
  MutableArraySequence<Event> initial_sequence(initial_events, 2);

  std::random_device random_device;
  std::mt19937 generator(random_device());
  std::uniform_int_distribution<int> percent(1, 100);

  auto generate_event = [push, coin, &generator, &percent](Sequence<Event>* context) {
    (void) context;
    return percent(generator) <= 60 ? push : coin;
  };

  LazySequence<Event> generated_events(generate_event, &initial_sequence, 2);
  LazySequenceInStream<Event> event_stream(&generated_events);

  std::cout << "Начальное состояние: " << turnstile.get_current_state().name << "\n\n";

  turnstile.open();
  event_stream.open();

  for (int i = 0; i < 5000; i++) {
    Event event = event_stream.read();
    std::cout << "Событие: " << event.name << "\n";
    turnstile.process_event(event);
    std::cout << "Текущее состояние: " << turnstile.get_current_state().name << "\n\n";
  }

  event_stream.close();
  turnstile.close();
  std::cout << "История переходов записана в test.txt\n";

  return 0;
}
