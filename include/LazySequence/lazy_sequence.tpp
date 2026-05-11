#ifndef LABORATORYWORK4_LAZY_SEQUENCE_TPP
#define LABORATORYWORK4_LAZY_SEQUENCE_TPP

#include "lazy_sequence.h"

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
Sequence<T>* LazySequence<T>::make_initial_for_recurrence(
  T (*recurrence)(Sequence<T>*),
  const Sequence<T> &initial
) {
  if (recurrence == nullptr) {
    throw std::invalid_argument("Порождающее правило не может быть нулевым");
  }

  return copy_sequence(&initial);
}

template <class T>
Sequence<T>* LazySequence<T>::make_initial_for_recurrence(
  const std::function<T(Sequence<T>*)> &recurrence,
  Sequence<T>* initial
) {
  if (!recurrence) {
    throw std::invalid_argument("Порождающее правило не может быть пустым");
  }

  return copy_sequence(initial);
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
void LazySequence<T>::materialize_until(int index) {
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
bool LazySequence<T>::try_materialize_next() {
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
LazySequence<T>::LazySequence()
  : materialized(new MutableArraySequence<T>()),
    length(0),
    generator(nullptr) {}

template <class T>
LazySequence<T>::LazySequence(Sequence<T>* materialized, Cardinal length, Generator<T>* generator)
  : materialized(materialized),
    length(length),
    generator(generator) {
  if (materialized == nullptr) {
    throw std::invalid_argument("Материализованная часть не может быть нулевой");
  }
}

template <class T>
LazySequence<T>::LazySequence(const T* items, int count)
  : materialized(make_from_array(items, count)),
    length(static_cast<std::size_t>(count)),
    generator(nullptr) {}

template <class T>
LazySequence<T>::LazySequence(Sequence<T>* seq)
  : materialized(copy_sequence(seq)),
    length(0),
    generator(nullptr) {
  length = Cardinal(static_cast<std::size_t>(materialized->get_length()));
}

template <class T>
LazySequence<T>::LazySequence(T (*recurrence)(Sequence<T>*), const Sequence<T> &initial)
  : materialized(make_initial_for_recurrence(recurrence, initial)),
    length(Cardinal::infinity()),
    generator(new Generator<T>(this, recurrence, materialized)) {}

template <class T>
LazySequence<T>::LazySequence(std::function<T(Sequence<T>*)> recurrence, Sequence<T>* initial, std::size_t context_size)
  : materialized(make_initial_for_recurrence(recurrence, initial)),
    length(Cardinal::infinity()),
    generator(new Generator<T>(this, recurrence, materialized, context_size)) {}

template <class T>
LazySequence<T>::LazySequence(const LazySequence<T> &other)
  : materialized(copy_sequence(other.materialized)),
    length(other.length),
    generator(nullptr) {
  if (other.generator != nullptr) {
    generator = new Generator<T>(*other.generator, this);
  }
}

template <class T>
const T& LazySequence<T>::get_first() {
  return get(0);
}

template <class T>
const T& LazySequence<T>::get_last() {
  if (length.is_infinite()) {
    throw std::logic_error("У бесконечной последовательности нет последнего элемента");
  }

  if (length.value() == 0) {
    throw std::out_of_range("Индекс вне допустимого диапазона");
  }

  return get(static_cast<int>(length.value() - 1));
}

template <class T>
const T& LazySequence<T>::get(int index) {
  materialize_until(index);
  return materialized->get(index);
}

template <class T>
LazySequence<T>* LazySequence<T>::get_subsequence(int startIndex, int endIndex) {
  if (startIndex < 0 || endIndex < 0 || startIndex > endIndex) {
    throw std::out_of_range("Индекс вне допустимого диапазона");
  }

  materialize_until(endIndex);

  Sequence<T>* subsequence = materialized->get_sub_sequence(startIndex, endIndex);
  LazySequence<T>* result = new LazySequence<T>(subsequence);

  delete subsequence;
  return result;
}

template <class T>
Cardinal LazySequence<T>::get_length() const {
  return length;
}

template <class T>
std::size_t LazySequence<T>::get_materialized_count() const {
  return static_cast<std::size_t>(materialized->get_length());
}

template <class T>
Sequence<T>* LazySequence<T>::append(const T &item) {
  if (length.is_infinite()) {
    throw std::logic_error("Нельзя вернуть бесконечный LazySequence как Sequence");
  }

  if (length.value() > 0) {
    materialize_until(static_cast<int>(length.value() - 1));
  }

  Sequence<T>* result = copy_sequence(materialized);
  result->append(item);

  return result;
}

template <class T>
Sequence<T>* LazySequence<T>::prepend(const T &item) {
  if (length.is_infinite()) {
    throw std::logic_error("Нельзя вернуть бесконечный LazySequence как Sequence");
  }

  if (length.value() > 0) {
    materialize_until(static_cast<int>(length.value() - 1));
  }

  Sequence<T>* result = copy_sequence(materialized);
  result->prepend(item);

  return result;
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
    result->generator = new Generator<T>(
      result,
      this,
      static_cast<std::size_t>(index),
      item,
      GeneratorChangeKind::Insert,
      static_cast<std::size_t>(initial->get_length())
    );
  } catch (...) {
    delete result;
    throw;
  }

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
    // Генератор хранит копию исходной последовательности и пропускает удаляемый элемент
    result->generator = new Generator<T>(
      result,
      this,
      static_cast<std::size_t>(index),
      GeneratorChangeKind::Remove,
      0,
      1
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
  if (other->get_length().is_finite()) {
    Cardinal list_length = other->get_length();

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
    // Генератор хранит копию правой последовательности и продолжает результат после левой части
    result->generator = new Generator<T>(
      result,
      other,
      left_length,
      GeneratorChangeKind::Append,
      left_length
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

  Cardinal right_length = seq->get_length();

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
  delete generator;
  delete materialized;
}

#endif // LABORATORYWORK4_LAZY_SEQUENCE_TPP
