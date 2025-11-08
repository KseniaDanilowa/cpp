#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>
#include <locale>
using namespace std;

template<typename T>
class Stack {
private:
    vector<T> elements;
    int capacity;

public:
    Stack(int size = 10) : capacity(size) {}

    void push(const T& element) {
        if (isFull()) {
            throw overflow_error("Стек переполнен");
        }
        elements.push_back(element);
    }

    T pop() {
        if (isEmpty()) {
            throw underflow_error("Стек пуст");
        }
        T topElement = elements.back();
        elements.pop_back();
        return topElement;
    }

    T top() const {
        if (isEmpty()) {
            throw underflow_error("Стек пуст");
        }
        return elements.back();
    }

    bool isEmpty() const {
        return elements.empty();
    }

    bool isFull() const {
        return elements.size() >= capacity;
    }

    int size() const {
        return elements.size();
    }

    void display() const {
        cout << "Стек: ";
        for (const auto& elem : elements) {
            cout << elem << " ";
        }
        cout << endl;
    }
};

template<typename T>
class Queue {
private:
    vector<T> elements;
    int capacity;

public:
    Queue(int size = 10) : capacity(size) {}

    void enqueue(const T& element) {
        if (isFull()) {
            throw overflow_error("Очередь переполнена");
        }
        elements.push_back(element);
    }

    T dequeue() {
        if (isEmpty()) {
            throw underflow_error("Очередь пуста");
        }
        T frontElement = elements[0];
        elements.erase(elements.begin());
        return frontElement;
    }

    T front() const {
        if (isEmpty()) {
            throw underflow_error("Очередь пуста");
        }
        return elements[0];
    }

    bool isEmpty() const {
        return elements.empty();
    }

    bool isFull() const {
        return elements.size() >= capacity;
    }

    int size() const {
        return elements.size();
    }
};

int main() {
    setlocale(LC_ALL, "rus");

    // Тест Stack<int>
    Stack<int> intStack(3);
    intStack.push(1);
    intStack.push(2);
    intStack.push(3);
    intStack.display();

    cout << "Верх стека: " << intStack.top() << endl;
    cout << "Вытащили: " << intStack.pop() << endl;
    cout << "Размер: " << intStack.size() << endl;

    // Тест Queue<string>
    Queue<string> stringQueue(3);
    stringQueue.enqueue("Привет");
    stringQueue.enqueue("Мир");
    cout << "Первый в очереди: " << stringQueue.front() << endl;
    cout << "Вытащили: " << stringQueue.dequeue() << endl;
    cout << "Размер: " << stringQueue.size() << endl;

    return 0;
}