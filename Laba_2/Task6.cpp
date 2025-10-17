#include <iostream>
using namespace std;

const int N = 10;   


void makeFib(int* start, int* end) {
    int* p = start;
    *p = 0;
    if (p + 1 < end) *(p + 1) = 1;
    for (p = start + 2; p < end; ++p)
        *p = *(p - 1) + *(p - 2);
}


int sumArr(const int* start, const int* end) {
    int s = 0;
    for (const int* p = start; p < end; ++p) s += *p;
    return s;
}


int minArr(const int* start, const int* end) {
    int m = *start;
    for (const int* p = start + 1; p < end; ++p)
        if (*p < m) m = *p; // сравниваем через разыменование
    return m;
}

/* копируем диапазон [srcStart, srcEnd) в dst в обратном порядке */
void reverseCopy(const int* srcStart, const int* srcEnd, int* dst) {
    --srcEnd;
    while (srcEnd >= srcStart) 
        *dst++ = *srcEnd--; // копируем и сдвигаем указатели
}


void printArr(const int* start, const int* end) {
    for (const int* p = start; p < end; ++p) cout << *p << ' ';
    cout << '\n';
}

int main() {
    setlocale(LC_ALL, "Russian");

    int offset; // номер элемента, с которого пользователь хочет видеть 10 чисел (= смещение от F0)
    cout << "С какого элемента (смещение от 0) начать? ";
    cin >> offset;
    if (offset < 0) offset = 0;

    int len = offset + N;              
    int* fib = new int[len];           

    makeFib(fib, fib + len);           
    int* userArr = fib + offset;       

    cout << "Ваша последовательность: ";
    printArr(userArr, userArr + N);

    cout << "Сумма = " << sumArr(userArr, userArr + N) << '\n';
    cout << "Минимум = " << minArr(userArr, userArr + N) << '\n';

    int rev[N];
    reverseCopy(userArr, userArr + N, rev);
    cout << "В обратном порядке: ";
    printArr(rev, rev + N);

    delete[] fib;                     
    return 0;
}