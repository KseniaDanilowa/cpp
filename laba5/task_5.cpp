// Подключаем необходимые заголовочные файлы
#include <iostream>              // Для ввода-вывода
#include <thread>                // Для работы с потоками
#include <vector>                // Для векторов (динамических массивов)
#include <queue>                 // Для очередей (FIFO - First In, First Out)
#include <future>                // Для работы с асинхронными задачами (future, packaged_task)
#include <functional>            // Для функциональных объектов (std::function)
#include <mutex>                 // Для мьютексов - синхронизации потоков
#include <condition_variable>    // Для условных переменных
#include <clocale>               // Для функций локализации (setlocale) - нужен для русских символов

// Класс пула потоков - управляет группой рабочих потоков
class ThreadPool {
private:  // Приватные члены - доступны только внутри класса
    std::vector<std::thread> workers;        // Вектор рабочих потоков
    std::queue<std::function<void()>> tasks; // Очередь задач (функций для выполнения)
    std::mutex queue_mutex;                  // Мьютекс для защиты очереди от одновременного доступа
    std::condition_variable condition;       // Условная переменная для синхронизации потоков
    bool stop = false;                       // Флаг остановки пула

public:   // Публичные методы - доступны извне
    // Конструктор пула потоков
    // size_t num_threads - количество рабочих потоков для создания
    ThreadPool(size_t num_threads) {
        // Создаем указанное количество рабочих потоков
        for (size_t i = 0; i < num_threads; ++i) {
            // emplace_back создает поток на месте, передавая лямбда-функцию
            workers.emplace_back([this]() {  // [this] - захватываем указатель на текущий объект
                // Бесконечный цикл работы потока
                while (true) {
                    std::function<void()> task;  // Переменная для хранения задачи

                    // Блок защищенный мьютексом
                    {
                        // Создаем unique_lock для защиты доступа к очереди
                        std::unique_lock<std::mutex> lock(queue_mutex);

                        // Ждем пока условие не станет истинным
                        // condition.wait() атомарно разблокирует мьютекс и ждет уведомления
                        condition.wait(lock, [this]() {
                            // Условие: пул остановлен ИЛИ есть задачи в очереди
                            return stop || !tasks.empty();
                            });

                        // Если пул остановлен и задач нет - завершаем поток
                        if (stop && tasks.empty()) {
                            return;  // Выход из бесконечного цикла
                        }

                        // Извлекаем задачу из очереди
                        // std::move - перемещаем объект вместо копирования
                        task = std::move(tasks.front());
                        tasks.pop();  // Удаляем задачу из очереди
                    }  // Мьютекс автоматически разблокируется здесь

                    // Выполняем задачу вне блока мьютекса
                    task();
                }
                });
        }
    }

    // Шаблонный метод для отправки задачи в пул
    // F&& f - функция для выполнения (universal reference - может быть lvalue или rvalue)
    // Args&&... args - параметры функции (параметры шаблона variadic - переменное количество)
    template<class F, class... Args>
    // auto - автоматическое определение типа возврата
    // decltype(f(args...)) - тип, который возвращает вызов f(args...)
    auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
        // Определяем тип возврата функции f
        using return_type = decltype(f(args...));

        // Создаем packaged_task - обертку для функции, которая может быть выполнена асинхронно
        // std::bind связывает функцию f с ее аргументами args...
        // std::forward сохраняет типы ссылок (lvalue/rvalue)
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );

        // Получаем future объект для получения результата позже
        std::future<return_type> res = task->get_future();

        // Блок защищенный мьютексом для добавления задачи в очередь
        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            // Если пул остановлен - выбрасываем исключение
            if (stop) {
                throw std::runtime_error("submit on stopped ThreadPool");
            }

            // Добавляем задачу в очередь
            // [task]() { (*task)(); } - лямбда, которая вызывает packaged_task
            tasks.emplace([task]() { (*task)(); });
        }  // Мьютекс разблокируется здесь

        // Уведомляем один рабочий поток о появлении новой задачи
        condition.notify_one();

        // Возвращаем future для получения результата позже
        return res;
    }

    // Деструктор класса - вызывается при уничтожении объекта
    ~ThreadPool() {
        // Блок для установки флага остановки
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;  // Устанавливаем флаг остановки
        }  // Мьютекс разблокируется

        // Уведомляем ВСЕ потоки о необходимости завершения
        condition.notify_all();

        // Ждем завершения всех рабочих потоков
        for (std::thread& worker : workers) {
            worker.join();  // join() ждет завершения потока
        }
    }
};

// Обычная функция для демонстрации работы пула
// Вычисляет факториал числа n (n! = 1 * 2 * 3 * ... * n)
// unsigned long long - тип для больших целых чисел без знака (64 бита)
unsigned long long factorial(int n) {
    // Базовый случай рекурсии: факториал 0 и 1 равен 1
    if (n <= 1) return 1;

    // Инициализируем результат единицей
    unsigned long long result = 1;

    // Цикл от 2 до n включительно
    for (int i = 2; i <= n; ++i) {
        result *= i;  // Умножаем результат на текущее число
    }

    return result;  // Возвращаем вычисленный факториал
}


int main() {
    
    setlocale(LC_ALL, "Russian");

    // Создаем пул из 4 рабочих потоков
    ThreadPool pool(4);

    // Вектор для хранения future объектов (обещаний результатов)
    std::vector<std::future<unsigned long long>> results;

    // Отправляем 10 задач на вычисление факториалов от 1! до 10!
    for (int i = 1; i <= 10; ++i) {
        // submit возвращает future, который мы сохраняем в векторе
        results.emplace_back(pool.submit(factorial, i));
    }

    // Собираем и выводим результаты всех задач
    for (int i = 1; i <= 10; ++i) {
        // future.get() ждет завершения задачи и возвращает результат
        unsigned long long result = results[i - 1].get();

        // Выводим результат: "5! = 120"
        std::cout << i << "! = " << result << std::endl;
    }

    // Выводим сообщение о завершении
    std::cout << "Все задачи выполнены." << std::endl;

    // Успешное завершение программы
    return 0;
}
