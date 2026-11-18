
#include "pid_controller.hpp"

#include <algorithm>
#include <stdexcept>

/**
 * @brief Создаёт PID-регулятор и задаёт его параметры.
 */
PIDController::PIDController(
    double kp,
    double ki,
    double kd,
    double minOutput,
    double maxOutput
)
{
    if (minOutput >= maxOutput)
    {
        throw std::invalid_argument(
            "Minimum output must be less than maximum output"
        );
    }

    m_kp = kp;
    m_ki = ki;
    m_kd = kd;

    m_minOutput = minOutput;
    m_maxOutput = maxOutput;

    reset();
}


/**
 * @brief Рассчитывает управляющее воздействие.
 */
double PIDController::calculate(double error, double dt)
{
    if (dt <= 0.0)
    {
        throw std::invalid_argument(
            "Time step must be greater than zero"
        );
    }

    // Пропорциональная составляющая.
    double deltaP =
        m_kp * (error - m_previousError);

    // Интегральная составляющая.
    double deltaI =
        m_ki * error * dt;

    // Дифференциальная составляющая.
    double deltaD =
        (m_kd / dt) *
        (error - 2.0 * m_previousError + m_previousError2);

    // Предварительное новое управляющее воздействие.
    double newOutput =
        m_previousOutput + deltaP + deltaI + deltaD;

    // Антиwindup: не допускаем накопления интегрального
    // воздействия, если оно усиливает насыщение регулятора.
    if (newOutput > m_maxOutput && deltaI > 0.0)
    {
        deltaI = 0.0;
    }
    else if (newOutput < m_minOutput && deltaI < 0.0)
    {
        deltaI = 0.0;
    }

    // Пересчитываем управляющее воздействие.
    newOutput =
        m_previousOutput + deltaP + deltaI + deltaD;

    // Ограничиваем выходной сигнал.
    newOutput = std::max(m_minOutput, newOutput);
    newOutput = std::min(m_maxOutput, newOutput);

    // Обновляем историю ошибок.
    m_previousError2 = m_previousError;
    m_previousError = error;

    // Сохраняем текущее управляющее воздействие.
    m_previousOutput = newOutput;

    return newOutput;
}


/**
 * @brief Сбрасывает внутреннее состояние регулятора.
 */
void PIDController::reset(double initialOutput)
{
    m_previousError = 0.0;
    m_previousError2 = 0.0;

    // Начальное воздействие также должно находиться
    // в допустимом диапазоне.
    m_previousOutput = std::max(
        m_minOutput,
        std::min(m_maxOutput, initialOutput)
    );
}


/**
 * @brief Возвращает коэффициент P.
 */
double PIDController::getKp() const
{
    return m_kp;
}


/**
 * @brief Возвращает коэффициент I.
 */
double PIDController::getKi() const
{
    return m_ki;
}


/**
 * @brief Возвращает коэффициент D.
 */
double PIDController::getKd() const
{
    return m_kd;
}


/**
 * @brief Возвращает последнее управляющее воздействие.
 */
double PIDController::getOutput() const
{
    return m_previousOutput;
}
