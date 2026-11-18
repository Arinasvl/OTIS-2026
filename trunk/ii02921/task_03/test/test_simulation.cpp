#include <gtest/gtest.h>
#include <stdexcept>
#include <cstdio>
#include <fstream>
#include <string>
#include "simulation.hpp"

// ============================================================
// ТЕСТ 1. Режим A: фиксированное количество шагов
// ============================================================
TEST(SimulationTest, FixedStepsModePerformsCorrectNumberOfSteps)
{
    Model3_8 model(0.0, 0.0, 1.0);
    PIDController pid(1.0, 0.0, 0.0, -100.0, 100.0);

    SimulationConfig config;
    config.mode = SimulationMode::FixedSteps;
    config.setpoint = 1.0;
    config.initialY = 0.0;
    config.dt = 0.1;
    config.steps = 5;

    SimulationResult result = runSimulation(model, pid, config);

    EXPECT_EQ(result.stepsPerformed, 5);
    EXPECT_EQ(result.rows.size(), 6); // + начальная строка
    EXPECT_DOUBLE_EQ(result.rows.front().time, 0.0);
    EXPECT_NEAR(result.rows.back().time, 0.5, 1e-9);
    EXPECT_FALSE(result.settled);
}

// ============================================================
// ТЕСТ 2. Режим B: точность достигнута сразу
// ============================================================
TEST(SimulationTest, UntilSettledModeStopsIfInitialErrorIsSmall)
{
    Model3_8 model(1.0, 0.0, 1.0);
    PIDController pid(1.0, 0.0, 0.0, -100.0, 100.0);

    SimulationConfig config;
    config.mode = SimulationMode::UntilSettled;
    config.setpoint = 5.0;
    config.initialY = 5.0; // ошибка = 0 < epsilon
    config.dt = 0.1;
    config.epsilon = 0.01;
    config.maxSteps = 100;

    SimulationResult result = runSimulation(model, pid, config);

    EXPECT_TRUE(result.settled);
    EXPECT_EQ(result.stepsPerformed, 0);
    EXPECT_EQ(result.rows.size(), 1);
}

// ============================================================
// ТЕСТ 3. Режим B: система реально достигает уставки
// ============================================================
TEST(SimulationTest, UntilSettledReachesSetpoint)
{
    // Интегратор: dy/dt = u
    Model3_8 model(0.0, 0.0, 1.0);
    PIDController pid(2.0, 0.0, 0.0, -100.0, 100.0);

    SimulationConfig config;
    config.mode = SimulationMode::UntilSettled;
    config.setpoint = 10.0;
    config.initialY = 0.0;
    config.dt = 0.1;
    config.epsilon = 0.01;
    config.maxSteps = 1000;

    SimulationResult result = runSimulation(model, pid, config);

    EXPECT_TRUE(result.settled);
    EXPECT_LE(result.stepsPerformed, 1000);

    // Финальная ошибка должна быть меньше epsilon
    double finalError = std::fabs(result.rows.back().error);
    EXPECT_LE(finalError, 0.01);
}

// ============================================================
// ТЕСТ 4. Режим B: maxSteps ограничивает цикл
// ============================================================
TEST(SimulationTest, UntilSettledRespectsMaxSteps)
{
    // Очень медленная модель
    Model3_8 model(0.0, 0.0, 0.001);
    PIDController pid(1.0, 0.0, 0.0, -10.0, 10.0);

    SimulationConfig config;
    config.mode = SimulationMode::UntilSettled;
    config.setpoint = 1000.0;
    config.initialY = 0.0;
    config.dt = 0.1;
    config.epsilon = 0.001;
    config.maxSteps = 50;

    SimulationResult result = runSimulation(model, pid, config);

    // Не должна успеть достичь уставки
    EXPECT_FALSE(result.settled);
    EXPECT_EQ(result.stepsPerformed, 50);
}

// ============================================================
// ТЕСТ 5. Некорректный dt вызывает исключение
// ============================================================
TEST(SimulationTest, InvalidTimeStepThrowsException)
{
    Model3_8 model(1.0, 0.0, 1.0);
    PIDController pid(1.0, 0.0, 0.0, -100.0, 100.0);

    SimulationConfig config;
    config.mode = SimulationMode::FixedSteps;
    config.setpoint = 5.0;
    config.initialY = 0.0;
    config.dt = 0.0;
    config.steps = 10;

    EXPECT_THROW(runSimulation(model, pid, config), std::invalid_argument);
}

// ============================================================
// ТЕСТ 6. Нулевое количество шагов вызывает исключение
// ============================================================
TEST(SimulationTest, ZeroStepsThrowsException)
{
    Model3_8 model(1.0, 0.0, 1.0);
    PIDController pid(1.0, 0.0, 0.0, -100.0, 100.0);

    SimulationConfig config;
    config.mode = SimulationMode::FixedSteps;
    config.setpoint = 5.0;
    config.initialY = 0.0;
    config.dt = 0.1;
    config.steps = 0;

    EXPECT_THROW(runSimulation(model, pid, config), std::invalid_argument);
}

// ============================================================
// ТЕСТ 7. Отрицательный epsilon вызывает исключение
// ============================================================
TEST(SimulationTest, NegativeEpsilonThrowsException)
{
    Model3_8 model(1.0, 0.0, 1.0);
    PIDController pid(1.0, 0.0, 0.0, -100.0, 100.0);

    SimulationConfig config;
    config.mode = SimulationMode::UntilSettled;
    config.setpoint = 5.0;
    config.initialY = 0.0;
    config.dt = 0.1;
    config.epsilon = -0.01;
    config.maxSteps = 100;

    EXPECT_THROW(runSimulation(model, pid, config), std::invalid_argument);
}

// ============================================================
// ТЕСТ 8. Экспорт в CSV создаёт файл с правильным количеством строк
// ============================================================
TEST(SimulationTest, CSVExportCreatesCorrectFile)
{
    Model3_8 model(0.0, 0.0, 1.0);
    PIDController pid(1.0, 0.0, 0.0, -100.0, 100.0);

    SimulationConfig config;
    config.mode = SimulationMode::FixedSteps;
    config.setpoint = 1.0;
    config.initialY = 0.0;
    config.dt = 0.1;
    config.steps = 3;

    SimulationResult result = runSimulation(model, pid, config);

    const std::string testFile = "test_output.csv";
    bool exported = exportSimulationCSV(result, testFile);
    EXPECT_TRUE(exported);

    // Проверяем содержимое файла
    std::ifstream file(testFile);
    ASSERT_TRUE(file.is_open());

    std::string header;
    std::getline(file, header);
    EXPECT_EQ(header, "Step,Time,Setpoint,Output,Error,Control");

    int lineCount = 0;
    std::string line;
    while (std::getline(file, line))
    {
        if (!line.empty())
            lineCount++;
    }
    file.close();

    // 4 строки: начальная + 3 шага
    EXPECT_EQ(lineCount, 4);

    // Удаляем тестовый файл
    std::remove(testFile.c_str());
}