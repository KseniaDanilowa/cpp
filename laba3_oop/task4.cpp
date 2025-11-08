#include <iostream>
#include <string>
#include <locale>
using namespace std;

class Vehicle {
protected:
    string brand;
    string model;
    int year;
    double fuelLevel;

public:
    Vehicle(string b, string m, int y) : brand(b), model(m), year(y), fuelLevel(100) {}
    virtual ~Vehicle() {}

    virtual void startEngine() = 0;
    virtual void stopEngine() = 0;
    virtual void refuel(double amount) = 0;

    virtual void displayInfo() const {
        cout << brand << " " << model << " (" << year << ")" << endl;
        cout << "Уровень топлива: " << fuelLevel << "%" << endl;
    }

    double getFuelLevel() const { return fuelLevel; }
    void setFuelLevel(double level) {
        if (level >= 0 && level <= 100) fuelLevel = level;
    }
};

class Car : public Vehicle {
private:
    int doors;
    string transmission;

public:
    Car(string b, string m, int y, int d, string trans)
        : Vehicle(b, m, y), doors(d), transmission(trans) {}

    void startEngine() override {
        cout << "Автомобиль " << brand << " завел двигатель." << endl;
    }

    void stopEngine() override {
        cout << "Автомобиль " << brand << " заглушил двигатель." << endl;
    }

    void refuel(double amount) override {
        if (amount > 0) {
            fuelLevel += amount;
            if (fuelLevel > 100) fuelLevel = 100;
            cout << "Автомобиль заправлен на " << amount << "%. Уровень топлива: " << fuelLevel << "%" << endl;
        }
    }
};

class Motorcycle : public Vehicle {
private:
    bool hasFairing;

public:
    Motorcycle(string b, string m, int y, bool fairing)
        : Vehicle(b, m, y), hasFairing(fairing) {}

    void startEngine() override {
        cout << "Мотоцикл " << brand << " завел двигатель." << endl;
    }

    void stopEngine() override {
        cout << "Мотоцикл " << brand << " заглушил двигатель." << endl;
    }

    void refuel(double amount) override {
        if (amount > 0) {
            fuelLevel += amount;
            if (fuelLevel > 100) fuelLevel = 100;
            cout << "Мотоцикл заправлен на " << amount << "%. Уровень топлива: " << fuelLevel << "%" << endl;
        }
    }

    void wheelie() {
        cout << brand << " " << model << " делает вилли!" << endl;
    }
};

int main() {
    setlocale(LC_ALL, "rus");

    Vehicle* vehicles[] = {
        new Car("Toyota", "Camry", 2020, 4, "Автомат"),
        new Motorcycle("Yamaha", "R1", 2021, true)
    };

    for (int i = 0; i < 2; ++i) {
        vehicles[i]->displayInfo();
        vehicles[i]->startEngine();
        vehicles[i]->refuel(10);
        vehicles[i]->stopEngine();
        cout << "------------------------" << endl;
    }

    //уникальный метод, который имеет смысл только для мотоцикла
    Motorcycle* moto = dynamic_cast<Motorcycle*>(vehicles[1]);
    if (moto) {
        moto->wheelie();
    }

    for (int i = 0; i < 2; ++i) {
        delete vehicles[i];
    }

    return 0;
}