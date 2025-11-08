#include <iostream>
#include <string>
using namespace std;

class Student {
private:
    string name;
    int age;
    double grades[5];
    double averageGrade;

    void calculateAverage() {
        double sum = 0.0;
        for (int i = 0; i < 5; ++i) {
            sum += grades[i];
        }
        averageGrade = sum / 5;
    }

public:
    Student(string n, int a) : name(n), age(a) {
        for (int i = 0; i < 5; ++i) {
            grades[i] = 0.0;
        }
        averageGrade = 0.0;
    }

    string getName() const { return name; }
    int getAge() const { return age; }

    void setAge(int a) {
        if (a > 0 && a < 150) {
            age = a;
        }
        else {
            cout << "Ошибка: некорректный возраст" << endl;
        }
    }

    void setGrade(int index, double value) {
        if (index >= 0 && index < 5) {
            if (value >= 0 && value <= 5) {
                grades[index] = value;
                calculateAverage();
            }
            else {
                cout << "Ошибка: оценка должна быть от 0 до 10" << endl;
            }
        }
        else {
            cout << "Ошибка: индекс должен быть от 0 до 4" << endl;
        }
    }

    double getGrade(int index) const {
        if (index >= 0 && index < 5) {
            return grades[index];
        }
        else {
            cout << "Ошибка: индекс вне диапазона" << endl;
            return -1;
        }
    }

    void displayInfo() const {
        cout << "Имя: " << name << ", Возраст: " << age << ", Средний балл: " << averageGrade << endl;
    }

    bool hasScholarship() const {
        return averageGrade >= 4.5;
    }
};

int main() {
    setlocale(LC_ALL, "rus");

    Student student("Иван Иванов", 20);

    student.setGrade(0, 5);
    student.setGrade(1, 4);
    student.setGrade(2, 5);
    student.setGrade(3, 3);
    student.setGrade(4, 4);

    student.displayInfo();

    if (student.hasScholarship()) {
        cout << "Студент получает стипендию" << endl;
    }
    else {
        cout << "Стипендия не назначена" << endl;
    }

    return 0;
}