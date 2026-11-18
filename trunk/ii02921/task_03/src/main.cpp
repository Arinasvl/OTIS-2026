#include <iostream>
#include <cstdlib>
#include <memory>
#include <string>
#include <limits>
#include <cmath>
#include <clocale>
#include <exception>

#include "models.hpp"
#include "pid_controller.hpp"
#include "simulation.hpp"

using namespace std;

// ============================================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ВВОДА
// ============================================================

double readDouble(const string& message)
{
    double value;
    while (true)
    {
        cout << message;
        cin >> value;
        if (cin && isfinite(value))
        {
            return value;
        }
        cout << "Ошибка! Введите корректное конечное число.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int readInt(const string& message)
{
    int value;
    while (true)
    {
        cout << message;
        cin >> value;
        if (cin)
        {
            return value;
        }
        cout << "Ошибка! Введите целое число.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// ============================================================
// ГЛАВНАЯ ФУНКЦИЯ
// ============================================================

int main()
{
    setlocale(LC_ALL, "");

    try
    {
        cout << "====================================\n";
        cout << "    PID CONTROL SYSTEM - TASK 3\n";
        cout << "====================================\n";

        // 1. Выбор математической модели
        cout << "\n====================================\n";
        cout << "       MODEL SELECTION\n";
        cout << "====================================\n";
        cout << "1. Model 1.10 - Linear model with delay\n";
        cout << "2. Model 2.4  - Nonlinear model\n";
        cout << "3. Model 3.8  - Differential equation\n";

        int modelChoice = readInt("Choose model (1-3): ");
        while (modelChoice < 1 || modelChoice > 3)
        {
            cout << "Invalid choice. Select 1, 2 or 3.\n";
            modelChoice = readInt("Choose model (1-3): ");
        }

        unique_ptr<Model> model = nullptr;
        if (modelChoice == 1)
        {
            cout << "\nParameters of Model 1.10:\n";
            double a1 = readDouble("Enter a1: ");
            double b1 = readDouble("Enter b1: ");
            double b2 = readDouble("Enter b2: ");
            model = make_unique<Model1_10>(a1, b1, b2);
        }
        else if (modelChoice == 2)
        {
            cout << "\nParameters of Model 2.4:\n";
            double a = readDouble("Enter a: ");
            double b = readDouble("Enter b: ");
            model = make_unique<Model2_4>(a, b);
        }
        else if (modelChoice == 3)
        {
            cout << "\nParameters of Model 3.8:\n";
            double a1 = readDouble("Enter a1: ");
            double a2 = readDouble("Enter a2: ");
            double b = readDouble("Enter b: ");
            model = make_unique<Model3_8>(a1, a2, b);
        }

        cout << "\nSelected model: " << model->getName() << "\n";

        // 2. Настройка PID-регулятора
        cout << "\n====================================\n";
        cout << "       PID CONFIGURATION\n";
        cout << "====================================\n";
        double kp = readDouble("Enter Kp: ");
        double ki = readDouble("Enter Ki: ");
        double kd = readDouble("Enter Kd: ");
        cout << "\nControl signal limits:\n";
        double minOutput = readDouble("Enter minimum output: ");
        double maxOutput = readDouble("Enter maximum output: ");

        PIDController pid(kp, ki, kd, minOutput, maxOutput);

        // 3. Настройка параметров моделирования
        cout << "\n====================================\n";
        cout << "      SIMULATION CONFIGURATION\n";
        cout << "====================================\n";
        cout << "1. Fixed number of steps (Mode A)\n";
        cout << "2. Until required accuracy (Mode B)\n";

        int modeChoice = readInt("Choose simulation mode (1-2): ");
        while (modeChoice != 1 && modeChoice != 2)
        {
            cout << "Invalid choice. Select 1 or 2.\n";
            modeChoice = readInt("Choose simulation mode (1-2): ");
        }

        SimulationConfig config;
        if (modeChoice == 1)
        {
            config.mode = SimulationMode::FixedSteps;
            config.steps = readInt("Enter number of simulation steps: ");
        }
        else
        {
            config.mode = SimulationMode::UntilSettled;
            config.epsilon = readDouble("Enter required accuracy epsilon: ");
            config.maxSteps = readInt("Enter maximum number of steps: ");
        }

        cout << "\nInitial conditions:\n";
        config.setpoint = readDouble("Enter setpoint: ");
        config.initialY = readDouble("Enter initial output value: ");
        config.dt = readDouble("Enter time step dt: ");

        // 4. Запуск моделирования
        cout << "\nStarting simulation...\n";
        SimulationResult result = runSimulation(*model, pid, config);

        // 5. Вывод результатов
        printSimulationResults(result);

        // 6. Сохранение в CSV
                // 6. Сохранение в CSV и автоматический запуск графиков
        cout << "\nEnter CSV file name (e.g., result.csv): ";
        string fileName;
        cin >> fileName;

        if (exportSimulationCSV(result, fileName))
        {
            cout << "[OK] Results successfully saved to " << fileName << "\n";

            // Формируем команду для запуска Python-скрипта
            // Оборачиваем имя файла в кавычки на случай, если в нём есть пробелы
                // Запуск скрипта построения графика
            std::string plotCommand = "python plot_results.py " + csvFileName;

            // Проверка на инъекцию команд (требование SonarQube)
            if (plotCommand.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.- ") != std::string::npos) {
                std::cerr << "Ошибка: недопустимые символы в команде запуска графика.\n";
            }
            else {
                // Правильное объявление переменной ret
                int ret = std::system(plotCommand.c_str());
                if (ret != 0) {
                    std::cerr << "Предупреждение: скрипт графика завершился с кодом ошибки " << ret << "\n";
                }
            }
        else
        {
            cout << "[ERROR] Could not create CSV file.\n";
        }

        cout << "\nSimulation completed successfully.\n";
    }
    catch (const exception& e)
    {
        cerr << "\nPROGRAM ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}