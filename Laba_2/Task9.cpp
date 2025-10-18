#include <iostream>
using namespace std;

struct Account {
    int    number;   
    double balance;  
};

// увеличиваем баланс через указатель 
void deposit(Account* acc, double amount) {
    if (amount <= 0) return;      // защита от отрицательного баланса
    acc->balance += amount;       // ->  — это синоним разыменования + доступа к полю в одном символе: (*acc).balance
    cout << "Счёт " << acc->number
        << " пополнен на " << amount
        << " р.  Баланс: " << acc->balance << " р.\n";
}

// уменьшаем баланс через ссылке с проверкой средств 
void withdraw(Account& acc, double amount) {
    if (amount <= 0) return;
    if (acc.balance < amount)
        cout << "Ошибка: недостаточно средств на счёте "
        << acc.number << "!\n";
    else {
        acc.balance -= amount;    // доступ по ссылке через точку
        cout << "Со счёта " << acc.number
            << " снято " << amount
            << " р.  Баланс: " << acc.balance << " р.\n";
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    Account myAcc = { 123456, 1000.0 };   

    cout << "Начальный баланс счёта " << myAcc.number
        << ": " << myAcc.balance << " р.\n\n";

    deposit(&myAcc, 250.0);   
    withdraw(myAcc, 300.0);   

    withdraw(myAcc, 2000.0);  

    return 0;
}