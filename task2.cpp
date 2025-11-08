#include <iostream>
#include <string>
using namespace std;

class Person {
protected:
    string name;
    int age;
    string address;

public:
    Person(string n, int a, string addr) : name(n), age(a), address(addr) {}
    virtual void displayInfo() const {
        cout << "Имя: " << name << ", Возраст: " << age << ", Адрес: " << address << endl;
    }
    virtual ~Person() {}
};

class Student : public Person {
private:
    string studentId;
    double averageGrade;

public:
    Student(string n, int a, string addr, string id, double avg)
        : Person(n, a, addr), studentId(id), averageGrade(avg) {}

    //override - гарантия, что функция в производном классе переопределяет виртуальную функцию базового класса
    void displayInfo() const override {
        Person::displayInfo();
        cout << "ID студента: " << studentId << ", Средний балл: " << averageGrade << endl;
    }

    void setAverageGrade(double avg) {
        if (avg >= 0 && avg <= 5) {
            averageGrade = avg;
        }
        else {
            cout << "Ошибка: средний балл должен быть от 0 до 5" << endl;
        }
    }

    double getAverageGrade() const {
        return averageGrade;
    }
};

class Professor : public Person {
private:
    string department;
    double salary;
    int yearsOfExperience;

public:
    Professor(string n, int a, string addr, string dept, double sal, int exp)
        : Person(n, a, addr), department(dept), salary(sal), yearsOfExperience(exp) {}

    void displayInfo() const override {
        Person::displayInfo();
        cout << "Кафедра: " << department << ", Зарплата: " << salary << ", Стаж: " << yearsOfExperience << " лет" << endl;
    }

    double calculateExperienceBonus() const {
        // 5% за каждый год стажа
        return salary * 0.05 * yearsOfExperience;
    }
};

int main() {
    setlocale(LC_ALL, "rus");  

    Student student("Петр Петров", 20, "ул. Студенческая, 15", "S12345", 4.3);
    Professor prof("Доктор Иванов", 45, "ул. Академическая, 10", "Компьютерные науки", 50000, 15);

    student.displayInfo();
    prof.displayInfo();

    cout << "Надбавка профессора за стаж: " << prof.calculateExperienceBonus() << endl;

    return 0;
}