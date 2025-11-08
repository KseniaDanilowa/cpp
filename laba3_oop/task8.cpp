#include <iostream>
#include <string>
#include <locale>
using namespace std;

class Person {
protected:
    string name;
    int age;

public:
    Person(string n, int a) : name(n), age(a) {}
    virtual void display() const {
        cout << "Имя: " << name << ", Возраст: " << age << endl;
    }
};

class Employee {
protected:
    string position;
    double salary;

public:
    Employee(string pos, double sal) : position(pos), salary(sal) {}
    virtual void work() const {
        cout << "Выполняет работу как сотрудник" << endl;
    }
};

// Teacher наследует от Person и Employee (оба виртуально)
class Teacher : public virtual Person, public virtual Employee {
protected:  // <-- Сделали protected
    string subject;
    int experienceYears;

public:
    Teacher(string n, int a, string pos, double sal, string sub, int exp)
        : Person(n, a), Employee(pos, sal), subject(sub), experienceYears(exp) {}

    void display() const override {
        Person::display();
        cout << "Должность: " << position << ", Зарплата: " << salary << endl;
        cout << "Предмет: " << subject << ", Стаж: " << experienceYears << " лет" << endl;
    }

    void work() const override {
        cout << "Преподает предмет: " << subject << endl;
    }

    void teach() const {
        cout << name << " ведет урок по " << subject << endl;
    }
};

// Researcher тоже наследует от Person (виртуально)
class Researcher : public virtual Person {
protected:
    string researchArea;
    int publicationsCount;

public:
    // Принимает параметры для Person
    Researcher(string area, int publications, string n, int a)
        : Person(n, a), researchArea(area), publicationsCount(publications) {}

    void conductResearch() const {
        cout << "Проводит исследования в области: " << researchArea << endl;
    }
};

// Professor наследует от Teacher и Researcher
class Professor : public Teacher, public Researcher {
public:
    // Professor сам вызывает Person и Employee
    Professor(string n, int a, string pos, double sal, string sub, int exp, string area, int pubs)
        : Person(n, a), Employee(pos, sal), Teacher(n, a, pos, sal, sub, exp), Researcher(area, pubs, n, a) {}

    void display() const override {
        Person::display();
        cout << "Должность: " << position << ", Зарплата: " << salary << endl;
        cout << "Предмет: " << subject << ", Стаж: " << experienceYears << " лет" << endl;
        cout << "Область исследований: " << researchArea << ", Публикаций: " << publicationsCount << endl;
    }

    void work() const override {
        cout << "Преподает и ведет научные исследования." << endl;
    }
};

int main() {
    setlocale(LC_ALL, "rus");

    Teacher t("Иванов И.И.", 35, "Учитель", 40000, "Математика", 10);
    t.display();
    t.work();
    t.teach();

    cout << endl;

    Professor p("Петров П.П.", 50, "Профессор", 80000, "Физика", 20, "Квантовая механика", 50);
    p.display();
    p.work();
    p.conductResearch();

    return 0;
}