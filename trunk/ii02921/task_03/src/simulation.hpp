
#pragma once

#include <vector>
#include <string>

#include "models.hpp"
#include "pid_controller.hpp"

/**
 * @brief Режим проведения моделирования.
 *
 * FixedSteps - моделирование в течение заданного числа шагов.
 * UntilSettled - моделирование до достижения заданной точности.
 */
enum class SimulationMode
{
    FixedSteps,
    UntilSettled
};


/**
 * @brief Параметры одного эксперимента.
 *
 * Содержит настройки объекта управления,
 * PID-регулятора и временных характеристик моделирования.
 */
struct SimulationConfig
{
    SimulationMode mode = SimulationMode::FixedSteps;

    double setpoint = 25.0;
    double initialY = 0.0;
    double dt = 0.1;

    int steps = 100;
    int maxSteps = 10000;

    double epsilon = 0.01;
};


/**
 * @brief Результаты одного временного шага.
 *
 * Хранит значения времени, уставки, выходной переменной,
 * ошибки регулирования и управляющего воздействия.
 */
struct SimulationRow
{
    int step;
    double time;
    double setpoint;
    double y;
    double error;
    double u;
};


/**
 * @brief Полный результат моделирования.
 *
 * Содержит таблицу результатов, признак достижения
 * установившегося режима и количество выполненных шагов.
 */
struct SimulationResult
{
    std::vector<SimulationRow> rows;

    SimulationMode mode = SimulationMode::FixedSteps;

    bool settled = false;
    int stepsPerformed = 0;
};


/**
 * @brief Выполняет моделирование замкнутой системы управления.
 *
 * @param model Математическая модель объекта управления.
 * @param pid PID-регулятор.
 * @param config Параметры эксперимента.
 * @return Результаты моделирования.
 */
SimulationResult runSimulation(
    Model& model,
    PIDController& pid,
    const SimulationConfig& config
);


/**
 * @brief Выводит результаты моделирования в консоль.
 *
 * @param result Результаты эксперимента.
 */
void printSimulationResults(const SimulationResult& result);


/**
 * @brief Экспортирует результаты в CSV-файл.
 *
 * @param result Результаты эксперимента.
 * @param fileName Имя выходного файла.
 * @return true, если файл успешно записан.
 */
bool exportSimulationCSV(
    const SimulationResult& result,
    const std::string& fileName
);
