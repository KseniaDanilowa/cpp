// Подключаем необходимые заголовочные файлы
#include <iostream>    // Для ввода-вывода
#include <future>      // Для асинхронных вычислений (std::async, std::future)
#include <random>      // Для генерации случайных чисел
#include <vector>      // Для векторов
#include <chrono>      // Для измерения времени
#include <cmath>       // Для математических функций (std::abs)
#include <clocale>     // Для функций локализации (setlocale) - нужен для русских символов

// Класс для вычисления числа π методом Монте-Карло
class PiCalculator {
private:  // Приватные члены
    std::random_device rd;    // Источник случайных чисел от ОС
    std::mt19937 gen;         // Генератор псевдослучайных чисел Мерсенна-Твистера

public:   // Публичные методы
    // Конструктор класса - вызывается при создании объекта
    // Инициализируем генератор с seed от random_device
    PiCalculator() : gen(rd()) {}

    // Метод для вычисления части числа π
    // Использует метод Монте-Карло: генерирует случайные точки и считает сколько попало в четверть круга
    // int total_points - количество точек для генерации
    double calculatePiPortion(int total_points) {
        // Создаем равномерное распределение от 0.0 до 1.0
        std::uniform_real_distribution<> dis(0.0, 1.0);

        // Счетчик точек, попавших внутрь четверти круга единичного радиуса
        int points_inside_circle = 0;

        // Генерируем указанное количество случайных точек
        for (int i = 0; i < total_points; ++i) {
            // Генерируем случайные координаты x и y в диапазоне [0, 1)
            double x = dis(gen);  // Координата X
            double y = dis(gen);  // Координата Y

            // Проверяем, находится ли точка (x,y) внутри четверти круга
            // Уравнение круга: x² + y² ≤ 1 (для четверти круга с центром в (0,0) и радиусом 1)
            if (x * x + y * y <= 1.0) {
                // Точка внутри круга - увеличиваем счетчик
                points_inside_circle++;
            }
        }

        // Вычисляем приближение π по формуле метода Монте-Карло
        // Площадь четверти круга = (π * r²) / 4 = π/4
        // Отношение точек внутри круга к общему количеству ≈ π/4
        // Поэтому π ≈ 4 * (точки_внутри / общее_количество)
        return 4.0 * points_inside_circle / total_points;
    }

    // Метод для параллельного вычисления π с использованием нескольких задач
    // int total_points - общее количество точек для всех задач
    // int num_tasks - количество параллельных задач
    double calculatePiParallel(int total_points, int num_tasks) {
        // Вектор для хранения объектов future (обещаний результатов)
        std::vector<std::future<double>> futures;

        // Вычисляем количество точек для каждой задачи
        int points_per_task = total_points / num_tasks;

        // Создаем асинхронные задачи
        // Цикл по количеству задач
        for (int i = 0; i < num_tasks; ++i) {
            // std::async запускает функцию асинхронно
            // std::launch::async - политика запуска: создать новый поток
            // &PiCalculator::calculatePiPortion - указатель на метод класса
            // this - указатель на текущий объект
            // points_per_task - параметр метода
            futures.push_back(std::async(std::launch::async,
                &PiCalculator::calculatePiPortion, this, points_per_task));
        }

        // Собираем результаты от всех задач
        double total_pi = 0.0;  // Сумма всех приближений π

        // Проходим по всем future объектам
        for (auto& future : futures) {
            // future.get() ждет завершения задачи и возвращает результат
            // Добавляем результат к общей сумме
            total_pi += future.get();
        }

        // Возвращаем среднее значение всех приближений
        return total_pi / num_tasks;
    }
};


int main() {
    
    setlocale(LC_ALL, "Russian");


    // Создаем объект калькулятора
    PiCalculator calculator;

    // Замеряем время начала вычислений
    auto start = std::chrono::high_resolution_clock::now();

    // Вызываем параллельное вычисление π
    // 1,000,000 точек всего, разделенные на 4 задачи
    double pi_parallel = calculator.calculatePiParallel(1000000, 4);

    // Замеряем время окончания
    auto end = std::chrono::high_resolution_clock::now();

    // Вычисляем продолжительность в миллисекундах
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Выводим результаты
    std::cout << "Вычисленное значение pi: " << pi_parallel << std::endl;
    std::cout << "Истинное значение pi: " << 3.141592653589793 << std::endl;

    // Вычисляем абсолютную погрешность
    // std::abs - функция модуля (абсолютного значения)
    std::cout << "Погрешность: " << std::abs(pi_parallel - 3.141592653589793) << std::endl;

    // Выводим время выполнения
    std::cout << "Время вычисления: " << duration.count() << " мс" << std::endl;

    // Успешное завершение программы
    return 0;
}
