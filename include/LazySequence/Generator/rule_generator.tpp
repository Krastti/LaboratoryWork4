#ifndef LABORATORYWORK4_RULE_GENERATOR_TPP
#define LABORATORYWORK4_RULE_GENERATOR_TPP

#include <cstddef>
#include <stdexcept>

template <class T>
RuleGenerator<T>::RuleGenerator(Rule rule)
  : rule(rule),
    context(1) {
  if (!rule) {
    throw std::invalid_argument("Правило генерации не может быть пустым");
  }
}

template <class T>
RuleGenerator<T>::RuleGenerator(Rule rule, const Sequence<T> &initial_context)
  : rule(rule),
    context(initial_context.get_length() > 0 ? static_cast<std::size_t>(initial_context.get_length()) : 1) {
  if (!rule) {
    throw std::invalid_argument("Правило генерации не может быть пустым");
  }

  for (int index = 0; index < initial_context.get_length(); index++) {
    context.push_back(initial_context.get(index));
  }
}

template <class T>
T RuleGenerator<T>::get_next() {
  Sequence<T>* context_sequence = context.to_sequence();

  try {
    T item = rule(context_sequence);
    delete context_sequence;

    context.push_back(item);
    return item;
  } catch (...) {
    delete context_sequence;
    throw;
  }
}

template <class T>
bool RuleGenerator<T>::has_next() const {
  return static_cast<bool>(rule);
}

template <class T>
BaseGenerator<T>* RuleGenerator<T>::clone() const {
  return new RuleGenerator<T>(*this);
}

#endif // LABORATORYWORK4_RULE_GENERATOR_TPP
