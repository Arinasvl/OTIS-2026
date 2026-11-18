import csv
import sys
import os
import matplotlib.pyplot as plt


def read_csv(filename):
    """Чтение результатов симуляции из CSV-файла."""
    time = []
    setpoint = []
    output = []
    error = []
    control = []

    with open(filename, "r", newline="", encoding="utf-8-sig") as file:
        reader = csv.DictReader(file)
        required_columns = ["Time", "Setpoint", "Output", "Error", "Control"]

        if reader.fieldnames is None:
            raise ValueError("CSV-файл пустой или не содержит заголовков.")

        for column in required_columns:
            if column not in reader.fieldnames:
                raise ValueError("В CSV-файле отсутствует столбец: " + column)

        for row in reader:
            time.append(float(row["Time"]))
            setpoint.append(float(row["Setpoint"]))
            output.append(float(row["Output"]))
            error.append(float(row["Error"]))
            control.append(float(row["Control"]))

    if len(time) == 0:
        raise ValueError("В CSV-файле нет результатов симуляции.")

    return time, setpoint, output, error, control


def build_graph(filename):
    """Построение графика переходного процесса (как в примере)."""
    # ИСПРАВЛЕНИЕ: заменяем неиспользуемую переменную 'error' на '_'
    time, setpoint, output, _, control = read_csv(filename)

    # Создаём ОДИН график с тремя линиями
    fig, ax1 = plt.subplots(figsize=(12, 8))

    # Зеленая линия — уставка (setpoint)
    ax1.plot(time, setpoint, 'g--', linewidth=2, label='Setpoint (g)')

    # Синяя линия — выходная переменная (y)
    ax1.plot(time, output, 'b-', linewidth=2, label='Output (y)')

    # Красная линия — управляющее воздействие (u)
    # Масштабируем, чтобы было видно на том же графике
    ax1.plot(time, control, 'r-', linewidth=2, label='Control (u)')

    ax1.set_title('Transition Process', fontsize=14, fontweight='bold')
    ax1.set_xlabel('Time (s)', fontsize=12)
    ax1.set_ylabel('Value', fontsize=12)
    ax1.legend(fontsize=11, loc='best')
    ax1.grid(True, alpha=0.3)

    # Сохранение изображения
    folder = os.path.dirname(os.path.abspath(filename))
    name = os.path.splitext(os.path.basename(filename))[0]
    image_path = os.path.join(folder, name + '_plot.png')
    fig.savefig(image_path, dpi=150, bbox_inches='tight')
    print("График сохранен в файл:", image_path)

    # Показываем график
    plt.show()


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Использование: python plot_results.py имя_файла.csv")
        sys.exit(1)

    csv_filename = sys.argv[1]

    if not os.path.exists(csv_filename):
        print("Ошибка: CSV-файл не найден:", csv_filename)
        sys.exit(1)

    try:
        build_graph(csv_filename)
    except Exception as error:
        print("Ошибка построения графика:", error)
        sys.exit(1)