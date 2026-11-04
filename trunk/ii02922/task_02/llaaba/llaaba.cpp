#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <clocale>
#include <cstdlib>

using namespace std;

// ==========================================
// 1. БАЗОВЫЙ КЛАСС И МОДЕЛИ
// ==========================================

class Model {
public:
    virtual double calculateNext(double y, double u, double dt) = 0;
    virtual string getName() const = 0;
    virtual void reset() = 0;
    virtual bool isStable(double dt = 0.1) const { return true; }
    virtual ~Model() {}
};

// 1. Линейная модель (Model 1.10)
class Model1_10 : public Model {
private:
    double a1;
    double b1;
    double b2;
    double u_t_minus_1;
    double u_t_minus_2;

public:
    Model1_10(double a1, double b1, double b2) : a1(a1), b1(b1), b2(b2) {
        reset();
    }

    double calculateNext(double y, double u, double dt) override {
        (void)dt;
        double nextY = a1 * y + b1 * u + b2 * u_t_minus_2;
        u_t_minus_2 = u_t_minus_1;
        u_t_minus_1 = u;
        return nextY;
    }

    string getName() const override {
        return "Model 1.10 - Distributed Control Window Model";
    }

    void reset() override {
        u_t_minus_1 = 0.0;
        u_t_minus_2 = 0.0;
    }

    // Критерий устойчивости: |a1| < 1
    bool isStable(double dt) const override {
        (void)dt;
        return std::fabs(a1) < 1.0;
    }
};

// 2. Нелинейная модель (Model 2.4)
class Model2_4 : public Model {
private:
    double a;
    double b;

public:
    Model2_4(double a, double b) : a(a), b(b) {}

    double calculateNext(double y, double u, double dt) override {
        (void)dt;
        return a * y * std::cos(y) + b * std::log(1.0 + std::fabs(u));
    }

    string getName() const override {
        return "Model 2.4 - Logarithmic Multi-Variable Interaction";
    }

    void reset() override {}

    // Критерий устойчивости (по линеаризации в нуле): |a| < 1
    bool isStable(double dt) const override {
        (void)dt;
        return std::fabs(a) < 1.0;
    }
};

// 3. Модель дифференциального уравнения (Model 3.8)
class Model3_8 : public Model {
private:
    double a1;
    double a2;
    double b;

public:
    Model3_8(double a1, double a2, double b) : a1(a1), a2(a2), b(b) {}

    double calculateNext(double y, double u, double dt) override {
        double dydt = -a1 * y - a2 * y * y + b * u;
        return y + dt * dydt;
    }

    string getName() const override {
        return "Model 3.8 - Combined Linear-Quadratic Decay";
    }

    void reset() override {}

    // Критерий устойчивости для метода Эйлера: 0 < a1 * dt < 2
    bool isStable(double dt) const override {
        double lambda = a1 * dt;
        return (lambda > 0.0 && lambda < 2.0);
    }
};


// ==========================================
// 2. ГЕНЕРАТОР ВХОДНЫХ ВОЗДЕЙСТВИЙ
// ==========================================

double getInput(int type, int t) {
    if (type == 1) {
        return 1.0; // Ступенчатое
    }
    if (type == 2) {
        return (t == 0) ? 1.0 : 0.0; // Импульсное
    }
    if (type == 3) {
        return std::sin(t); // Гармоническое
    }
    return 0.0;
}


// ==========================================
// 3. ЛОГИКА СИМУЛЯЦИИ
// ==========================================

void simulate(
    Model& model,
    int inputType,
    int n,
    double y0,
    double dt,
    const string& fileName
) {
    cout << endl;
    cout << "==============================================" << endl;
    cout << model.getName() << endl;
    cout << "==============================================" << endl;

    // Проверка устойчивости (Требование п. 2 задания)
    if (!model.isStable(dt)) {
        cout << "[ПРЕДУПРЕЖДЕНИЕ] Система является неустойчивой! Возможен расходящийся процесс." << endl;
    }
    else {
        cout << "[СТАТУС] Анализ устойчивости пройден: система стабильна." << endl;
    }

    cout << endl;
    cout << "t\t" << "u\t\t" << "y" << endl;
    cout << "----------------------------------------------" << endl;

    model.reset();
    double y = y0;

    ofstream file(fileName);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла!" << endl;
        return;
    }

    file << "t,u,y\n";

    for (int t = 0; t < n; t++) {
        double u = getInput(inputType, t);
        double nextY = model.calculateNext(y, u, dt);

        cout << t << "\t" << u << "\t\t" << nextY << endl;
        file << t << "," << u << "," << nextY << "\n";

        y = nextY;
    }

    file.close();
    cout << endl << "Результаты сохранены в файл: " << fileName << endl;
}


// ==========================================
// 4. ТОЧКА ВХОДА (MAIN)
// ==========================================

int main() {
    setlocale(LC_ALL, "");

    cout << "==============================================" << endl;
    cout << "        ПРОГРАММА МОДЕЛИРОВАНИЯ (TASK 2)      " << endl;
    cout << "==============================================" << endl;

    int n;
    cout << "\nВведите количество шагов моделирования n: ";
    cin >> n;
    if (n <= 0) {
        cout << "Количество шагов должно быть больше 0." << endl;
        return 1;
    }

    int modelChoice;
    cout << "\nВыберите модель:\n1 - Model 1.10\n2 - Model 2.4\n3 - Model 3.8\nВаш выбор: ";
    cin >> modelChoice;
    if (modelChoice <= 0) {
        cout << "Выберите число 1,2 или 3." << endl;
        return 1;
    }

    int inputChoice;
    cout << "\nВыберите входное воздействие:\n1 - Ступенчатое\n2 - Импульсное\n3 - Гармоническое\nВаш выбор: ";
    cin >> inputChoice;
    if (inputChoice <= 0) {
        cout << "Числа могут быть только 1,2 или 3." << endl;
        return 1;
    }

    if (inputChoice < 1 || inputChoice > 3) {
        cout << "Неверный тип воздействия." << endl;
        return 1;
    }

    double y0, dt;
    cout << "\nВведите начальное значение y: ";
    cin >> y0;
    cout << "Введите шаг времени dt: ";
    cin >> dt;

    if (dt <= 0) {
        cout << "dt должен быть больше 0." << endl;
        return 1;
    }

    Model* model = nullptr;
    string fileName;

    if (modelChoice == 1) {
        double a1, b1, b2;
        cout << "\nВведите коэффициенты Model 1.10 (a1, b1, b2):\n";
        cin >> a1 >> b1 >> b2;
        model = new Model1_10(a1, b1, b2);
        fileName = "model1_10.csv";
    }
    else if (modelChoice == 2) {
        double a, b;
        cout << "\nВведите коэффициенты Model 2.4 (a, b):\n";
        cin >> a >> b;
        model = new Model2_4(a, b);
        fileName = "model2_4.csv";
    }
    else if (modelChoice == 3) {
        double a1, a2, b;
        cout << "\nВведите коэффициенты Model 3.8 (a1, a2, b):\n";
        cin >> a1 >> a2 >> b;
        model = new Model3_8(a1, a2, b);
        fileName = "model3_8.csv";
    }
    else {
        cout << "Неверный номер модели." << endl;
        return 1;
    }

    simulate(*model, inputChoice, n, y0, dt, fileName);

    delete model;
    cout << "\n==============================================" << endl;
    cout << "          Моделирование завершено.            " << endl;
    cout << "==============================================" << endl;

    return 0;
}