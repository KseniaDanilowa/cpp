// Подключаем заголовочные файлы стандартной библиотеки C++
// Эти файлы содержат готовые функции и классы, которые мы будем использовать
#include <iostream>    // Для ввода-вывода (cout, endl) - позволяет выводить текст на экран
#include <vector>      // Для работы с динамическими массивами (векторами)
#include <thread>      // Для создания и управления потоками выполнения
#include <mutex>       // Для синхронизации потоков (мьютексы)
#include <chrono>      // Для измерения времени выполнения
#include <random>      // Для генерации случайных чисел
#include <clocale>     // Для функций локализации (setlocale) - нужен для русских символов

// Объявляем класс VectorSumCalculator
// Класс - это шаблон для создания объектов, объединяющий данные и функции
class VectorSumCalculator {
private:  // private - это модификатор доступа, означающий что эти члены доступны только внутри класса
    std::vector<int> data;     // Вектор для хранения целых чисел (динамический массив)
    std::mutex sum_mutex;      // Мьютекс для защиты общей суммы от одновременного доступа

public:   // public - модификатор доступа, члены доступны извне класса
    // Конструктор класса - специальная функция, которая вызывается при создании объекта
    // size_t - это тип данных для размеров, обычно unsigned long long
    VectorSumCalculator(size_t size) {
        // Создаем генератор случайных чисел на основе аппаратного шума
        std::random_device rd;  // random_device получает случайные числа от ОС

        // Инициализируем генератор Мерсенна-Твистера с seed от random_device
        std::mt19937 gen(rd());  // mt19937 - высококачественный генератор случайных чисел

        // Создаем распределение для равномерного распределения чисел от 1 до 100
        std::uniform_int_distribution<> dis(1, 100);

        // Резервируем место в векторе для size элементов (оптимизация производительности)
        data.reserve(size);

        // Цикл for: инициализируем i=0, выполняем пока i < size, после каждой итерации i++
        for (size_t i = 0; i < size; ++i) {
            // Добавляем случайное число в конец вектора
            data.push_back(dis(gen));  // dis(gen) генерирует число от 1 до 100
        }
    }

    // Метод для однопоточного вычисления суммы всех элементов вектора
    // long long - тип данных для больших целых чисел (64 бита)
    long long calculateSingleThreaded() {
        long long sum = 0;  // Инициализируем сумму нулем

        // Цикл по диапазону: для каждого элемента num в векторе data
        // Это современный синтаксис цикла for для контейнеров C++
        for (int num : data) {
            sum += num;  // Добавляем текущее число к сумме
        }
        return sum;  // Возвращаем итоговую сумму
    }

    // Метод для многопоточного вычисления суммы
    // int num_threads - параметр, указывающий количество потоков для использования
    long long calculateMultiThreaded(int num_threads) {
        long long total_sum = 0;  // Общая сумма, к которой будут добавлять частичные суммы

        // Вектор для хранения объектов потоков
        std::vector<std::thread> threads;

        // Вычисляем размер порции данных для каждого потока
        // size_t - беззнаковый целочисленный тип для размеров
        size_t chunk_size = data.size() / num_threads;

        // Лямбда-функция - анонимная функция, определенная прямо в коде
        // [&] - захват всех переменных по ссылке (можно изменять внешние переменные)
        // (int thread_id) - параметр функции
        auto worker = [&](int thread_id) {
            // Вычисляем начальный и конечный индексы для этого потока
            size_t start = thread_id * chunk_size;
            // Тернарный оператор: условие ? значение_если_true : значение_если_false
            size_t end = (thread_id == num_threads - 1) ? data.size() : start + chunk_size;

            long long local_sum = 0;  // Локальная сумма для этого потока

            // Суммируем элементы от start до end
            for (size_t i = start; i < end; ++i) {
                local_sum += data[i];
            }

            // Блокируем мьютекс для безопасного доступа к общей переменной
            // lock_guard автоматически разблокирует мьютекс при выходе из области видимости
            std::lock_guard<std::mutex> lock(sum_mutex);
            total_sum += local_sum;  // Добавляем локальную сумму к общей
        };

        // Создаем и запускаем потоки
        // emplace_back создает объект на месте, передавая аргументы конструктору
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(worker, i);  // Создаем поток с функцией worker и параметром i
        }

        // Ждем завершения всех потоков
        // join() блокирует выполнение до тех пор, пока поток не завершится
        for (auto& t : threads) {
            t.join();
        }

        return total_sum;  // Возвращаем итоговую сумму
    }
};


int main() {
    
    setlocale(LC_ALL, "Russian");


    // Создаем объект calculator класса VectorSumCalculator
    // Передаем 10 миллионов как размер вектора
    VectorSumCalculator calculator(10000000);

    // Замеряем время начала однопоточного вычисления
    // auto - автоматическое определение типа переменной
    auto start = std::chrono::high_resolution_clock::now();

    // Вызываем метод однопоточного суммирования
    auto single_result = calculator.calculateSingleThreaded();

    // Замеряем время окончания
    auto end = std::chrono::high_resolution_clock::now();

    // Вычисляем продолжительность в миллисекундах
    auto single_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Повторяем замеры для многопоточного вычисления
    start = std::chrono::high_resolution_clock::now();
    auto multi_result = calculator.calculateMultiThreaded(4);  // Используем 4 потока
    end = std::chrono::high_resolution_clock::now();
    auto multi_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Выводим результаты сравнения
    std::cout << "Однопоточный результат: " << single_result << " (время: " << single_duration.count() << " мс)" << std::endl;
    std::cout << "Многопоточный результат: " << multi_result << " (время: " << multi_duration.count() << " мс)" << std::endl;

    // Проверяем совпадение результатов
    // Тернарный оператор для выбора строки в зависимости от условия
    std::cout << "Результаты совпадают: " << (single_result == multi_result ? "Да" : "Нет") << std::endl;

    // Возвращаем 0, что означает успешное завершение программы
    return 0;
}
