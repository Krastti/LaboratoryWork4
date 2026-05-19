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

void set_console_color(WORD color) {
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void reset_console_color() {
  set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void clear_console() {
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO info;
  DWORD written = 0;
  DWORD cells = 0;
  COORD home = {0, 0};

  if (!GetConsoleScreenBufferInfo(console, &info)) {
    return;
  }

  cells = info.dwSize.X * info.dwSize.Y;
  FillConsoleOutputCharacter(console, ' ', cells, home, &written);
  FillConsoleOutputAttribute(console, info.wAttributes, cells, home, &written);
  SetConsoleCursorPosition(console, home);
}

void print_lamp(const std::string &title, bool is_on, WORD color) {
  std::cout << title << " ";

  if (is_on) {
    set_console_color(color | FOREGROUND_INTENSITY);
    std::cout << "● ГОРИТ";
  } else {
    set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    std::cout << "○ не горит";
  }

  reset_console_color();
  std::cout << "\n";
}

void show_turnstile_interface(
    int step,
    const Event &event,
    const State &state,
    const std::string &action_message) {

  std::cout << "Демонстрация машины состояний: турникет\n";
  std::cout << "Шаг: " << step << "\n";
  std::cout << "Событие: " << event.name << "\n";
  std::cout << "Действие: " << action_message << "\n\n";

  print_lamp("Лампочка \"Закрыт\":", state.name == "Закрыт",
      FOREGROUND_RED);
  print_lamp("Лампочка \"Открыт\": ", state.name == "Открыт",
      FOREGROUND_GREEN);

  std::cout << "\nТекущее состояние: " << state.name << "\n";
}

int main() {
  SetConsoleCP(CP_UTF8);
  SetConsoleOutputCP(CP_UTF8);

  State locked(1, "Закрыт");
  State unlocked(2, "Открыт");
  Event coin(1, "Монета");
  Event push(2, "Толкнуть");
  std::string action_message = "Ожидание первого события.";

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
      [&action_message]() {
        action_message = "Монета принята. Турникет открыт.";
      }));

  turnstile.add_transition(Transition(
      unlocked,
      push,
      locked,
      "Пропустить человека и заблокировать турникет",
      [&action_message]() {
        action_message = "Проход выполнен. Турникет снова закрыт.";
      }));

  turnstile.add_transition(Transition(
      locked,
      push,
      locked,
      "Отказать в проходе",
      [&action_message]() {
        action_message = "Проход запрещен. Сначала нужна монета.";
      }));

  turnstile.add_transition(Transition(
      unlocked,
      coin,
      unlocked,
      "Оставить турникет открытым",
      [&action_message]() {
        action_message = "Турникет уже открыт. Монета не меняет состояние.";
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

  turnstile.open();
  event_stream.open();

  show_turnstile_interface(0, Event(0, "Нет"), turnstile.get_current_state(), action_message);
  Sleep(200);

  for (int i = 0; i < 20; i++) {
    Event event = event_stream.read();
    turnstile.process_event(event);
    show_turnstile_interface(i, event, turnstile.get_current_state(), action_message);
    Sleep(1000);
  }

  event_stream.close();
  turnstile.close();
  std::cout << "История переходов записана в test.txt\n";

  return 0;
}
