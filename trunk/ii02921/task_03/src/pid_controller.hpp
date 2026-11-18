
#pragma once

/**
 * @brief Класс цифрового PID-регулятора.
 *
 * Реализует алгоритм автоматического регулирования
 * в инкрементальной форме.
 *
 * Регулятор рассчитывает приращение управляющего воздействия
 * на основании текущей и двух предыдущих ошибок.
 */
class PIDController
{
private:

    // Коэффициенты PID-регулятора.
    double m_kp;
    double m_ki;
    double m_kd;

    // Границы допустимого управляющего воздействия.
    double m_minOutput;
    double m_maxOutput;

    // История ошибок регулирования.
    double m_previousError;
    double m_previousError2;

    // Управляющее воздействие на предыдущем шаге.
    double m_previousOutput;

public:

    /**
     * @brief Конструктор PID-регулятора.
     *
     * @param kp Коэффициент пропорционального усиления.
     * @param ki Коэффициент интегрального усиления.
     * @param kd Коэффициент дифференциального усиления.
     * @param minOutput Минимальное управляющее воздействие.
     * @param maxOutput Максимальное управляющее воздействие.
     */
    PIDController(
        double kp,
        double ki,
        double kd,
        double minOutput,
        double maxOutput
    );

    /**
     * @brief Рассчитывает новое управляющее воздействие.
     *
     * Используется инкрементальный алгоритм PID:
     *
     * deltaU = Kp*(e - e1)
     *        + Ki*e*dt
     *        + Kd/dt*(e - 2*e1 + e2)
     *
     * @param error Текущая ошибка регулирования.
     * @param dt Шаг дискретизации по времени.
     * @return Новое управляющее воздействие.
     */
    double calculate(double error, double dt);

    /**
     * @brief Сбрасывает внутреннее состояние регулятора.
     *
     * @param initialOutput Начальное управляющее воздействие.
     */
    void reset(double initialOutput = 0.0);

    /**
     * @brief Возвращает коэффициент пропорциональной составляющей.
     */
    double getKp() const;

    /**
     * @brief Возвращает коэффициент интегральной составляющей.
     */
    double getKi() const;

    /**
     * @brief Возвращает коэффициент дифференциальной составляющей.
     */
    double getKd() const;

    /**
     * @brief Возвращает последнее управляющее воздействие.
     */
    double getOutput() const;
};

