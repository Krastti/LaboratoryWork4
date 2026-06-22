#ifndef LABORATORYWORK4_STATE_MACHINE_H
#define LABORATORYWORK4_STATE_MACHINE_H

#include "../Sequence/sequence.h"
#include "../Stream/in_stream.h"
#include "../Stream/out_stream.h"
#include "event.h"
#include "state.h"
#include "transition.h"

class StateMachine {
private:
  Sequence<State>* states; // Все зарегистрированные состояния машины
  Sequence<Pair<Pair<int, int>, Transition>>* transitions; // Таблица переходов: (state_id, event_id) -> Transition
  State current_state;
  State initial_state;
  OutStream<Transition>* history;
  bool has_initial_state;
  bool is_open;

  /**
   * Создает ключ таблицы переходов из id состояния и события
   */
  Pair<int, int> make_transition_key(int state_id, int event_id) const;

  /**
   * Сравнивает ключи таблицы переходов
   */
  bool transition_keys_equal(const Pair<int, int> &left, const Pair<int, int> &right) const;

  /**
   * Ищет переход для пары текущее состояние + входное событие
   */
  const Transition* find_transition(const State &state, const Event &event) const;

  /**
   * Проверяет, зарегистрировано ли состояние с указанным id
   */
  bool has_state(int state_id) const;

public:
  /**
   * Создает пустую машину состояний без потока истории
   */
  StateMachine();

  /**
   * Создает машину состояний с потоком для записи истории переходов
   */
  explicit StateMachine(OutStream<Transition>* history);

  /**
   * Создает копию машины состояний без открытия потока истории
   */
  StateMachine(const StateMachine &other);

  /**
   * Копирует состояния, переходы и текущую позицию автомата
   */
  StateMachine& operator=(const StateMachine &other);

  /**
   * Задает поток, в который будут записываться выполненные переходы
   */
  void set_history_stream(OutStream<Transition>* history);

  /**
   * Регистрирует новое состояние автомата
   */
  void add_state(const State &state);

  /**
   * Добавляет переход между уже зарегистрированными состояниями
   */
  void add_transition(const Transition &transition);

  /**
   * Задает начальное состояние и делает его текущим
   */
  void set_initial_state(const State &state);

  /**
   * Открывает машину и поток истории, если он задан
   */
  void open();

  /**
   * Закрывает машину и поток истории, если он задан
   */
  void close();

  /**
   * Обрабатывает одно событие: находит переход, выполняет действие и меняет состояние
   */
  void process_event(const Event &event);

  /**
   * Последовательно читает события из потока и передает их в process_event
   */
  void run(InStream<Event> &stream);

  /**
   * Возвращает автомат в начальное состояние
   */
  void reset();

  /**
   * Возвращает текущее состояние автомата
   */
  const State& get_current_state() const;

  /**
   * Возвращает зарегистрированные состояния
   */
  const Sequence<State>* get_states() const;

  /**
   * Возвращает количество зарегистрированных переходов
   */
  int get_transition_count() const;

  /**
   * Возвращает зарегистрированные переходы автомата
   */
  const Sequence<Pair<Pair<int, int>, Transition>>* get_transitions() const;

  /**
   * Возвращает начальное состояние автомата
   */
  const State& get_initial_state() const;

  /**
   * Возвращает признак открытия машины
   */
  bool get_is_open() const;

  /**
   * Закрывает поток истории при необходимости и освобождает состояния
   */
  ~StateMachine();
};

#endif // LABORATORYWORK4_STATE_MACHINE_H
