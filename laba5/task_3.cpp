// Подключаем необходимые заголовочные файлы
#include <iostream>    // Для ввода-вывода на консоль
#include <vector>      // Для работы с динамическими массивами (векторами)
#include <thread>      // Для создания и управления потоками
#include <chrono>      // Для измерения времени выполнения
#include <random>      // Для генерации случайных чисел
#include <algorithm>   // Для алгоритмов (std::min, std::max)
#include <clocale>     // Для функций локализации (setlocale) - нужен для русских символов

// Класс для обработки изображений
class ImageProcessor {
private:  // Приватные члены - доступны только внутри класса
    // Двумерный вектор: вектор строк, каждая строка - вектор пикселей
    std::vector<std::vector<int>> image;

    // Ширина и высота изображения в пикселях
    int width, height;

public:   // Публичные методы - доступны извне класса
    // Конструктор класса - вызывается при создании объекта
    // int w, int h - ширина и высота изображения
    // : width(w), height(h) - инициализация членов класса (список инициализации)
    ImageProcessor(int w, int h) : width(w), height(h) {
        // Создаем генератор случайных чисел
        std::random_device rd;                    // Источник энтропии от ОС
        std::mt19937 gen(rd());                   // Генератор Мерсенна-Твистера
        std::uniform_int_distribution<> dis(0, 255); // Равномерное распределение 0-255

        // Изменяем размер вектора: height строк, каждая шириной width
        image.resize(height, std::vector<int>(width));

        // Заполняем изображение случайными значениями яркости (0-255)
        // Внешний цикл по строкам (y - координата по вертикали)
        for (int y = 0; y < height; ++y) {
            // Внутренний цикл по столбцам (x - координата по горизонтали)
            for (int x = 0; x < width; ++x) {
                // Присваиваем пикселю случайное значение
                image[y][x] = dis(gen);
            }
        }
    }

    // Метод однопоточного применения фильтра размытия
    void applyFilterSingleThread() {
        // Создаем копию изображения для результатов обработки
        std::vector<std::vector<int>> result = image;

        // Обрабатываем только внутренние пиксели (не края)
        // Начинаем с y=1 и заканчиваем y=height-2 (не включая края)
        for (int y = 1; y < height - 1; ++y) {
            // Аналогично для x: от 1 до width-2
            for (int x = 1; x < width - 1; ++x) {
                // Применяем фильтр размытия к пикселю (x,y)
                result[y][x] = blurPixel(x, y);
            }
        }

        // Заменяем оригинальное изображение результатом (move - перемещение, не копирование)
        image = std::move(result);
    }

    // Метод многопоточного применения фильтра
    // int num_threads - количество потоков для использования
    void applyFilterMultiThread(int num_threads) {
        // Создаем копию изображения для результатов
        std::vector<std::vector<int>> result = image;

        // Вектор для хранения потоков
        std::vector<std::thread> threads;

        // Вычисляем высоту порции для каждого потока
        int chunk_height = height / num_threads;

        // Лямбда-функция - рабочая функция для потоков
        // [&] - захват всех внешних переменных по ссылке
        auto worker = [&](int thread_id) {
            // Вычисляем начальную и конечную строки для этого потока
            int start_y = thread_id * chunk_height;

            // Тернарный оператор: если последний поток - обрабатываем до конца
            int end_y = (thread_id == num_threads - 1) ? height - 1 : start_y + chunk_height;

            // Убеждаемся что работаем только с внутренними пикселями
            // std::max возвращает максимум из двух значений
            start_y = std::max(1, start_y);

            // std::min возвращает минимум из двух значений
            end_y = std::min(height - 1, end_y);

            // Обрабатываем пиксели в выделенной полосе
            for (int y = start_y; y < end_y; ++y) {
                for (int x = 1; x < width - 1; ++x) {
                    result[y][x] = blurPixel(x, y);
                }
            }
        };

        // Создаем и запускаем потоки
        for (int i = 0; i < num_threads; ++i) {
            // emplace_back создает поток на месте
            threads.emplace_back(worker, i);
        }

        // Ждем завершения всех потоков
        for (auto& t : threads) {
            t.join();  // join() ждет завершения потока
        }

        // Заменяем изображение результатом
        image = std::move(result);
    }

    // Приватный метод для применения фильтра размытия к одному пикселю
    // int x, int y - координаты пикселя для обработки
    int blurPixel(int x, int y) {
        int sum = 0;    // Сумма значений соседних пикселей
        int count = 0;  // Количество учтенных пикселей

        // Проходим по 3x3 окну вокруг пикселя (x,y)
        // dy и dx изменяются от -1 до +1
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                // Вычисляем координаты соседнего пикселя
                int ny = y + dy;  // new y
                int nx = x + dx;  // new x

                // Проверяем что соседний пиксель находится в пределах изображения
                if (ny >= 0 && ny < height && nx >= 0 && nx < width) {
                    // Добавляем значение пикселя к сумме
                    sum += image[ny][nx];

                    // Увеличиваем счетчик пикселей
                    count++;
                }
            }
        }

        // Возвращаем среднее значение (целочисленное деление)
        return sum / count;
    }

    // Метод для вывода статистики изображения
    void printStatistics() {
        long long total = 0;      // Сумма всех пикселей
        int min_val = 255;        // Минимальное значение (начинаем с максимума)
        int max_val = 0;          // Максимальное значение (начинаем с минимума)

        // Проходим по всем пикселям изображения
        for (const auto& row : image) {          // Для каждой строки
            for (int val : row) {                // Для каждого пикселя в строке
                total += val;                    // Добавляем к общей сумме
                min_val = std::min(min_val, val); // Обновляем минимум
                max_val = std::max(max_val, val); // Обновляем максимум
            }
        }

        // Вычисляем среднее значение (преобразуем к double для точности)
        double avg = static_cast<double>(total) / (width * height);

        // Выводим статистику на экран
        std::cout << "Image statistics: avg=" << avg
            << ", min=" << min_val << ", max=" << max_val << std::endl;
    }
};


int main() {
   
    setlocale(LC_ALL, "Russian");

    // Создаем объект для обработки изображения 100x100 пикселей
    ImageProcessor processor(100, 100);

    // Выводим статистику исходного изображения
    std::cout << "Исходное изображение:" << std::endl;
    processor.printStatistics();

    // Замеряем время однопоточной обработки
    auto start = std::chrono::high_resolution_clock::now();
    processor.applyFilterSingleThread();
    auto end = std::chrono::high_resolution_clock::now();
    auto single_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Выводим статистику после однопоточной обработки
    std::cout << "После однопоточной обработки (" << single_duration.count() << " мс):" << std::endl;
    processor.printStatistics();

    // Создаем новое изображение для сравнения многопоточной обработки
    ImageProcessor processor2(100, 100);

    // Замеряем время многопоточной обработки
    start = std::chrono::high_resolution_clock::now();
    processor2.applyFilterMultiThread(4);  // Используем 4 потока
    end = std::chrono::high_resolution_clock::now();
    auto multi_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Выводим статистику после многопоточной обработки
    std::cout << "После многопоточной обработки (" << multi_duration.count() << " мс):" << std::endl;
    processor2.printStatistics();

    // Завершаем программу успешно
    return 0;
}
