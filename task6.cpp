#include <iostream>
#include <string>
#include <locale>
using namespace std;

class BankAccount {
private:
    string accountNumber;
    string ownerName;
    double balance;
    static int totalAccounts;           // Общее количество счетов
    static double totalBankBalance;     // Общий баланс банка
    const double MIN_BALANCE = 10.0;    // Минимальный баланс

public:
    BankAccount(string accNum, string owner, double initialBalance)
        : accountNumber(accNum), ownerName(owner), balance(initialBalance) {
        totalAccounts++;
        totalBankBalance += balance;
    }

    ~BankAccount() {
        totalAccounts--;
        totalBankBalance -= balance;
    }

    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            totalBankBalance += amount;
        }
    }

    bool withdraw(double amount) {
        if (amount > 0 && balance - amount >= MIN_BALANCE) {
            balance -= amount;
            totalBankBalance -= amount;
            return true;
        }
        cout << "Недостаточно средств или минимальный баланс будет нарушен." << endl;
        return false;
    }

    void displayAccountInfo() const {
        cout << "Счёт: " << accountNumber << ", Владелец: " << ownerName
            << ", Баланс: " << balance << endl;
    }

    static int getTotalAccounts() {
        return totalAccounts;
    }

    static double getTotalBankBalance() {
        return totalBankBalance;
    }

    static double getAverageBalance() {
        if (totalAccounts > 0) {
            return totalBankBalance / totalAccounts;
        }
        return 0.0;
    }
};

// Инициализация статических переменных
int BankAccount::totalAccounts = 0;
double BankAccount::totalBankBalance = 0.0;

int main() {
    setlocale(LC_ALL, "rus");

    cout << "Начальное количество счетов: " << BankAccount::getTotalAccounts() << endl;
    cout << "Начальный общий баланс: " << BankAccount::getTotalBankBalance() << endl;

    {
        BankAccount acc1("001", "Иванов", 100.0);
        acc1.displayAccountInfo();

        BankAccount acc2("002", "Петров", 200.0);
        acc2.displayAccountInfo();

        cout << "Количество счетов: " << BankAccount::getTotalAccounts() << endl;
        cout << "Общий баланс: " << BankAccount::getTotalBankBalance() << endl;
        cout << "Средний баланс: " << BankAccount::getAverageBalance() << endl;

        acc1.withdraw(50.0);
        cout << "После снятия 50 с acc1:" << endl;
        cout << "Общий баланс: " << BankAccount::getTotalBankBalance() << endl;
    }

    cout << "После выхода из блока:" << endl;
    cout << "Количество счетов: " << BankAccount::getTotalAccounts() << endl;
    cout << "Общий баланс: " << BankAccount::getTotalBankBalance() << endl;

    return 0;
}