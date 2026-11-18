
#include "simulation.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <stdexcept>

using namespace std;


/**
 * @brief Проверяет корректность параметров моделирования.
 */
static void validateConfig(const SimulationConfig& config)
{
    if (config.dt <= 0.0)
    {
        throw invalid_argument(
            "Time step must be greater than zero"
        );
    }

    if (!isfinite(config.dt) ||
        !isfinite(config.setpoint) ||
        !isfinite(config.initialY) ||
        !isfinite(config.epsilon))
    {
        throw invalid_argument(
            "Simulation parameters must be finite"
        );
    }

    if (config.mode == SimulationMode::FixedSteps)
    {
        if (config.steps <= 0)
        {
            throw invalid_argument(
                "Number of steps must be greater than zero"
            );
        }
    }

    if (config.mode == SimulationMode::UntilSettled)
    {
        if (config.epsilon <= 0.0)
        {
            throw invalid_argument(
                "Epsilon must be greater than zero"
            );
        }

        if (config.maxSteps <= 0)
        {
            throw invalid_argument(
                "Maximum number of steps must be greater than zero"
            );
        }
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
        fabs(initialError) <= config.epsilon)
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

        if (!isfinite(nextY))
        {
            throw runtime_error(
                "The mathematical model returned a non-finite value"
            );
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
            fabs(nextError) <= config.epsilon)
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
    cout << fixed << setprecision(4);

    cout << "\n";
    cout << "============================================================\n";
    cout << "                 SIMULATION RESULTS\n";
    cout << "============================================================\n";

    cout << left
        << setw(8) << "Step"
        << setw(12) << "Time"
        << setw(14) << "Setpoint"
        << setw(14) << "Output"
        << setw(14) << "Error"
        << setw(14) << "Control"
        << "\n";

    cout << "------------------------------------------------------------\n";

    for (const SimulationRow& row : result.rows)
    {
        cout << left
            << setw(8) << row.step
            << setw(12) << row.time
            << setw(14) << row.setpoint
            << setw(14) << row.y
            << setw(14) << row.error
            << setw(14) << row.u
            << "\n";
    }

    cout << "------------------------------------------------------------\n";

    cout << "Steps performed: " << result.stepsPerformed << "\n";

    if (result.mode == SimulationMode::FixedSteps)
    {
        cout << "Status: Fixed-step simulation completed\n";
    }
    else if (result.settled)
    {
        cout << "Status: Settled regime reached\n";
    }
    else
    {
        cout << "Status: Maximum number of steps reached\n";
    }

    cout << "============================================================\n";
}


/**
 * @brief Сохраняет результаты в CSV-файл.
 */
bool exportSimulationCSV(
    const SimulationResult& result,
    const string& fileName
)
{
    ofstream file(fileName);

    if (!file.is_open())
    {
        return false;
    }

    file << "Step,Time,Setpoint,Output,Error,Control\n";

    file << fixed << setprecision(6);

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
