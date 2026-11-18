#include "simulation.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <stdexcept>
#include <string>

/**
 * @brief Проверяет корректность параметров моделирования.
 */
static void validateConfig(const SimulationConfig& config)
{
    if (config.dt <= 0.0)
    {
        throw std::invalid_argument("Time step must be greater than zero");
    }

    if (!std::isfinite(config.dt) ||
        !std::isfinite(config.setpoint) ||
        !std::isfinite(config.initialY) ||
        !std::isfinite(config.epsilon))
    {
        throw std::invalid_argument("Simulation parameters must be finite");
    }

    // ИСПРАВЛЕНИЕ 1: Объединение вложенных if (Merge this "if" statement)
    if (config.mode == SimulationMode::FixedSteps && config.steps <= 0)
    {
        throw std::invalid_argument("Number of steps must be greater than zero");
    }

    if (config.mode == SimulationMode::UntilSettled && config.epsilon <= 0.0)
    {
        throw std::invalid_argument("Epsilon must be greater than zero");
    }

    if (config.mode == SimulationMode::UntilSettled && config.maxSteps <= 0)
    {
        throw std::invalid_argument("Maximum number of steps must be greater than zero");
    }
}

/**
 * @brief Выполняет моделирование системы управления.
 */
SimulationResult runSimulation(
    Model& model,
    PIDController& pid,
    const SimulationConfig& config
)
{
    validateConfig(config);

    SimulationResult result;
    result.mode = config.mode;

    // Начальная инициализация объекта и регулятора.
    model.reset();
    pid.reset();

    double y = config.initialY;

    // Сохраняем начальное состояние системы.
    double initialError = config.setpoint - y;

    result.rows.push_back({
        0,
        0.0,
        config.setpoint,
        y,
        initialError,
        0.0
        });

    // Проверяем, достигнута ли точность уже в начальный момент.
    if (config.mode == SimulationMode::UntilSettled &&
        std::fabs(initialError) <= config.epsilon)
    {
        result.settled = true;
        return result;
    }

    int totalSteps = config.steps;

    if (config.mode == SimulationMode::UntilSettled)
    {
        totalSteps = config.maxSteps;
    }

    // Основной цикл моделирования.
    for (int i = 1; i <= totalSteps; i++)
    {
        // Вычисление ошибки регулирования.
        double error = config.setpoint - y;

        // Расчёт управляющего воздействия PID-регулятором.
        double u = pid.calculate(error, config.dt);

        // Вычисление нового состояния объекта.
        double nextY = model.calculateNext(y, u, config.dt);

        if (!std::isfinite(nextY))
        {
            // ИСПРАВЛЕНИЕ 2: Использование специализированного исключения для математических ошибок
            throw std::domain_error("The mathematical model returned a non-finite value");
        }

        // Переход к следующему состоянию.
        y = nextY;

        // Ошибка после перехода объекта в новое состояние.
        double nextError = config.setpoint - y;

        // Сохранение результатов временного шага.
        double currentTime = i * config.dt;

        result.rows.push_back({
            i,
            currentTime,
            config.setpoint,
            y,
            nextError,
            u
            });

        result.stepsPerformed = i;

        // Проверка достижения установившегося режима.
        if (config.mode == SimulationMode::UntilSettled &&
            std::fabs(nextError) <= config.epsilon)
        {
            result.settled = true;
            break;
        }
    }

    return result;
}

/**
 * @brief Выводит таблицу результатов в консоль.
 */
void printSimulationResults(const SimulationResult& result)
{
    std::cout << std::fixed << std::setprecision(4);

    std::cout << "\n";
    std::cout << "============================================================\n";
    std::cout << "                 SIMULATION RESULTS\n";
    std::cout << "============================================================\n";

    std::cout << std::left
        << std::setw(8) << "Step"
        << std::setw(12) << "Time"
        << std::setw(14) << "Setpoint"
        << std::setw(14) << "Output"
        << std::setw(14) << "Error"
        << std::setw(14) << "Control"
        << "\n";

    std::cout << "------------------------------------------------------------\n";

    for (const SimulationRow& row : result.rows)
    {
        std::cout << std::left
            << std::setw(8) << row.step
            << std::setw(12) << row.time
            << std::setw(14) << row.setpoint
            << std::setw(14) << row.y
            << std::setw(14) << row.error
            << std::setw(14) << row.u
            << "\n";
    }

    std::cout << "------------------------------------------------------------\n";

    std::cout << "Steps performed: " << result.stepsPerformed << "\n";

    if (result.mode == SimulationMode::FixedSteps)
    {
        std::cout << "Status: Fixed-step simulation completed\n";
    }
    else if (result.settled)
    {
        std::cout << "Status: Settled regime reached\n";
    }
    else
    {
        std::cout << "Status: Maximum number of steps reached\n";
    }

    std::cout << "============================================================\n";
}

/**
 * @brief Сохраняет результаты в CSV-файл.
 */
 /**
  * @brief Сохраняет результаты в CSV-файл.
  */
bool exportSimulationCSV(
    const SimulationResult& result,
    const std::string& fileName
)
{
    // ЗАЩИТА ОТ PATH INJECTION: запрещаем символы перехода по директориям
    if (fileName.find("..") != std::string::npos ||
        fileName.find('/') != std::string::npos ||
        fileName.find('\\') != std::string::npos)
    {
        throw std::invalid_argument("Invalid file name: directory traversal is not allowed");
    }

    std::ofstream file(fileName);

    if (!file.is_open())
    {
        return false;
    }

    file << "Step,Time,Setpoint,Output,Error,Control\n";
    file << std::fixed << std::setprecision(6);

    for (const SimulationRow& row : result.rows)
    {
        file << row.step << ","
            << row.time << ","
            << row.setpoint << ","
            << row.y << ","
            << row.error << ","
            << row.u << "\n";
    }

    file.close();
    return true;
}