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

    double y0;
    cout << "\nВведите начальное значение y: ";
    cin >> y0;

    double dt;
    cout << "Введите шаг времени dt: ";
    cin >> dt;

    if (dt <= 0) {
        cout << "dt должен быть больше 0.\n";
        return 1;
    }

    unique_ptr<Model> model = nullptr;
    string fileName;

    if (modelChoice == 1) {
        // ИСПРАВЛЕНО: каждая переменная в отдельной строке
        double a1;
        double b1;
        double b2;

        cout << "\nВведите коэффициенты Model 1.10:\na1 = "; cin >> a1;
        cout << "b1 = "; cin >> b1;
        cout << "b2 = "; cin >> b2;

        model = make_unique<Model1_10>(a1, b1, b2);
        fileName = "model1_10.csv";
    }
    else if (modelChoice == 2) {
        // ИСПРАВЛЕНО: каждая переменная в отдельной строке
        double a;
        double b;

        cout << "\nВведите коэффициенты Model 2.4:\na = "; cin >> a;
        cout << "b = "; cin >> b;

        model = make_unique<Model2_4>(a, b);
        fileName = "model2_4.csv";
    }
    else if (modelChoice == 3) {
        // ИСПРАВЛЕНО: каждая переменная в отдельной строке
        double a1;
        double a2;
        double b;

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

    if (model) {
        simulate(*model, inputChoice, n, y0, dt, fileName);
        system("python plot.py");
    }

    cout << "\n==============================================\n";
    cout << "          Моделирование завершено.\n";
    cout << "==============================================\n";

    return 0;
}