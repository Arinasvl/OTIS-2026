#include <gtest/gtest.h>
#include <stdexcept>
#include <cmath>
#include "pid_controller.hpp"

// ============================================================
// ТЕСТ 1. Проверка конструктора и геттеров
// ============================================================
TEST(PIDControllerTest, ConstructorSetsCoefficients)
{
    PIDController pid(2.5, 1.5, 0.5, -10.0, 10.0);

    EXPECT_DOUBLE_EQ(pid.getKp(), 2.5);
    EXPECT_DOUBLE_EQ(pid.getKi(), 1.5);
    EXPECT_DOUBLE_EQ(pid.getKd(), 0.5);
    EXPECT_DOUBLE_EQ(pid.getOutput(), 0.0);
}

// ============================================================
// ТЕСТ 2. Конструктор выбрасывает исключение при minOutput >= maxOutput
// ============================================================
TEST(PIDControllerTest, InvalidLimitsThrowException)
{
    EXPECT_THROW(
        PIDController(1.0, 0.0, 0.0, 5.0, 5.0),
        std::invalid_argument
    );
    EXPECT_THROW(
        PIDController(1.0, 0.0, 0.0, 10.0, -10.0),
        std::invalid_argument
    );
}

// ============================================================
// ТЕСТ 3. Нулевой шаг времени вызывает исключение
// ============================================================
TEST(PIDControllerTest, ZeroTimeStepThrowsException)
{
    PIDController pid(1.0, 0.0, 0.0, -100.0, 100.0);
    EXPECT_THROW(pid.calculate(1.0, 0.0), std::invalid_argument);
    EXPECT_THROW(pid.calculate(1.0, -0.1), std::invalid_argument);
}

// ============================================================
// ТЕСТ 4. Только P-составляющая: выход = Kp * ошибка
// ============================================================
TEST(PIDControllerTest, ProportionalOnlyResponse)
{
    PIDController pid(2.0, 0.0, 0.0, -100.0, 100.0);

    // Первый шаг: previousError = 0, error = 5
    // deltaP = 2.0 * (5 - 0) = 10
    double output = pid.calculate(5.0, 0.1);
    EXPECT_DOUBLE_EQ(output, 10.0);
}

// ============================================================
// ТЕСТ 5. Только I-составляющая: выход накапливается
// ============================================================
TEST(PIDControllerTest, IntegralAccumulates)
{
    PIDController pid(0.0, 1.0, 0.0, -100.0, 100.0);

    // Шаг 1: deltaI = 1.0 * 5.0 * 0.1 = 0.5, output = 0 + 0.5 = 0.5
    double first = pid.calculate(5.0, 0.1);
    EXPECT_DOUBLE_EQ(first, 0.5);

    // Шаг 2: deltaI = 0.5, output = 0.5 + 0.5 = 1.0
    double second = pid.calculate(5.0, 0.1);
    EXPECT_DOUBLE_EQ(second, 1.0);

    // Шаг 3: output = 1.5
    double third = pid.calculate(5.0, 0.1);
    EXPECT_DOUBLE_EQ(third, 1.5);
}

// ============================================================
// ТЕСТ 6. Только D-составляющая: реагирует на изменение ошибки
// ============================================================
TEST(PIDControllerTest, DerivativeReactsToErrorChange)
{
    PIDController pid(0.0, 0.0, 1.0, -1000.0, 1000.0);

    // Шаг 1: deltaD = (1.0/0.1) * (5 - 0 + 0) = 50
    double first = pid.calculate(5.0, 0.1);
    EXPECT_DOUBLE_EQ(first, 50.0);

    // Шаг 2: deltaD = (1.0/0.1) * (5 - 2*5 + 0) = -50
    // output = 50 + (-50) = 0
    double second = pid.calculate(5.0, 0.1);
    EXPECT_DOUBLE_EQ(second, 0.0);
}

// ============================================================
// ТЕСТ 7. Ограничение управляющего сигнала сверху
// ============================================================
TEST(PIDControllerTest, OutputDoesNotExceedMaximum)
{
    PIDController pid(100.0, 0.0, 0.0, -5.0, 5.0);

    // deltaP = 100 * 10 = 1000, но ограничен maxOutput = 5
    double output = pid.calculate(10.0, 0.1);
    EXPECT_LE(output, 5.0);
    EXPECT_DOUBLE_EQ(output, 5.0);
}

// ============================================================
// ТЕСТ 8. Ограничение управляющего сигнала снизу
// ============================================================
TEST(PIDControllerTest, OutputDoesNotGoBelowMinimum)
{
    PIDController pid(100.0, 0.0, 0.0, -5.0, 5.0);

    double output = pid.calculate(-10.0, 0.1);
    EXPECT_GE(output, -5.0);
    EXPECT_DOUBLE_EQ(output, -5.0);
}

// ============================================================
// ТЕСТ 9. Anti-windup: интеграл не накапливается при насыщении
// ============================================================
TEST(PIDControllerTest, AntiWindupPreventsIntegralGrowth)
{
    PIDController pid(0.0, 10.0, 0.0, -5.0, 5.0);

    // deltaI = 10 * 10 * 0.1 = 10 > maxOutput
    // Anti-windup обнуляет deltaI → output = 0
    double first = pid.calculate(10.0, 0.1);
    EXPECT_DOUBLE_EQ(first, 0.0);

    // Даже после нескольких шаков интеграл не должен "разгоняться"
    double second = pid.calculate(10.0, 0.1);
    EXPECT_DOUBLE_EQ(second, 0.0);
}

// ============================================================
// ТЕСТ 10. Reset очищает историю ошибок
// ============================================================
TEST(PIDControllerTest, ResetClearsPreviousErrors)
{
    PIDController pid(1.0, 0.0, 0.0, -100.0, 100.0);

    pid.calculate(2.0, 0.1);
    pid.calculate(3.0, 0.1);
    pid.reset();

    // После reset previousError = 0, поэтому deltaP = 1.0 * (3 - 0) = 3
    double output = pid.calculate(3.0, 0.1);
    EXPECT_DOUBLE_EQ(output, 3.0);
}

// ============================================================
// ТЕСТ 11. Reset с начальным управляющим воздействием
// ============================================================
TEST(PIDControllerTest, ResetWithInitialOutput)
{
    PIDController pid(1.0, 0.0, 0.0, -100.0, 100.0);

    pid.reset(5.0);

    // previousOutput = 5, deltaP = 0, deltaI = 0, deltaD = 0
    double output = pid.calculate(0.0, 0.1);
    EXPECT_DOUBLE_EQ(output, 5.0);
}

// ============================================================
// ТЕСТ 12. Reset ограничивает initialOutput допустимым диапазоном
// ============================================================
TEST(PIDControllerTest, ResetClampsInitialOutput)
{
    PIDController pid(1.0, 0.0, 0.0, -5.0, 5.0);

    pid.reset(100.0);
    EXPECT_DOUBLE_EQ(pid.getOutput(), 5.0);

    pid.reset(-100.0);
    EXPECT_DOUBLE_EQ(pid.getOutput(), -5.0);
}