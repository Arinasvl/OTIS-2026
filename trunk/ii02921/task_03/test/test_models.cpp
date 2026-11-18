#include <gtest/gtest.h>
#include <cmath>
#include <string>
#include "models.hpp"

// ============================================================
// ТЕСТ 1. Модель 1.10: линейная с задержкой
// ============================================================
TEST(ModelsTest, LinearModelCalculatesCorrectly)
{
    Model1_10 model(0.5, 2.0, 3.0);

    // y(t+1) = 0.5*4 + 2*1 + 3*0 = 4.0
    double first = model.calculateNext(4.0, 1.0, 0.1);
    EXPECT_DOUBLE_EQ(first, 4.0);

    // y(t+1) = 0.5*4 + 2*2 + 3*0 = 6.0
    double second = model.calculateNext(4.0, 2.0, 0.1);
    EXPECT_DOUBLE_EQ(second, 6.0);

    // y(t+1) = 0.5*4 + 2*0 + 3*1 = 5.0 (u(t-2)=1)
    double third = model.calculateNext(4.0, 0.0, 0.1);
    EXPECT_DOUBLE_EQ(third, 5.0);
}

// ============================================================
// ТЕСТ 2. Модель 1.10: эффект задержки на 2 шага
// ============================================================
TEST(ModelsTest, LinearModelShowsDelayEffect)
{
    Model1_10 model(0.0, 1.0, 1.0);

    // Шаг 1: u=5, u(t-2)=0 → y = 0 + 5 + 0 = 5
    EXPECT_DOUBLE_EQ(model.calculateNext(0.0, 5.0, 0.1), 5.0);

    // Шаг 2: u=0, u(t-2)=0 → y = 0 + 0 + 0 = 0
    EXPECT_DOUBLE_EQ(model.calculateNext(0.0, 0.0, 0.1), 0.0);

    // Шаг 3: u=0, u(t-2)=5 → y = 0 + 0 + 5 = 5 (задержка видна!)
    EXPECT_DOUBLE_EQ(model.calculateNext(0.0, 0.0, 0.1), 5.0);
}

// ============================================================
// ТЕСТ 3. Модель 1.10: reset очищает историю
// ============================================================
TEST(ModelsTest, LinearModelResetClearsInputHistory)
{
    Model1_10 model(0.0, 1.0, 1.0);

    model.calculateNext(0.0, 5.0, 0.1);
    model.calculateNext(0.0, 7.0, 0.1);
    model.reset();

    // После reset u(t-2) = 0, поэтому y = 0 + 1*0 + 1*0 = 0
    EXPECT_DOUBLE_EQ(model.calculateNext(0.0, 0.0, 0.1), 0.0);
}

// ============================================================
// ТЕСТ 4. Модель 2.4: нелинейная с логарифмом
// ============================================================
TEST(ModelsTest, NonlinearModelCalculatesCorrectly)
{
    Model2_4 model(2.0, 3.0);

    // y(t+1) = 2*1*cos(1) + 3*ln(1+|1|) = 2*cos(1) + 3*ln(2)
    double result = model.calculateNext(1.0, 1.0, 0.1);
    double expected = 2.0 * cos(1.0) + 3.0 * log(2.0);
    EXPECT_NEAR(result, expected, 1e-9);
}

// ============================================================
// ТЕСТ 5. Модель 2.4: отрицательное u работает через модуль
// ============================================================
TEST(ModelsTest, NonlinearModelHandlesNegativeInput)
{
    Model2_4 model(0.0, 1.0);

    // y = 0 + 1*ln(1+|-5|) = ln(6)
    double result = model.calculateNext(0.0, -5.0, 0.1);
    EXPECT_NEAR(result, log(6.0), 1e-9);

    // Положительное u даёт тот же результат
    model.reset();
    double sameResult = model.calculateNext(0.0, 5.0, 0.1);
    EXPECT_NEAR(sameResult, log(6.0), 1e-9);
}

// ============================================================
// ТЕСТ 6. Модель 3.8: метод Эйлера
// ============================================================
TEST(ModelsTest, DifferentialModelUsesEulerMethod)
{
    Model3_8 model(1.0, 1.0, 1.0);

    // dy/dt = -1*2 - 1*4 + 1*3 = -3
    // y(t+1) = 2 + 0.1*(-3) = 1.7
    double result = model.calculateNext(2.0, 3.0, 0.1);
    EXPECT_DOUBLE_EQ(result, 1.7);
}

// ============================================================
// ТЕСТ 7. Модель 3.8: квадратичный член замедляет рост
// ============================================================
TEST(ModelsTest, DifferentialModelQuadraticTermSlowsGrowth)
{
    // Без квадратичного члена
    Model3_8 withoutQuad(1.0, 0.0, 1.0);
    double y1 = withoutQuad.calculateNext(2.0, 3.0, 0.1);

    // С квадратичным членом
    Model3_8 withQuad(1.0, 1.0, 1.0);
    double y2 = withQuad.calculateNext(2.0, 3.0, 0.1);

    // Квадратичный член уменьшает dy/dt, поэтому y2 < y1
    EXPECT_LT(y2, y1);
}

// ============================================================
// ТЕСТ 8. Все модели возвращают корректные имена
// ============================================================
TEST(ModelsTest, AllModelsReturnCorrectNames)
{
    Model1_10 m1(0.5, 1.0, 0.2);
    Model2_4  m2(1.0, 1.0);
    Model3_8  m3(1.0, 0.0, 1.0);

    EXPECT_EQ(m1.getName(), "Model 1.10 - Distributed Control Window Model");
    EXPECT_EQ(m2.getName(), "Model 2.4 - Logarithmic Multi-Variable Interaction");
    EXPECT_EQ(m3.getName(), "Model 3.8 - Combined Linear-Quadratic Decay");
}

// ============================================================
// ТЕСТ 9. Полиморфизм: вызов через указатель на базовый класс
// ============================================================
TEST(ModelsTest, PolymorphicCallThroughBasePointer)
{
    // Создаём модель через unique_ptr базового класса
    std::unique_ptr<Model> model = std::make_unique<Model3_8>(1.0, 0.0, 2.0);

    // Вызываем calculateNext через базовый указатель
    double result = model->calculateNext(4.0, 3.0, 0.1);

    // dy/dt = -1*4 - 0 + 2*3 = 2, y(t+1) = 4 + 0.1*2 = 4.2
    EXPECT_NEAR(result, 4.2, 1e-9);
    EXPECT_EQ(model->getName(), "Model 3.8 - Combined Linear-Quadratic Decay");
}