#include <iostream>
using namespace std;


void arrayInfo(const int* arr, int size)   // arr – указатель на первый элемент
{
    if (size <= 0) {                       
        cout << "Массив пуст.\n";
        return;
    }

    int sum = 0;
    for (const int* p = arr; p < arr + size; ++p)  // обход указателем
        sum += *p;

    cout << "Размер массива: " << size << '\n';
    cout << "Сумма элементов: " << sum << '\n';
    cout << "Первый элемент: " << *arr << '\n';           // *arr = arr[0]
    cout << "Последний элемент: " << *(arr + size - 1)     // *(arr + size - 1) = arr[size-1]
        << "\n\n";
}

int main()
{
    setlocale(LC_ALL, "Russian");

    int a[] = { 3, 9, -4, 7, 2, 5 };
    int b[] = { 10, 20, 30 };

    cout << "Массив a[]: ";
    arrayInfo(a, sizeof(a) / sizeof(a[0]));   

    cout << "Массив b[]: ";
    arrayInfo(b, sizeof(b) / sizeof(b[0]));   

    return 0;
}