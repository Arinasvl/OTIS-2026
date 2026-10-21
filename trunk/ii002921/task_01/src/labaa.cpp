#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <clocale>

using namespace std;


// ============================================================
// БАЗОВЫЙ АБСТРАКТНЫЙ КЛАСС MODEL
// ============================================================

class Model
{
public:

    // Расчёт следующего значения y
    virtual double calculateNext(double y, double u, double dt) = 0;

    // Название модели
    virtual string getName() const = 0;

    // Сброс внутренних параметров модели
    virtual void reset() = 0;

    // Виртуальный деструктор
    virtual ~Model()
    {
    }
};


// ============================================================
// МОДЕЛЬ 1.10
//
// y(t+1) = a1*y(t) + b1*u(t) + b2*u(t-2)
// ============================================================

class Model1_10 : public Model
{
private:

    double a1;
    double b1;
    double b2;

    // Два предыдущих значения входного сигнала
    double u_t_minus_1;
    double u_t_minus_2;

public:

    // Конструктор
    Model1_10(double a1, double b1, double b2)
    {
        this->a1 = a1;
        this->b1 = b1;
        this->b2 = b2;

        u_t_minus_1 = 0.0;
        u_t_minus_2 = 0.0;
    }


    // Расчёт следующего значения
    double calculateNext(double y, double u, double dt) override
    {
        // dt для этой модели не используется
        (void)dt;

        double nextY =
            a1 * y
            + b1 * u
            + b2 * u_t_minus_2;

        // Сдвигаем предыдущие значения входа
        u_t_minus_2 = u_t_minus_1;
        u_t_minus_1 = u;

        return nextY;
    }


    // Название модели
    string getName() const override
    {
        return "Model 1.10 - Distributed Control Window Model";
    }


    // Сброс
    void reset() override
    {
        u_t_minus_1 = 0.0;
        u_t_minus_2 = 0.0;
    }
};


// ============================================================
// МОДЕЛЬ 2.4
//
// y(t+1) = a*y(t)*cos(y(t)) + b*ln(1+|u(t)|)
// ============================================================

class Model2_4 : public Model
{
private:

    double a;
    double b;

public:

    // Конструктор
    Model2_4(double a, double b)
    {
        this->a = a;
        this->b = b;
    }


    // Расчёт следующего значения
    double calculateNext(double y, double u, double dt) override
    {
        // dt для этой модели не используется
        (void)dt;

        return
            a * y * cos(y)
            + b * log(1.0 + fabs(u));
    }


    // Название модели
    string getName() const override
    {
        return "Model 2.4 - Logarithmic Multi-Variable Interaction";
    }


    // Сброс
    void reset() override
    {
    }
};


// ============================================================
// МОДЕЛЬ 3.8
//
// dy/dt = -a1*y - a2*y^2 + b*u
//
// Используется метод Эйлера:
//
// y(t+1) = y(t) + dt * dy/dt
// ============================================================

class Model3_8 : public Model
{
private:

    double a1;
    double a2;
    double b;

public:

    // Конструктор
    Model3_8(double a1, double a2, double b)
    {
        this->a1 = a1;
        this->a2 = a2;
        this->b = b;
    }


    // Расчёт следующего значения
    double calculateNext(double y, double u, double dt) override
    {
        double dydt =
            -a1 * y
            - a2 * y * y
            + b * u;

        return y + dt * dydt;
    }


    // Название модели
    string getName() const override
    {
        return "Model 3.8 - Combined Linear-Quadratic Decay";
    }


    // Сброс
    void reset() override
    {
    }
};


// ============================================================
// ФУНКЦИЯ ФОРМИРОВАНИЯ ВХОДНОГО СИГНАЛА
//
// 1 - ступенчатый
// 2 - импульсный
// 3 - гармонический
// ============================================================

double getInput(int type, int t)
{
    // Ступенчатый сигнал
    if (type == 1)
    {
        return 1.0;
    }


    // Импульсный сигнал
    if (type == 2)
    {
        if (t == 0)
        {
            return 1.0;
        }

        return 0.0;
    }


    // Гармонический сигнал
    if (type == 3)
    {
        return sin(t);
    }


    return 0.0;
}


// ============================================================
// ФУНКЦИЯ МОДЕЛИРОВАНИЯ
// ============================================================

void simulate(
    Model& model,
    int inputType,
    int n,
    double y0,
    double dt,
    const string& fileName
)
{
    cout << endl;
    cout << "==============================================" << endl;
    cout << model.getName() << endl;
    cout << "==============================================" << endl;

    cout << endl;

    cout << "t\t";
    cout << "u\t\t";
    cout << "y" << endl;

    cout << "----------------------------------------------" << endl;


    // Сбрасываем внутреннее состояние модели
    model.reset();

    // Начальное значение y
    double y = y0;


    // Открываем CSV-файл
    ofstream file(fileName);


    if (!file.is_open())
    {
        cout << "Ошибка открытия файла!" << endl;
        return;
    }


    // Заголовок CSV
    file << "t,u,y\n";


    // Основной цикл моделирования
    for (int t = 0; t < n; t++)
    {
        // Получаем входной сигнал
        double u = getInput(inputType, t);


        // Рассчитываем новое значение
        double nextY =
            model.calculateNext(y, u, dt);


        // Выводим результат на экран
        cout << t << "\t";
        cout << u << "\t\t";
        cout << nextY << endl;


        // Записываем результат в CSV
        file << t << ",";
        file << u << ",";
        file << nextY << "\n";


        // Переходим к следующему значению
        y = nextY;
    }


    // Закрываем файл
    file.close();


    cout << endl;
    cout << "Результаты сохранены в файл: ";
    cout << fileName << endl;
}


// ============================================================
// ГЛАВНАЯ ФУНКЦИЯ
// ============================================================

int main()
{
    setlocale(LC_ALL, "");


    cout << "==============================================" << endl;
    cout << "       ПРОГРАММА МОДЕЛИРОВАНИЯ" << endl;
    cout << "==============================================" << endl;


    // --------------------------------------------------------
    // Ввод количества шагов
    // --------------------------------------------------------

    int n;

    cout << endl;
    cout << "Введите количество шагов моделирования n: ";
    cin >> n;


    if (n <= 0)
    {
        cout << "Количество шагов должно быть больше 0." << endl;

        return 1;
    }


    // --------------------------------------------------------
    // Выбор модели
    // --------------------------------------------------------

    int modelChoice;

    cout << endl;

    cout << "Выберите модель:" << endl;

    cout << "1 - Model 1.10" << endl;
    cout << "2 - Model 2.4" << endl;
    cout << "3 - Model 3.8" << endl;

    cout << "Ваш выбор: ";
    cin >> modelChoice;


    // --------------------------------------------------------
    // Выбор входного сигнала
    // --------------------------------------------------------

    int inputChoice;

    cout << endl;

    cout << "Выберите входное воздействие:" << endl;

    cout << "1 - Ступенчатое" << endl;
    cout << "2 - Импульсное" << endl;
    cout << "3 - Гармоническое" << endl;

    cout << "Ваш выбор: ";
    cin >> inputChoice;


    if (inputChoice < 1 || inputChoice > 3)
    {
        cout << "Неверный тип воздействия." << endl;

        return 1;
    }


    // --------------------------------------------------------
    // Начальное значение y
    // --------------------------------------------------------

    double y0;

    cout << endl;
    cout << "Введите начальное значение y: ";
    cin >> y0;


    // --------------------------------------------------------
    // Шаг времени
    // --------------------------------------------------------

    double dt;

    cout << "Введите шаг времени dt: ";
    cin >> dt;


    if (dt <= 0)
    {
        cout << "dt должен быть больше 0." << endl;

        return 1;
    }


    // --------------------------------------------------------
    // Указатель на базовый класс
    // --------------------------------------------------------

    Model* model = nullptr;

    string fileName;


    // --------------------------------------------------------
    // Создание модели 1.10
    // --------------------------------------------------------

    if (modelChoice == 1)
    {
        double a1;
        double b1;
        double b2;


        cout << endl;
        cout << "Введите коэффициенты Model 1.10:" << endl;

        cout << "a1 = ";
        cin >> a1;

        cout << "b1 = ";
        cin >> b1;

        cout << "b2 = ";
        cin >> b2;


        model = new Model1_10(a1, b1, b2);

        fileName = "model1_10.csv";
    }


    // --------------------------------------------------------
    // Создание модели 2.4
    // --------------------------------------------------------

    else if (modelChoice == 2)
    {
        double a;
        double b;


        cout << endl;
        cout << "Введите коэффициенты Model 2.4:" << endl;

        cout << "a = ";
        cin >> a;

        cout << "b = ";
        cin >> b;


        model = new Model2_4(a, b);

        fileName = "model2_4.csv";
    }


    // --------------------------------------------------------
    // Создание модели 3.8
    // --------------------------------------------------------

    else if (modelChoice == 3)
    {
        double a1;
        double a2;
        double b;


        cout << endl;
        cout << "Введите коэффициенты Model 3.8:" << endl;

        cout << "a1 = ";
        cin >> a1;

        cout << "a2 = ";
        cin >> a2;

        cout << "b = ";
        cin >> b;


        model = new Model3_8(a1, a2, b);

        fileName = "model3_8.csv";
    }


    // --------------------------------------------------------
    // Если введён неправильный номер модели
    // --------------------------------------------------------

    else
    {
        cout << "Неверный номер модели." << endl;

        return 1;
    }


    // --------------------------------------------------------
    // Запускаем моделирование
    // --------------------------------------------------------

    simulate(
        *model,
        inputChoice,
        n,
        y0,
        dt,
        fileName
    );


    // --------------------------------------------------------
    // Освобождаем память
    // --------------------------------------------------------

    delete model;


    cout << endl;

    cout << "==============================================" << endl;
    cout << "          Моделирование завершено." << endl;
    cout << "==============================================" << endl;


    return 0;
}