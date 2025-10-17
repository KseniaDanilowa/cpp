#include <iostream>
using namespace std;


void swapValuesByValue(int a, int b) {
    int tmp = a;
    a = b;
    b = tmp;
    cout << "Внутри функции: a=" << a << ", b=" << b << '\n';
}

//получаем адреса исходных переменных, меняем оригиналы через разыменование
void swapValues(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

// получаем ссылки на исходные переменные, которые  являются альтернативными именами для оригиналов
void swapValues(int& a, int& b) {
    int tmp = a;
    a = b;
    b = tmp;
}

int main() {
    setlocale(LC_ALL, "Russian");
    int x = 5, y = 10;

    cout << "Начальные значения: x=" << x << ", y=" << y << "\n\n";

    /* 1. По значению – не влияет на x,y */
    cout << "----- swapValuesByValue(x, y) -----\n";
    swapValuesByValue(x, y);
    cout << "После вызова: x=" << x << ", y=" << y << "\n\n";

    /* 2. По указателю – влияет */
    cout << "----- swapValues(&x, &y) по указателю -----\n";
    swapValues(&x, &y);
    cout << "После вызова: x=" << x << ", y=" << y << "\n\n";

    /* 3. По ссылке – влияет */
    cout << "----- swapValues(x, y) по ссылке -----\n";
    swapValues(x, y);          // теперь снова меняем
    cout << "После вызова: x=" << x << ", y=" << y << '\n';

    return 0;
}