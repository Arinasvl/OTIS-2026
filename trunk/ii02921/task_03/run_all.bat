@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo =======================================
echo  [ШАГ 1/4] Очистка и сборка проекта...
echo =======================================
if exist build (
    echo Удаляю старую папку build...
    rmdir /s /q build
)

cmake -S . -B build -G "Visual Studio 18 2026" -A x64
if !ERRORLEVEL! NEQ 0 (
    echo [ОШИБКА] CMake конфигурация не удалась!
    pause
    exit /b !ERRORLEVEL!
)

cmake --build build --config Debug
if !ERRORLEVEL! NEQ 0 (
    echo [ОШИБКА] Сборка не удалась!
    pause
    exit /b !ERRORLEVEL!
)
echo [OK] Сборка завершена.

echo.
echo =======================================
echo  [ШАГ 2/4] Запуск тестов...
echo =======================================
ctest --test-dir build -C Debug --output-on-failure
if !ERRORLEVEL! NEQ 0 (
    echo [ОШИБКА] Тесты не прошли!
    pause
    exit /b !ERRORLEVEL!
)
echo [OK] Все тесты пройдены.

echo.
echo =======================================
echo  [ШАГ 3/4] Запуск программы моделирования...
echo =======================================
echo.
echo Сейчас запустится программа task_03.exe
echo Введите параметры в консоли ниже:
echo   - Выберите модель (1, 2 или 3)
echo   - Введите коэффициенты
echo   - Настройте PID-регулятор (Kp, Ki, Kd)
echo   - Выберите режим симуляции (A или B)
echo   - Введите имя CSV-файла для сохранения
echo.
echo После сохранения CSV автоматически откроется график!
echo.
pause

build\Debug\task_03.exe

if !ERRORLEVEL! NEQ 0 (
    echo [ОШИБКА] Программа завершилась с ошибкой!
    pause
    exit /b !ERRORLEVEL!
)

echo.
echo [OK] Программа завершена успешно.

echo.
echo =======================================
echo  [ШАГ 4/4] Генерация Doxygen-документации...
echo =======================================
if exist Doxyfile (
    echo Генерирую HTML-документацию в doc\html\...
    doxygen Doxyfile
    if !ERRORLEVEL! NEQ 0 (
        echo [ОШИБКА] Doxygen не удался!
        pause
        exit /b !ERRORLEVEL!
    )
    echo [OK] Документация создана в doc\html\
) else (
    echo [ОШИБКА] Файл Doxyfile не найден!
    pause
    exit /b 1
)

echo.
echo =======================================
echo  [УСПЕХ] Все шаги выполнены!
echo =======================================
echo.
echo Результаты:
echo   - Графики сохранены рядом с CSV-файлами (*.png)
echo   - Документация: doc\html\index.html
echo.
pause