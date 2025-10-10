#include <iostream>
using namespace std;

int main() {

    setlocale(LC_ALL, "Russian");

    const int MAX = 256;          
    char str[MAX];

    cout << "Введите строку: ";
    cin.getline(str, MAX);      

    
    char* p = str;
    while (*p) ++p;               
    int len = p - str;            // разность указателей = длина
    cout << "Длина строки: " << len << '\n';

    
    char* left = str;
    char* right = str + len - 1;
    while (left < right) {
        char tmp = *left;
        *left = *right;
        *right = tmp;
        ++left;
        --right;
    }
    cout << "Перевернутая строка: " << str << '\n';

    
    char ch;
    cout << "Какой символ подсчитать? ";
    cin >> ch;

    int count = 0;
    for (char* q = str; *q; ++q)
        if (*q == ch) ++count;

    cout << "Символ '" << ch << "' встречается " << count << " раз(а).\n";
    return 0;
}