#include <cassert>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../include/LazySequence/cardinal.h"
#include "../include/LazySequence/circular_buffer.h"
#include "../include/LazySequence/lazy_sequence.h"
#include "../include/Sequence/array_sequence.h"
#include "../include/Stream/file_in_stream.h"
#include "../include/Stream/file_out_stream.h"
#include "../include/Stream/lazy_sequence_in_stream.h"
#include "../include/Stream/sequence_in_stream.h"
#include "../include/Stream/sequence_out_stream.h"
#include "../include/Stream/string_in_stream.h"
#include "../include/StateMachine/state_machine.h"

void assert_should_have_thrown() {
  assert(false && "should have thrown");
}

template <class TException, class TFunc>
void assert_throws(TFunc func) {
  try {
    func();
    assert_should_have_thrown();
  } catch (const TException&) {
  }
}

int fibonacci_rule(Sequence<int>* context) {
  return context->get(context->get_length() - 2) +
         context->get(context->get_length() - 1);
}

int multiply_by_two(const int &item) {
  return item * 2;
}

bool is_even(const int &item) {
  return item % 2 == 0;
}

int sum_int(const int &accumulator, const int &current) {
  return accumulator + current;
}

std::string serialize_int(const int &item) {
  return std::to_string(item);
}

int deserialize_int(const std::string &line) {
  return std::stoi(line);
}

std::string serialize_transition_for_test(const Transition &transition) {
  return transition.from_state.name + "->" + transition.to_state.name +
         ":" + transition.event.name;
}

void test_cardinal_and_circular_buffer_asserts() {
  Cardinal finite(5);
  Cardinal infinite = Cardinal::infinity();

  assert(finite.is_finite());
  assert(finite.value() == 5);
  assert(infinite.is_infinite());
  assert((finite + Cardinal(3)).value() == 8);
  assert((finite + infinite).is_infinite());
  assert(finite < infinite);
  assert(infinite == Cardinal::infinity());
  assert_throws<std::logic_error>([&]() { (void)infinite.value(); });

  CircularBuffer<int> buffer(3);
  assert(buffer.is_empty());
  assert(buffer.get_capacity() == 3);

  buffer.push_back(1);
  buffer.push_back(2);
  buffer.push_back(3);
  assert(buffer.is_full());
  assert(buffer.get_first() == 1);
  assert(buffer.get_last() == 3);

  buffer.push_back(4);
  assert(buffer.get_count() == 3);
  assert(buffer.get(0) == 2);
  assert(buffer.get(1) == 3);
  assert(buffer.get(2) == 4);

  Sequence<int>* sequence = buffer.to_sequence();
  assert(sequence->get_length() == 3);
  assert(sequence->get(0) == 2);
  assert(sequence->get(2) == 4);
  delete sequence;

  buffer.pop_front();
  assert(buffer.get_first() == 3);
  buffer.clear();
  assert(buffer.is_empty());

  assert_throws<std::invalid_argument>([]() { CircularBuffer<int> bad_buffer(0); });
  assert_throws<std::out_of_range>([&]() { buffer.get_first(); });
}

void test_lazy_sequence_asserts() {
  int items[] = {10, 20, 30};
  LazySequence<int> finite(items, 3);

  assert(finite.get_cardinal_length().value() == 3);
  assert(finite.get_materialized_count() == 3);
  assert(finite.get_first() == 10);
  assert(finite.get_last() == 30);
  assert(finite.get(1) == 20);
  assert(finite.try_get(3).is_none());

  LazySequence<int>* appended = finite.append(40);
  assert(appended->get_length() == 4);
  assert(appended->get_last() == 40);
  delete appended;

  LazySequence<int>* prepended = finite.prepend(5);
  assert(prepended->get_length() == 4);
  assert(prepended->get_first() == 5);
  delete prepended;

  LazySequence<int>* inserted = finite.insert_at(25, 2);
  assert(inserted->get_length() == 4);
  assert(inserted->get(2) == 25);
  assert(inserted->get(3) == 30);
  delete inserted;

  LazySequence<int>* removed = finite.remove_at(1);
  assert(removed->get_length() == 2);
  assert(removed->get(0) == 10);
  assert(removed->get(1) == 30);
  delete removed;

  LazySequence<int>* mapped = finite.map<int>(multiply_by_two);
  assert(mapped->get_length() == 3);
  assert(mapped->get(0) == 20);
  assert(mapped->get(2) == 60);
  delete mapped;

  LazySequence<int>* filtered = finite.where(is_even);
  assert(filtered->get_length() == 3);
  assert(filtered->get(0) == 10);
  assert(filtered->get(2) == 30);
  delete filtered;

  assert(finite.reduce<int>(sum_int, 0) == 60);

  int initial_items[] = {0, 1};
  MutableArraySequence<int> initial(initial_items, 2);
  int generator_calls = 0;
  auto fibonacci = [&generator_calls](Sequence<int>* context) {
    generator_calls++;
    return context->get(0) + context->get(1);
  };

  LazySequence<int> lazy_fibonacci(fibonacci, &initial, 2);
  assert(lazy_fibonacci.get_cardinal_length().is_infinite());
  assert(lazy_fibonacci.get(0) == 0);
  assert(lazy_fibonacci.get(1) == 1);
  assert(lazy_fibonacci.get(2) == 1);
  assert(lazy_fibonacci.get(3) == 2);
  assert(lazy_fibonacci.get(4) == 3);
  assert(lazy_fibonacci.get_materialized_count() == 5);
  int calls_after_materialization = generator_calls;
  assert(lazy_fibonacci.get(3) == 2);
  assert(generator_calls == calls_after_materialization);

  LazySequence<int>* lazy_inserted = lazy_fibonacci.insert_at(99, 2);
  assert(lazy_inserted->get(0) == 0);
  assert(lazy_inserted->get(1) == 1);
  assert(lazy_inserted->get(2) == 99);
  assert(lazy_inserted->get(3) == 1);
  delete lazy_inserted;

  LazySequence<int>* lazy_removed = lazy_fibonacci.remove_at(2);
  assert(lazy_removed->get(0) == 0);
  assert(lazy_removed->get(1) == 1);
  assert(lazy_removed->get(2) == 2);
  delete lazy_removed;

  LazySequence<int>* even_fibonacci = lazy_fibonacci.where(is_even);
  assert(even_fibonacci->get(0) == 0);
  assert(even_fibonacci->get(1) == 2);
  assert(even_fibonacci->get(2) == 8);
  delete even_fibonacci;

  assert_throws<std::out_of_range>([&]() { finite.get(-1); });
  assert_throws<std::out_of_range>([&]() { finite.get(3); });
  Sequence<int>* finite_as_sequence = &finite;
  assert_throws<std::logic_error>([&]() { finite_as_sequence->try_find(is_even); });
}

void test_stream_asserts() {
  int items[] = {1, 2, 3};
  MutableArraySequence<int> sequence(items, 3);

  SequenceInStream<int> input(&sequence);
  assert(!input.is_open());
  assert(input.is_can_seek());
  assert(input.is_can_go_back());
  assert_throws<std::logic_error>([&]() { input.read(); });

  input.open();
  assert(input.read() == 1);
  assert(input.get_position() == 1);
  assert(input.seek(2) == 2);
  assert(input.read() == 3);
  assert(input.is_end_of_stream());
  assert_throws<std::out_of_range>([&]() { input.read(); });
  input.close();

  MutableArraySequence<int> destination;
  SequenceOutStream<int> output(&destination);
  assert_throws<std::logic_error>([&]() { output.write(1); });
  output.open();
  assert(output.write(7) == 1);
  assert(output.write(8) == 2);
  assert(output.get_destination()->get_length() == 2);
  assert(output.get_destination()->get(0) == 7);
  assert(output.get_destination()->get(1) == 8);
  output.close();

  StringInStream<int> string_stream("10\n20\n30", deserialize_int);
  string_stream.open();
  assert(string_stream.read() == 10);
  assert(string_stream.seek(2) == 2);
  assert(string_stream.read() == 30);
  assert(string_stream.is_end_of_stream());
  string_stream.close();

  int initial_items[] = {1, 1};
  MutableArraySequence<int> initial(initial_items, 2);
  LazySequence<int> lazy(fibonacci_rule, &initial, 2);
  LazySequenceInStream<int> lazy_stream(&lazy);
  lazy_stream.open();
  assert(lazy_stream.read() == 1);
  assert(lazy_stream.read() == 1);
  assert(lazy_stream.read() == 2);
  assert(lazy_stream.seek(5) == 5);
  assert(lazy_stream.read() == 8);
  lazy_stream.close();

  const char* test_file = "laboratory_work4_stream_test.txt";
  std::remove(test_file);

  FileOutStream<int> file_output(test_file, serialize_int);
  file_output.open();
  assert(file_output.write(4) == 1);
  assert(file_output.write(5) == 2);
  file_output.close();

  FileInStream<int> file_input(test_file, deserialize_int);
  file_input.open();
  assert(file_input.read() == 4);
  assert(file_input.seek(0) == 0);
  assert(file_input.read() == 4);
  assert(file_input.read() == 5);
  assert(file_input.is_end_of_stream());
  file_input.close();
  std::remove(test_file);
}

void test_state_machine_asserts() {
  State locked(1, "Locked");
  State unlocked(2, "Unlocked");
  Event coin(1, "Coin");
  Event push(2, "Push");

  MutableArraySequence<Transition> history_sequence;
  SequenceOutStream<Transition> history(&history_sequence);
  StateMachine turnstile(&history);

  turnstile.add_state(locked);
  turnstile.add_state(unlocked);
  turnstile.set_initial_state(locked);

  int action_count = 0;
  turnstile.add_transition(Transition(
      locked,
      coin,
      unlocked,
      "unlock",
      [&action_count]() { action_count++; }));
  turnstile.add_transition(Transition(unlocked, push, locked, "lock"));
  turnstile.add_transition(Transition(locked, push, locked, "deny"));

  assert(turnstile.get_transition_count() == 3);
  assert(turnstile.get_current_state() == locked);
  assert(!turnstile.get_is_open());

  turnstile.open();
  assert(turnstile.get_is_open());
  turnstile.process_event(coin);
  assert(turnstile.get_current_state() == unlocked);
  assert(action_count == 1);
  assert(history.get_destination()->get_length() == 1);
  assert(history.get_destination()->get(0).action_name == "unlock");

  turnstile.process_event(push);
  assert(turnstile.get_current_state() == locked);
  assert(history.get_destination()->get_length() == 2);
  turnstile.close();
  assert(!turnstile.get_is_open());

  turnstile.reset();
  assert(turnstile.get_current_state() == locked);

  Event events[] = {coin, push, push};
  MutableArraySequence<Event> event_sequence(events, 3);
  SequenceInStream<Event> event_stream(&event_sequence);
  StateMachine runner;
  runner.add_state(locked);
  runner.add_state(unlocked);
  runner.set_initial_state(locked);
  runner.add_transition(Transition(locked, coin, unlocked, "unlock"));
  runner.add_transition(Transition(unlocked, push, locked, "lock"));
  runner.add_transition(Transition(locked, push, locked, "deny"));
  runner.run(event_stream);
  assert(runner.get_current_state() == locked);
  assert(event_stream.is_end_of_stream());

  StateMachine invalid;
  assert_throws<std::logic_error>([&]() { invalid.get_current_state(); });
  invalid.add_state(locked);
  assert_throws<std::invalid_argument>([&]() { invalid.add_state(locked); });
  assert_throws<std::invalid_argument>([&]() {
    invalid.add_transition(Transition(locked, coin, unlocked));
  });
}

void run_all_tests() {
  test_cardinal_and_circular_buffer_asserts();
  test_lazy_sequence_asserts();
  test_stream_asserts();
  test_state_machine_asserts();
}

void print_test_menu() {
  std::cout << "1. Cardinal and CircularBuffer Tests\n";
  std::cout << "2. LazySequence Tests\n";
  std::cout << "3. Stream Tests\n";
  std::cout << "4. StateMachine Tests\n";
  std::cout << "5. All Tests\n";
  std::cout << "0. Exit\n";
}

int read_choice() {
  int choice = 0;

  if (!(std::cin >> choice)) {
    std::cin.clear();
    std::cin.ignore(1000, '\n');
    return -1;
  }

  return choice;
}

int main() {
  print_test_menu();

  int choice = read_choice();

  switch (choice) {
    case 1:
      test_cardinal_and_circular_buffer_asserts();
      break;
    case 2:
      test_lazy_sequence_asserts();
      break;
    case 3:
      test_stream_asserts();
      break;
    case 4:
      test_state_machine_asserts();
      break;
    case 5:
      run_all_tests();
      break;
    case 0:
      return 0;
    default:
      std::cout << "Invalid choice\n";
      return 1;
  }

  std::cout << "Tests passed\n";
  return 0;
}
