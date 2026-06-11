#ifndef LABORATORYWORK4_LAZY_SEQUENCE_TPP
#define LABORATORYWORK4_LAZY_SEQUENCE_TPP

#include "lazy_sequence.h"

#include <limits>
#include <stdexcept>

template <class T>
Sequence<T>* LazySequence<T>::make_from_array(const T* items, int count) {
  if (count < 0) {
    throw std::out_of_range("Количество элементов не может быть отрицательным");
  }

  if (items == nullptr && count > 0) {
    throw std::invalid_argument("Нельзя создать LazySequence из нулевого массива");
  }

  return new MutableArraySequence<T>(items, count);
}

template <class T>
Sequence<T>* LazySequence<T>::copy_sequence(const Sequence<T>* seq) {
  if (seq == nullptr) {
    throw std::invalid_argument("Нельзя создать LazySequence из нулевой последовательности");
  }

  MutableArraySequence<T>* copy = new MutableArraySequence<T>();

  for (int index = 0; index < seq->get_length(); index++) {
    copy->append(seq->get(index));
  }

  return copy;
}

template <class T>
void LazySequence<T>::materialize_until(int index) const {
  if (index < 0) {
    throw std::out_of_range("Индекс вне допустимого диапазона");
  }

  if (length.is_finite() && static_cast<std::size_t>(index) >= length.value()) {
    throw std::out_of_range("Индекс вне допустимого диапазона");
  }

  while (materialized->get_length() <= index) {
    if (!try_materialize_next()) {
      throw std::out_of_range("Индекс вне допустимого диапазона");
    }
  }
}

template <class T>
bool LazySequence<T>::try_materialize_next() const {
  if (generator == nullptr) {
    return false;
  }

  Option<T> next = generator->try_get_next();

  if (next.is_none()) {
    return false;
  }

  materialized->append(next.get_value());

  return true;
}

template <class T>
void LazySequence<T>::sys_append(const T &item) {
  if (length.is_infinite()) {
    throw std::logic_error("Нельзя выполнить внутреннее добавление к бесконечной LazySequence");
  }

  Sequence<T>* updated = materialized->append(item);

  if (updated != materialized) {
    delete materialized;
    materialized = updated;
  }

  length = Cardinal(length.value() + 1);
}

template <class T>
Sequence<T>* LazySequence<T>::new_empty_instance() const {
  return new LazySequence<T>();
}

template <class T>
LazySequence<T>::LazySequence()
  : materialized(new MutableArraySequence<T>()),
    length(0),
    generator(nullptr),
    transfinite_tail(nullptr) {}

template <class T>
LazySequence<T>::LazySequence(Sequence<T>* materialized, Cardinal length, BaseGenerator<T>* generator)
  : materialized(materialized),
    length(length),
    generator(generator),
    transfinite_tail(nullptr) {
  if (materialized == nullptr) {
    throw std::invalid_argument("Материализованная часть не может быть нулевой");
  }
}

template <class T>
LazySequence<T>::LazySequence(const T* items, int count)
  : materialized(make_from_array(items, count)),
    length(static_cast<std::size_t>(count)),
    generator(nullptr),
    transfinite_tail(nullptr) {}

template <class T>
LazySequence<T>::LazySequence(Sequence<T>* seq)
  : materialized(copy_sequence(seq)),
    length(0),
    generator(nullptr),
    transfinite_tail(nullptr) {
  length = Cardinal(static_cast<std::size_t>(materialized->get_length()));
}

template <class T>
LazySequence<T>::LazySequence(T (*recurrence)(Sequence<T>*), const Sequence<T> &initial)
  : materialized(nullptr),
    length(Cardinal::infinity()),
    generator(nullptr),
    transfinite_tail(nullptr) {
  if (recurrence == nullptr) {
    throw std::invalid_argument("Порождающее правило не может быть нулевым");
  }

  materialized = copy_sequence(&initial);

  try {
    generator = new RuleGenerator<T>(recurrence, *materialized);
  } catch (...) {
    delete materialized;
    materialized = nullptr;
    throw;
  }
}

template <class T>
LazySequence<T>::LazySequence(std::function<T(Sequence<T>*)> recurrence, Sequence<T>* initial, std::size_t context_size)
  : materialized(nullptr),
    length(Cardinal::infinity()),
    generator(nullptr),
    transfinite_tail(nullptr) {
  if (!recurrence) {
    throw std::invalid_argument("Порождающее правило не может быть пустым");
  }

  materialized = copy_sequence(initial);

  try {
    generator = new RuleGenerator<T>(recurrence, *materialized, context_size);
  } catch (...) {
    delete materialized;
    materialized = nullptr;
    throw;
  }
}

template <class T>
LazySequence<T>::LazySequence(const LazySequence<T> &other)
  : materialized(copy_sequence(other.materialized)),
    length(other.length),
    generator(nullptr),
    transfinite_tail(nullptr) {
  if (other.generator != nullptr) {
    generator = other.generator->clone();
  }

  if (other.transfinite_tail != nullptr) {
    transfinite_tail = new LazySequence<T>(*other.transfinite_tail);
  }
}

template <class T>
const T& LazySequence<T>::get_first() const {
  return get(0);
}

template <class T>
const T& LazySequence<T>::get_last() const {
  if (length.is_infinite()) {
    throw std::logic_error("У бесконечной последовательности нет последнего элемента");
  }

  if (length.value() == 0) {
    throw std::out_of_range("Индекс вне допустимого диапазона");
  }

  return get(static_cast<int>(length.value() - 1));
}

template <class T>
const T& LazySequence<T>::get(int index) const {
  materialize_until(index);
  return materialized->get(index);
}

template <class T>
const T& LazySequence<T>::get(const Cardinal &index) const {
  if (index.is_finite()) {
    if (index.value() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
      throw std::overflow_error("Индекс LazySequence не помещается в int");
    }

    return get(static_cast<int>(index.value()));
  }

  if (transfinite_tail == nullptr) {
    throw std::out_of_range("Трансфинитный индекс вне допустимого диапазона");
  }

  if (index.omega_offset() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
    throw std::overflow_error("Смещение после omega не помещается в int");
  }

  return transfinite_tail->get(static_cast<int>(index.omega_offset()));
}

template <class T>
const T& LazySequence<T>::operator[](int index) const {
  return get(index);
}

template <class T>
Option<T> LazySequence<T>::try_get_first() const {
  return try_get(0);
}

template <class T>
Option<T> LazySequence<T>::try_get_last() const {
  if (length.is_infinite()) {
    return Option<T>::none();
  }

  if (length.value() == 0) {
    return Option<T>::none();
  }

  return try_get(static_cast<int>(length.value() - 1));
}

template <class T>
Option<T> LazySequence<T>::try_get(int index) const {
  try {
    return Option<T>::some(get(index));
  } catch (const std::exception&) {
    return Option<T>::none();
  }
}

template <class T>
Option<T> LazySequence<T>::try_get(const Cardinal &index) const {
  try {
    return Option<T>::some(get(index));
  } catch (const std::exception&) {
    return Option<T>::none();
  }
}

template <class T>
Option<T> LazySequence<T>::try_find(bool (*predicate)(const T &element)) const {
  if (predicate == nullptr) {
    throw std::invalid_argument("Нельзя выполнить try_find с нулевым предикатом");
  }

  throw std::logic_error("LazySequence не поддерживает поиск через Sequence::try_find");
}

template <class T>
Sequence<T>* LazySequence<T>::get_sub_sequence(int startIndex, int endIndex) const {
  if (startIndex < 0 || endIndex < 0 || startIndex > endIndex) {
    throw std::out_of_range("Индекс вне допустимого диапазона");
  }

  materialize_until(endIndex);

  return materialized->get_sub_sequence(startIndex, endIndex);
}

template <class T>
int LazySequence<T>::get_length() const {
  if (length.is_infinite()) {
    throw std::logic_error("Бесконечная LazySequence не имеет конечной длины");
  }

  if (length.value() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
    throw std::overflow_error("Длина LazySequence не помещается в int");
  }

  return static_cast<int>(length.value());
}

template <class T>
Cardinal LazySequence<T>::get_cardinal_length() const {
  return length;
}

template <class T>
std::size_t LazySequence<T>::get_materialized_count() const {
  return static_cast<std::size_t>(materialized->get_length());
}

template <class T>
EnumeratorWrapper<T> LazySequence<T>::get_enumerator() const {
  throw std::logic_error("LazySequence не поддерживает перечисление через Sequence::get_enumerator");
}

template <class T>
LazySequence<T>* LazySequence<T>::append(const T &item) {
  if (length.is_infinite()) {
    return new LazySequence<T>(*this);
  }

  if (length.value() > 0) {
    materialize_until(static_cast<int>(length.value() - 1));
  }

  Sequence<T>* result = copy_sequence(materialized);
  Sequence<T>* updated = result->append(item);

  if (updated != result) {
    delete result;
    result = updated;
  }

  return new LazySequence<T>(result, Cardinal(length.value() + 1), nullptr);
}

template <class T>
LazySequence<T>* LazySequence<T>::prepend(const T &item) {
  if (length.is_infinite()) {
    return insert_at(item, 0);
  }

  if (length.value() > 0) {
    materialize_until(static_cast<int>(length.value() - 1));
  }

  Sequence<T>* result = copy_sequence(materialized);
  Sequence<T>* updated = result->prepend(item);

  if (updated != result) {
    delete result;
    result = updated;
  }

  return new LazySequence<T>(result, Cardinal(length.value() + 1), nullptr);
}

template <class T>
LazySequence<T>* LazySequence<T>::insert_at(const T &item, int index) {
  if (index < 0) {
    throw std::out_of_range("Индекс вне допустимого диапазона");
  }

  // Если последовательность является конечной
  if (length.is_finite()) {
    if (static_cast<std::size_t>(index) > length.value()) {
      throw std::out_of_range("Индекс вне допустимого диапазона");
    }

    // Материализуем всю последовательность, т.к она - конечная
    if (length.value() > 0) {
      materialize_until(static_cast<int>(length.value() - 1));
    }

    Sequence<T>* result_sequence = copy_sequence(materialized);
    result_sequence->insert_at(item, index);
    LazySequence<T>* result = new LazySequence<T>(result_sequence);

    delete result_sequence;
    return result;
  }

  // Если последовательность является бесконечной
  MutableArraySequence<T>* initial = new MutableArraySequence<T>();

  // Копируем все элементы до вставки
  for (int current_index = 0; current_index < index; current_index++) {
    initial->append(get(current_index));
  }

  // Вставляем новый элемент
  initial->append(item);

  LazySequence<T>* result = new LazySequence<T>(initial, Cardinal::infinity(), nullptr);

  try {
    struct InsertItemRule {
      LazySequence<T>* source;
      std::size_t source_index;

      InsertItemRule(LazySequence<T>* source, std::size_t source_index)
        : source(new LazySequence<T>(*source)),
          source_index(source_index) {}

      InsertItemRule(const InsertItemRule &other)
        : source(new LazySequence<T>(*other.source)),
          source_index(other.source_index) {}

      InsertItemRule& operator=(const InsertItemRule &other) {
        if (this == &other) {
          return *this;
        }

        delete source;
        source = new LazySequence<T>(*other.source);
        source_index = other.source_index;

        return *this;
      }

      T operator()(Sequence<T>*) {
        T next = source->get(static_cast<int>(source_index));
        source_index++;
        return next;
      }

      ~InsertItemRule() {
        delete source;
      }
    };

    result->generator = new RuleGenerator<T>(
      InsertItemRule(this, static_cast<std::size_t>(index)),
      *initial
    );
  } catch (...) {
    delete result;
    throw;
  }

  return result;
}

template <class T>
LazySequence<T>* LazySequence<T>::insert_at(LazySequence<T> *items, const Cardinal &index) {
  if (items == nullptr) {
    throw std::invalid_argument("Нельзя вставить нулевую LazySequence");
  }

  if (index.is_finite()) {
    throw std::logic_error("Вставка LazySequence по конечному индексу пока не поддерживается");
  }

  if (index.omega_offset() != 0) {
    throw std::out_of_range("Вставка поддерживается только по индексу omega");
  }

  if (length.is_finite()) {
    throw std::out_of_range("Индекс omega вне конечной LazySequence");
  }

  LazySequence<T>* result = new LazySequence<T>(*this);
  delete result->transfinite_tail;
  result->transfinite_tail = new LazySequence<T>(*items);

  return result;
}

template <class T>
LazySequence<T>* LazySequence<T>::remove_at(int index) {
  if (index < 0) {
    throw std::out_of_range("Индекс вне допустимого диапазона");
  }

  // Если последовательность является конечной
  if (length.is_finite()) {
    if (static_cast<std::size_t>(index) >= length.value()) {
      throw std::out_of_range("Индекс вне допустимого диапазона");
    }

    if (length.value() > 0) {
      materialize_until(static_cast<int>(length.value() - 1));
    }

    MutableArraySequence<T>* result_sequence = new MutableArraySequence<T>();

    for (std::size_t current_index = 0; current_index < length.value(); current_index++) {
      if (current_index != static_cast<std::size_t>(index)) {
        result_sequence->append(materialized->get(static_cast<int>(current_index)));
      }
    }

    LazySequence<T>* result = new LazySequence<T>(result_sequence);
    delete result_sequence;

    return result;
  }

  // Если последовательность является бесконечной
  MutableArraySequence<T>* initial = new MutableArraySequence<T>();

  LazySequence<T>* result = new LazySequence<T>(initial, Cardinal::infinity(), nullptr);

  try {
    struct RemoveItemRule {
      LazySequence<T>* source;
      std::size_t source_index;
      std::size_t remove_index;

      RemoveItemRule(LazySequence<T>* source, std::size_t remove_index)
        : source(new LazySequence<T>(*source)),
          source_index(0),
          remove_index(remove_index) {}

      RemoveItemRule(const RemoveItemRule &other)
        : source(new LazySequence<T>(*other.source)),
          source_index(other.source_index),
          remove_index(other.remove_index) {}

      RemoveItemRule& operator=(const RemoveItemRule &other) {
        if (this == &other) {
          return *this;
        }

        delete source;
        source = new LazySequence<T>(*other.source);
        source_index = other.source_index;
        remove_index = other.remove_index;

        return *this;
      }

      T operator()(Sequence<T>*) {
        if (source_index == remove_index) {
          source_index++;
        }

        T next = source->get(static_cast<int>(source_index));
        source_index++;
        return next;
      }

      ~RemoveItemRule() {
        delete source;
      }
    };

    result->generator = new RuleGenerator<T>(
      RemoveItemRule(this, static_cast<std::size_t>(index)),
      *initial
    );
  } catch (...) {
    delete result;
    throw;
  }

  return result;
}

template <class T>
LazySequence<T>* LazySequence<T>::concat(LazySequence<T>* other) {
  if (other == nullptr) {
    throw std::invalid_argument("Нельзя выполнить concat с нулевой последовательностью");
  }

  /*
   * Если левая последовательность является бесконечной, то метод concat
   * возвращает копию левой последовательности, потому что
   * правая часть является недостижимой
   */
  if (length.is_infinite()) {
    return new LazySequence<T>(*this);
  }

  if (length.value() > 0) {
    materialize_until(static_cast<int>(length.value() - 1));
  }

  // Если обе последовательности конечные, материализуем результат обычной последовательностью
  if (other->get_cardinal_length().is_finite()) {
    Cardinal list_length = other->get_cardinal_length();

    if (list_length.value() > 0) {
      other->materialize_until(static_cast<int>(list_length.value() - 1));
    }

    Sequence<T>* result_sequence = copy_sequence(materialized);

    for (std::size_t index = 0; index < list_length.value(); index++) {
      result_sequence->append(other->get(static_cast<int>(index)));
    }

    LazySequence<T>* result = new LazySequence<T>(result_sequence);
    delete result_sequence;

    return result;
  }

  std::size_t left_length = length.value();
  Sequence<T>* initial = copy_sequence(materialized);

  LazySequence<T>* result = new LazySequence<T>(initial, Cardinal::infinity(), nullptr);

  try {
    struct ConcatRule {
      LazySequence<T>* right;
      std::size_t right_index;

      explicit ConcatRule(LazySequence<T>* right)
        : right(new LazySequence<T>(*right)),
          right_index(0) {}

      ConcatRule(const ConcatRule &other)
        : right(new LazySequence<T>(*other.right)),
          right_index(other.right_index) {}

      ConcatRule& operator=(const ConcatRule &other) {
        if (this == &other) {
          return *this;
        }

        delete right;
        right = new LazySequence<T>(*other.right);
        right_index = other.right_index;

        return *this;
      }

      T operator()(Sequence<T>*) {
        T next = right->get(static_cast<int>(right_index));
        right_index++;
        return next;
      }

      ~ConcatRule() {
        delete right;
      }
    };

    result->generator = new RuleGenerator<T>(
      ConcatRule(other),
      *initial
    );
  } catch (...) {
    delete result;
    throw;
  }

  return result;
}

template <class T>
template <class T2>
LazySequence<T2>* LazySequence<T>::map(T2 (*func)(const T &item)) {
  if (func == nullptr) {
    throw std::invalid_argument("Нельзя выполнить map с нулевой функцией");
  }

  // Если последовательность является конечной, то применяем map как обычно
  if (length.is_finite()) {
    if (length.value() > 0) {
      materialize_until(static_cast<int>(length.value() - 1));
    }

    Sequence<T2>* mapped = materialized->template map<T2>(func);
    LazySequence<T2>* result = new LazySequence<T2>(mapped);

    delete mapped;
    return result;
  }

  // Если последовательность бесконечная
  MutableArraySequence<T2>* initial = new MutableArraySequence<T2>();

  // Правило хранит копию исходной последовательности и применяет func к очередному элементу
  struct MapRule {
    LazySequence<T>* source;
    T2 (*func)(const T &item);

    // Конструктор, который создаёт собственную копию исходной LazySequence
    MapRule(LazySequence<T>* source, T2 (*func)(const T &item))
      : source(new LazySequence<T>(*source)),
        func(func) {}

    // Копирующий конструктор
    MapRule(const MapRule &other)
      : source(new LazySequence<T>(*other.source)),
        func(other.func) {}

    // Оператор присваивания, который пересоздает копию source
    MapRule& operator=(const MapRule &other) {
      if (this == &other) {
        return *this;
      }

      delete source;
      source = new LazySequence<T>(*other.source);
      func = other.func;

      return *this;
    }

    T2 operator()(Sequence<T2>* current) const {
      int current_index = current->get_length();
      return func(source->get(current_index));
    }

    ~MapRule() {
      delete source;
    }
  };

  std::function<T2(Sequence<T2>*)> map_recurrence = MapRule(this, func);

  LazySequence<T2>* result = new LazySequence<T2>(map_recurrence, initial, 1);
  delete initial;

  return result;
}

template <class T>
template <class T2>
T2 LazySequence<T>::reduce(T2 (*func)(const T2 &accumulator, const T &current), const T2 &initial_element) {
  if (length.is_infinite()) {
    throw std::logic_error("reduce не может быть применён к бесконечной последовательности");
  }

  if (func == nullptr) {
    throw std::invalid_argument("Нельзя выполнить reduce с нулевой функцией");
  }

  T2 result = initial_element;

  for (std::size_t index = 0; index < length.value(); index++) {
    materialize_until(static_cast<int>(index));
    result = func(result, materialized->get(static_cast<int>(index)));
  }

  return result;
}

template <class T>
template <class T2>
T2 LazySequence<T>::reduce(
  T2 (*func)(const T2 &accumulator, const T &current),
  const T2 &initial_element,
  std::size_t limit
) {
  if (func == nullptr) {
    throw std::invalid_argument("Нельзя выполнить reduce с нулевой функцией");
  }

  T2 result = initial_element;

  for (std::size_t index = 0; index < limit; index++) {
    materialize_until(static_cast<int>(index));
    result = func(result, materialized->get(static_cast<int>(index)));
  }

  return result;
}

template <class T>
LazySequence<T>* LazySequence<T>::where(bool (*predicate)(const T &item)) {
  if (predicate == nullptr) {
    throw std::invalid_argument("Нельзя выполнить where с нулевым предикатом");
  }

  // Если последовательность конечная
  if (length.is_finite()) {
    if (length.value() > 0) {
      materialize_until(static_cast<int>(length.value() - 1));
    }

    Sequence<T>* filtered = materialized->where(predicate);
    LazySequence<T>* result = new LazySequence<T>(filtered);

    delete filtered;
    return result;
  }

  // Если последовательность бесконечная
  MutableArraySequence<T>* initial = new MutableArraySequence<T>();

  // Правило хранит копию исходной последовательности и текущую позицию поиска
  struct WhereRule {
    LazySequence<T>* source;
    int source_index;
    bool (*predicate)(const T &item);

    // Создаем собственную копию исходной последовательности для ленивой фильтрации
    WhereRule(LazySequence<T>* source, bool (*predicate)(const T &item))
      : source(new LazySequence<T>(*source)),
        source_index(0),
        predicate(predicate) {}

    // std::function копирует правило, поэтому копия тоже должна владеть своим source
    WhereRule(const WhereRule &other)
      : source(new LazySequence<T>(*other.source)),
        source_index(other.source_index),
        predicate(other.predicate) {}

    // Поддерживаем корректное копирующее присваивание для хранения внутри std::function
    WhereRule& operator=(const WhereRule &other) {
      if (this == &other) {
        return *this;
      }

      delete source;
      source = new LazySequence<T>(*other.source);
      source_index = other.source_index;
      predicate = other.predicate;

      return *this;
    }

    // Ищем следующий элемент исходной последовательности, который проходит predicate
    T operator()(Sequence<T>*) {
      while (true) {
        T item = source->get(source_index);
        source_index++;

        if (predicate(item)) {
          return item;
        }
      }
    }

    ~WhereRule() {
      delete source;
    }
  };

  std::function<T(Sequence<T>*)> where_recurrence = WhereRule(this, predicate);

  LazySequence<T>* result = new LazySequence<T>(where_recurrence, initial, 1);
  delete initial;

  return result;
}

template <class T>
template <class T2>
LazySequence<Pair<T, T2>>* LazySequence<T>::zip(Sequence<T2>* seq) {
  if (seq == nullptr) {
    throw std::invalid_argument("Нельзя выполнить zip с нулевой последовательностью");
  }

  // Правая Sequence всегда конечная, поэтому результат тоже будет конечным
  int result_length = seq->get_length();

  // Если левая LazySequence конечная и короче, ограничиваем результат ее длиной
  if (length.is_finite() && length.value() < static_cast<std::size_t>(result_length)) {
    result_length = static_cast<int>(length.value());
  }

  MutableArraySequence<Pair<T, T2>>* zipped = new MutableArraySequence<Pair<T, T2>>();

  // Материализуем только пары, которые реально попадают в конечный результат
  for (int index = 0; index < result_length; index++) {
    zipped->append(Pair<T, T2>(get(index), seq->get(index)));
  }

  LazySequence<Pair<T, T2>>* result = new LazySequence<Pair<T, T2>>(zipped);
  delete zipped;

  return result;
}

template <class T>
template <class T2>
LazySequence<Pair<T, T2>>* LazySequence<T>::zip(LazySequence<T2>* seq) {
  if (seq == nullptr) {
    throw std::invalid_argument("Нельзя выполнить zip с нулевой последовательностью");
  }

  Cardinal right_length = seq->get_cardinal_length();

  // Если хотя бы одна последовательность конечная, zip тоже конечный
  if (length.is_finite() || right_length.is_finite()) {
    std::size_t result_length = 0;

    if (length.is_finite() && right_length.is_finite()) {
      result_length = length.value() < right_length.value() ? length.value() : right_length.value();
    } else if (length.is_finite()) {
      result_length = length.value();
    } else {
      result_length = right_length.value();
    }

    MutableArraySequence<Pair<T, T2>>* zipped = new MutableArraySequence<Pair<T, T2>>();

    // Материализуем конечное количество пар до длины более короткой последовательности
    for (std::size_t index = 0; index < result_length; index++) {
      zipped->append(Pair<T, T2>(get(static_cast<int>(index)), seq->get(static_cast<int>(index))));
    }

    LazySequence<Pair<T, T2>>* result = new LazySequence<Pair<T, T2>>(zipped);
    delete zipped;

    return result;
  }

  // Если обе последовательности бесконечные, строим бесконечный lazy zip
  MutableArraySequence<Pair<T, T2>>* initial = new MutableArraySequence<Pair<T, T2>>();

  // Правило хранит копии обеих последовательностей и соединяет элементы с одинаковым индексом
  struct ZipRule {
    LazySequence<T>* left;
    LazySequence<T2>* right;

    // Создаем собственные копии обеих последовательностей для ленивого zip
    ZipRule(LazySequence<T>* left, LazySequence<T2>* right)
      : left(new LazySequence<T>(*left)),
        right(new LazySequence<T2>(*right)) {}

    // std::function копирует правило, поэтому копия тоже должна владеть своими source
    ZipRule(const ZipRule &other)
      : left(new LazySequence<T>(*other.left)),
        right(new LazySequence<T2>(*other.right)) {}

    // Поддерживаем корректное копирующее присваивание для хранения внутри std::function
    ZipRule& operator=(const ZipRule &other) {
      if (this == &other) {
        return *this;
      }

      delete left;
      delete right;
      left = new LazySequence<T>(*other.left);
      right = new LazySequence<T2>(*other.right);

      return *this;
    }

    // Берем элементы с одинаковым индексом и объединяем их в Pair
    Pair<T, T2> operator()(Sequence<Pair<T, T2>>* current) const {
      int current_index = current->get_length();
      return Pair<T, T2>(left->get(current_index), right->get(current_index));
    }

    ~ZipRule() {
      delete left;
      delete right;
    }
  };

  std::function<Pair<T, T2>(Sequence<Pair<T, T2>>*)> zip_recurrence = ZipRule(this, seq);

  LazySequence<Pair<T, T2>>* result = new LazySequence<Pair<T, T2>>(zip_recurrence, initial, 1);
  delete initial;

  return result;
}

template <class T>
LazySequence<T>::~LazySequence() {
  delete transfinite_tail;
  delete generator;
  delete materialized;
}

#endif // LABORATORYWORK4_LAZY_SEQUENCE_TPP
