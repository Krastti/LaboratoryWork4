#ifndef LABORATORYWORK4_CONFIGURATOR_H
#define LABORATORYWORK4_CONFIGURATOR_H

#include "../include/StateMachine/state_machine.h"

class StateMachineConfigurator {
public:
  /**
   * Сохраняет состояния, начальное состояние и переходы автомата в файл
   */
  static void export_to_file(const StateMachine &machine, const char* file_path);

  /**
   * Загружает автомат из файла без восстановления функций-действий
   */
  static StateMachine import_from_file(const char* file_path);
};

#endif // LABORATORYWORK4_CONFIGURATOR_H
