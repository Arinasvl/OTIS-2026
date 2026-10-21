#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <clocale>
#include <memory> // Добавлено для std::unique_ptr

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

    // Виртуальный деструктор (используем = default по стандарту C++11)
    virtual ~Model() = default;
};


// ============================================================
// МОДЕЛЬ 1.10
// y(t+1) = a1*y(t) + b1*u(t) + b2*u(t-2)
// ============================================================

class Model1_10 : public Model
{
private:
    // Префикс m_ добавлен для избежания затенения (shadowing) параметров
    double m_a1;
    double m_b1;
    double m_b2;
    double m_u_t_minus_1;
    double m_u_t_minus_2;

public:
    // Инициализация через список инициализации (best practice)
    Model1_10(double a1, double b1, double b2)
        : m_a1(a1), m_b1(b1), m_b2(b2), m_u_t_minus_1(0.0), m_u_t_minus_2(0.0)
    {
    }

    double calculateNext(double y, double u, double dt) override
    {
        (void)dt; // dt для этой модели не используется

        double nextY = m_a1 * y + m_b1 * u + m_b2 * m_u_t_minus_2;

        m_u_t_minus_2 = m_u_t_minus_1;
        m_u_t_minus_1 = u;

        return nextY;
    }

    string getName() const override
    {
        return "Model 1.10 - Distributed Control Window Model";
    }

    void reset() override
    {
        m_u_t_minus_1 = 0.0;
        m_u_t_minus_2 = 0.0;
    }
};


// ============================================================
// МОДЕЛЬ 2.4
// y(t+1) = a*y(t)*cos(y(t)) + b*ln(1+|u(t)|)
// ============================================================

class Model2_4 : public Model
{
private:
    double m_a;
    double m_b;

public:
    Model2_4(double a, double b) : m_a(a), m_b(b) {}

    double calculateNext(double y, double u, double dt) override
    {
        (void)dt;
        return m_a * y * cos(y) + m_b * log(1.0 + fabs(u));
    }

    string getName() const override
    {
        return "Model 2.4 - Logarithmic Multi-Variable Interaction";
    }

    void reset() override
    {
        // Модель 2.4 не имеет накапливаемого внутреннего состояния, 
        // зависящего от предыдущих шагов, поэтому сброс не требуется.
    }
};


// ============================================================
// МОДЕЛЬ 3.8
// dy/dt = -a1*y - a2*y^2 + b*u (метод Эйлера)
// ============================================================

class Model3_8 : public Model
{
private:
    double m_a1;
    double m_a2;
    double m_b;

public:
    Model3_8(double a1, double a2, double b) : m_a1(a1), m_a2(a2), m_b(b) {}

    double calculateNext(double y, double u, double dt) override
    {
        double dydt = -m_a1 * y - m_a2 * y * y + m_b * u;
        return y + dt * dydt;
    }

    string getName() const override
    {
        return "Model 3.8 - Combined Linear-Quadratic Decay";
    }

    void reset() override
    {
        // Модель 3.8 вычисляет следующее состояние явно из текущего y и u.
        // Внутреннее состояние не накапливается, поэтому сброс не требуется.
    }
};


// ============================================================
// ФУНКЦИЯ ФОРМИРОВАНИЯ ВХОДНОГО СИГНАЛА
// ============================================================

double getInput(int type, int t)
{
    if (type == 1) return 1.0; // Ступенчатый
    
    if (type == 2) {           // Импульсный
        return (t == 0) ? 1.0 : 0.0;
    }
    
    if (type == 3) return sin(t); // Гармонический

    return 0.0;
}


// ============================================================
// ФУНКЦИЯ МОДЕЛИРОВАНИЯ
// ============================================================

void simulate(Model& model, int inputType, int n, double y0, double dt, const string& fileName)
{
    cout << "\n==============================================\n";
    cout << model.getName() << "\n";
    cout << "==============================================\n\n";

    cout << "t\tu\t\ty\n";
    cout << "----------------------------------------------\n";

    model.reset();
    double y = y0;

    ofstream file(fileName);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла!\n";
        return;
    }

    file << "t,u,y\n";

    for (int t = 0; t < n; t++) {
        double u = getInput(inputType, t);
        double nextY = model.calculateNext(y, u, dt);

        cout << t << "\t" << u << "\t\t" << nextY << "\n";
        file << t << "," << u << "," << nextY << "\n";

        y = nextY;
    }

    file.close();
    cout << "\nРезультаты сохранены в файл: " << fileName << "\n";
}


// ============================================================
// ГЛАВНАЯ ФУНКЦИЯ
// ============================================================

int main()
{
    setlocale(LC_ALL, "");

    cout << "==============================================\n";
    cout << "       ПРОГРАММА МОДЕЛИРОВАНИЯ\n";
    cout << "==============================================\n";

    int n;
    cout << "\nВведите количество шагов моделирования n: ";
    cin >> n;
    if (n <= 0) {
        cout << "Количество шагов должно быть больше 0.\n";
        return 1;
    }

    int modelChoice;
    cout << "\nВыберите модель:\n1 - Model 1.10\n2 - Model 2.4\n3 - Model 3.8\nВаш выбор: ";
    cin >> modelChoice;

    int inputChoice;
    cout << "\nВыберите входное воздействие:\n1 - Ступенчатое\n2 - Импульсное\n3 - Гармоническое\nВаш выбор: ";
    cin >> inputChoice;
    if (inputChoice < 1 || inputChoice > 3) {
        cout << "Неверный тип воздействия.\n";
        return 1;
    }

    double y0, dt;
    cout << "\nВведите начальное значение y: ";
    cin >> y0;
    cout << "Введите шаг времени dt: ";
    cin >> dt;
    if (dt <= 0) {
        cout << "dt должен быть больше 0.\n";
        return 1;
    }

    // ИСПОЛЬЗОВАНИЕ УМНОГО УКАЗАТЕЛЯ (std::unique_ptr)
    // Это автоматически управляет памятью и избавляет от необходимости писать delete
    unique_ptr<Model> model = nullptr;
    string fileName;

    if (modelChoice == 1) {
        double a1, b1, b2;
        cout << "\nВведите коэффициенты Model 1.10:\na1 = "; cin >> a1;
        cout << "b1 = "; cin >> b1;
        cout << "b2 = "; cin >> b2;
        
        model = make_unique<Model1_10>(a1, b1, b2);
        fileName = "model1_10.csv";
    }
    else if (modelChoice == 2) {
        double a, b;
        cout << "\nВведите коэффициенты Model 2.4:\na = "; cin >> a;
        cout << "b = "; cin >> b;
        
        model = make_unique<Model2_4>(a, b);
        fileName = "model2_4.csv";
    }
    else if (modelChoice == 3) {
        double a1, a2, b;
        cout << "\nВведите коэффициенты Model 3.8:\na1 = "; cin >> a1;
        cout << "a2 = "; cin >> a2;
        cout << "b = "; cin >> b;
        
        model = make_unique<Model3_8>(a1, a2, b);
        fileName = "model3_8.csv";
    }
    else {
        cout << "Неверный номер модели.\n";
        return 1;
    }

    // Запускаем моделирование
    if (model) {
        simulate(*model, inputChoice, n, y0, dt, fileName);
    }

    // delete model; <-- УДАЛЕНО. unique_ptr освободит память автоматически!

    cout << "\n==============================================\n";
    cout << "          Моделирование завершено.\n";
    cout << "==============================================\n";

    return 0;
}
