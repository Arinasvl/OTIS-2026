#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <clocale>
#include <cstdlib>

using namespace std;

class Model
{
public:

    virtual double calculateNext(double y, double u, double dt) = 0;

    virtual string getName() const = 0;

    virtual void reset() = 0;

    virtual ~Model()
    {
    }
};

class Model1_10 : public Model
{
private:

    double a1;
    double b1;
    double b2;

    double u_t_minus_1;
    double u_t_minus_2;

public:

    Model1_10(double a1, double b1, double b2)
    {
        this->a1 = a1;
        this->b1 = b1;
        this->b2 = b2;

        u_t_minus_1 = 0.0;
        u_t_minus_2 = 0.0;
    }

    double calculateNext(double y, double u, double dt) override
    {
        (void)dt;

        double nextY =
            a1 * y
            + b1 * u
            + b2 * u_t_minus_2;

        u_t_minus_2 = u_t_minus_1;
        u_t_minus_1 = u;

        return nextY;
    }

    string getName() const override
    {
        return "Model 1.10 - Distributed Control Window Model";
    }

    void reset() override
    {
        u_t_minus_1 = 0.0;
        u_t_minus_2 = 0.0;
    }
};

class Model2_4 : public Model
{
private:

    double a;
    double b;

public:
    Model2_4(double a, double b)
    {
        this->a = a;
        this->b = b;
    }

    double calculateNext(double y, double u, double dt) override
    {
        (void)dt;

        return
            a * y * cos(y)
            + b * log(1.0 + fabs(u));
    }

    string getName() const override
    {
        return "Model 2.4 - Logarithmic Multi-Variable Interaction";
    }

    void reset() override
    {
    }
};

class Model3_8 : public Model
{
private:

    double a1;
    double a2;
    double b;

public:
    Model3_8(double a1, double a2, double b)
    {
        this->a1 = a1;
        this->a2 = a2;
        this->b = b;
    }

    double calculateNext(double y, double u, double dt) override
    {
        double dydt =
            -a1 * y
            - a2 * y * y
            + b * u;

        return y + dt * dydt;
    }

    string getName() const override
    {
        return "Model 3.8 - Combined Linear-Quadratic Decay";
    }

    void reset() override
    {
    }
};

double getInput(int type, int t)
{
    if (type == 1)
    {
        return 1.0;
    }

    if (type == 2)
    {
        if (t == 0)
        {
            return 1.0;
        }

        return 0.0;
    }

    if (type == 3)
    {
        return sin(t);
    }


    return 0.0;
}

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

    model.reset();

    double y = y0;

    ofstream file(fileName);

    if (!file.is_open())
    {
        cout << "Ошибка открытия файла!" << endl;
        return;
    }

    file << "t,u,y\n";

    for (int t = 0; t < n; t++)
    {
        double u = getInput(inputType, t);

        double nextY =
            model.calculateNext(y, u, dt);

        cout << t << "\t";
        cout << u << "\t\t";
        cout << nextY << endl;

        file << t << ",";
        file << u << ",";
        file << nextY << "\n";

        y = nextY;
    }

    file.close();


    cout << endl;
    cout << "Результаты сохранены в файл: ";
    cout << fileName << endl;
}

int main()
{
    setlocale(LC_ALL, "");


    cout << "==============================================" << endl;
    cout << "       ПРОГРАММА МОДЕЛИРОВАНИЯ" << endl;
    cout << "==============================================" << endl;

    int n;

    cout << endl;
    cout << "Введите количество шагов моделирования n: ";
    cin >> n;


    if (n <= 0)
    {
        cout << "Количество шагов должно быть больше 0." << endl;

        return 1;
    }

    int modelChoice;

    cout << endl;

    cout << "Выберите модель:" << endl;

    cout << "1 - Model 1.10" << endl;
    cout << "2 - Model 2.4" << endl;
    cout << "3 - Model 3.8" << endl;

    cout << "Ваш выбор: ";
    cin >> modelChoice;

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

    double y0;

    cout << endl;
    cout << "Введите начальное значение y: ";
    cin >> y0;

    double dt;

    cout << "Введите шаг времени dt: ";
    cin >> dt;


    if (dt <= 0)
    {
        cout << "dt должен быть больше 0." << endl;

        return 1;
    }

    Model* model = nullptr;

    string fileName;

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

    else
    {
        cout << "Неверный номер модели." << endl;

        return 1;
    }

    simulate(
        *model,
        inputChoice,
        n,
        y0,
        dt,
        fileName
    );
    system("py plot.py");

    delete model;

    cout << endl;

    cout << "==============================================" << endl;
    cout << "          Моделирование завершено." << endl;
    cout << "==============================================" << endl;

    return 0;
}