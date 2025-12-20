// Подключаем заголовочные файлы стандартной библиотеки C++
// Каждый include добавляет определенный набор функций и классов
#include <iostream>              // Для ввода-вывода (cout, endl)
#include <queue>                 // Для работы с очередью (FIFO - First In, First Out)
#include <thread>                // Для создания и управления потоками
#include <mutex>                 // Для синхронизации потоков с помощью мьютексов
#include <condition_variable>    // Для условных переменных - позволяют потокам ждать определенных условий
#include <clocale>               // Для функций локализации (setlocale) - нужен для русских символов

// Шаблонный класс (template) - это как шаблон для создания классов разных типов
// typename T - параметр шаблона, T может быть любым типом (int, double, string и т.д.)
template<typename T>
class ThreadSafeQueue {
private:  // Приватные члены доступны только внутри класса
    std::queue<T> queue_;              // Обычная очередь из стандартной библиотеки
    std::mutex mutex_;                 // Мьютекс для защиты доступа к очереди
    std::condition_variable cond_;     // Условная переменная для синхронизации потоков
    bool shutdown_ = false;            // Флаг завершения работы очереди

public:   // Публичные методы доступны извне класса
    // Метод для добавления элемента в очередь
    // T value - параметр типа T (копируется или перемещается)
    void push(T value) {
        // Создаем unique_lock - более гибкий замок чем lock_guard
        // Он может быть разблокирован и заблокирован вручную
        std::unique_lock<std::mutex> lock(mutex_);

        // Добавляем элемент в очередь с помощью move семантики (перемещение вместо копирования)
        queue_.push(std::move(value));

        // Уведомляем один ожидающий поток о том, что данные появились
        cond_.notify_one();
    }

    // Метод для извлечения элемента из очереди
    // T& value - ссылка на переменную, куда будет помещен извлеченный элемент
    // Возвращает bool: true если элемент извлечен, false если очередь закрыта и пуста
    bool pop(T& value) {
        // Блокируем мьютекс для безопасного доступа
        std::unique_lock<std::mutex> lock(mutex_);

        // Ждем пока условие станет истинным
        // Лямбда-функция [this]() - захватывает указатель на текущий объект
        // Условие: очередь не пуста ИЛИ флаг завершения установлен
        cond_.wait(lock, [this]() {
            return !queue_.empty() || shutdown_;  // Возвращает true если можно продолжать
            });

        // Если очередь пуста и работа завершена - возвращаем false
        if (queue_.empty() && shutdown_) {
            return false;  // Очередь закрыта и пуста
        }

        // Извлекаем элемент из начала очереди с помощью move семантики
        value = std::move(queue_.front());

        // Удаляем элемент из очереди
        queue_.pop();

        // Возвращаем true - элемент успешно извлечен
        return true;
    }

    // Метод для завершения работы очереди
    void shutdown() {
        // Блокируем мьютекс для безопасного изменения флага
        std::unique_lock<std::mutex> lock(mutex_);

        // Устанавливаем флаг завершения
        shutdown_ = true;

        // Уведомляем ВСЕ ожидающие потоки о завершении
        cond_.notify_all();
    }

    // Метод для проверки пустоты очереди
    // const - метод не изменяет объект
    bool empty() const {
        // Блокируем мьютекс для безопасного чтения
        std::unique_lock<std::mutex> lock(mutex_);

        // Возвращаем true если очередь пуста
        return queue_.empty();
    }
};

// Функция производителя - работает в отдельном потоке
// ThreadSafeQueue<int>& queue - ссылка на очередь (не копируем всю очередь)
// int count - количество элементов для генерации
void producer(ThreadSafeQueue<int>& queue, int count) {
    // Цикл от 0 до count-1
    for (int i = 0; i < count; ++i) {
        // Добавляем число i в очередь
        queue.push(i);

        // Приостанавливаем поток на 10 миллисекунд для имитации работы
        // this_thread - относится к текущему потоку
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // После генерации всех элементов завершаем работу очереди
    queue.shutdown();
}

// Функция потребителя - работает в отдельном потоке
// ThreadSafeQueue<int>& queue - ссылка на очередь
// int id - уникальный идентификатор потребителя
void consumer(ThreadSafeQueue<int>& queue, int id) {
    int value;  // Переменная для хранения извлеченного значения

    // Бесконечный цикл пока pop() возвращает true
    while (queue.pop(value)) {
        // Выводим сообщение о обработке элемента
        // "Consumer " + id + " processed: " + value
        std::cout << "Consumer " << id << " processed: " << value << std::endl;

        // Имитируем обработку - пауза 20 миллисекунд
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    // После завершения цикла выводим сообщение о завершении работы
    std::cout << "Consumer " << id << " finished." << std::endl;
}


int main() {
    
    setlocale(LC_ALL, "Russian");
    // Создаем потокобезопасную очередь для целых чисел
    ThreadSafeQueue<int> queue;

    // Создаем поток производителя
    // std::ref() - создает ссылку на объект (не копируем очередь)
    // producer - имя функции, std::ref(queue) - первый аргумент, 20 - второй аргумент
    std::thread producer_thread(producer, std::ref(queue), 20);

    // Создаем вектор для хранения потоков потребителей
    std::vector<std::thread> consumer_threads;

    // Создаем 3 потока потребителей
    for (int i = 0; i < 3; ++i) {
        // Добавляем новый поток в вектор
        // consumer - функция, std::ref(queue) - очередь, i - идентификатор
        consumer_threads.emplace_back(consumer, std::ref(queue), i);
    }

    // Ждем завершения потока производителя
    producer_thread.join();

    // Ждем завершения всех потоков потребителей
    // auto& t - автоматически определяем тип, & - ссылка
    for (auto& t : consumer_threads) {
        t.join();
    }

    // Выводим сообщение о завершении всех потоков
    std::cout << "Все потоки завершили работу." << std::endl;

    // Возвращаем 0 - успешное завершение программы
    return 0;
}
