#include <iostream>
#include <cmath>        
#include <iomanip>      
#include <string>

using namespace std;


double to_rad(double deg) { return deg * 3.14159265359 / 180.0; }


string int_to_base(long long n, int base)
{
    if (n == 0) return "0";
    string digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string res;
    bool neg = false;
    if (n < 0) { neg = true; n = -n; }
    while (n)
    {
        res = digits[n % base] + res;
        n /= base;
    }
    return neg ? "-" + res : res;
}


void show_menu()
{
    cout << "\n=== Доступные операции ===\n";
    cout << "1)  +   сложение\n";
    cout << "2)  -   вычитание\n";
    cout << "3)  *   умножение\n";
    cout << "4)  /   деление\n";
    cout << "5)  ^   степень (a^b)\n";
    cout << "6)  r   квадратный корень (sqrt(a))\n";
    cout << "7)  s   синус (sin(a))  – угол в градусах\n";
    cout << "8)  c   косинус (cos(a)) – угол в градусах\n";
    cout << "9)  t   тангенс (tan(a)) – угол в градусах\n";
    cout << "10) l   натуральный логарифм (ln(a))\n";
    cout << "11) L   десятичный логарифм (lg(a))\n";
    cout << "12) !   факториал (a!)  – a целое неотрицательное, a<20\n";
    cout << "13) b   перевести a в систему счисления\n";
    cout << "0)  x   выход\n";
    cout << "==========================\n";
}


int main()
{
    setlocale(LC_ALL, "rus");   
    cout << "=== Инженерный калькулятор ===\n";

    double a = 0, b = 0;
    cout << "Введите первое число: ";  cin >> a;
    cout << "Введите второе число: ";  cin >> b;

    char choice;
    do
    {
        show_menu();
        cout << "Выберите операцию: ";
        cin >> choice;

        switch (choice)
        {
        case '1': cout << "Результат: " << a + b << '\n'; break;
        case '2': cout << "Результат: " << a - b << '\n'; break;
        case '3': cout << "Результат: " << a * b << '\n'; break;
        case '4':
            if (b == 0) cout << "Ошибка: деление на ноль!\n";
            else        cout << "Результат: " << a / b << '\n';
            break;
        case '5': cout << "Результат: " << pow(a, b) << '\n'; break;
        case '6': cout << "sqrt(" << a << ") = " << sqrt(a) << '\n'; break;
        case '7': cout << "sin(" << a << "°) = " << sin(to_rad(a)) << '\n'; break;
        case '8': cout << "cos(" << a << "°) = " << cos(to_rad(a)) << '\n'; break;
        case '9': cout << "tan(" << a << "°) = " << tan(to_rad(a)) << '\n'; break;
        case 'l':
        case 'L':
            if (a <= 0) cout << "Ошибка: аргумент логарифма должен быть > 0\n";
            else cout << (choice == 'l' ? "ln(" : "lg(") << a << ") = "
                << (choice == 'l' ? log(a) : log10(a)) << '\n';
            break;
        case '!':
        {
            if (a < 0 || a > 20 || floor(a) != a)
                cout << "Факториал определён для целых 0–20\n";
            else
            {
                long long f = 1;
                for (int i = 2; i <= (int)a; ++i) f *= i;
                cout << (int)a << "! = " << f << '\n';
            }
            break;
        }
        case 'b':
        {
            int base;
            cout << "Введите целевую систему счисления (2–36): ";
            cin >> base;
            if (base < 2 || base > 36)
                cout << "Основание должно быть 2–36\n";
            else
                cout << "Число " << a << " в системе " << base
                << " = " << int_to_base((long long)a, base) << '\n';
            break;
        }
        case 'x': case '0': cout << "Работа завершена. До свидания!\n"; break;
        default: cout << "Неизвестная операция, попробуйте ещё раз.\n";
        }
    } while (choice != 'x' && choice != '0');

    return 0;
}