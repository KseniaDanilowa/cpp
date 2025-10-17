#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

struct Date { int day, month, year; };
struct Student {
    string fio;
    Date   birth;
    int    marks[5];
};

// прототипы – чтобы main и printStudent знали о существовании функций 
double getAverageRating(const Student& s);
void   printStudent(const Student& s);


int main() {
    setlocale(LC_ALL, "Russian");

    Student group[4] = {
        {"Иванов Иван Иванович",     {12, 3, 2003}, {5, 5, 4, 5, 4}},
        {"Петрова Анна Сергеевна",   { 1, 9, 2002}, {4, 4, 4, 4, 4}},
        {"Сидоров Алексей Павлович", {30, 7, 2001}, {5, 5, 5, 5, 5}},
        {"Кузнецова Мария Андреевна",{18, 5, 2002}, {3, 4, 3, 4, 3}}
    };

    cout << "Список всех студентов:\n";
    for (int i = 0; i < 4; ++i) printStudent(group[i]);

    cout << "\nСтуденты со средним баллом выше 4.0:\n";
    for (int i = 0; i < 4; ++i)
        if (getAverageRating(group[i]) > 4.0)
            printStudent(group[i]);

    return 0;
}


double getAverageRating(const Student& s) {
    double sum = 0.0;
    for (int i = 0; i < 5; ++i) sum += s.marks[i];
    return sum / 5.0;
}


/*для "красивого" вывода даты:
setw - устанавливает ширину поля вывода; 
setfill - манипулятор, заполняющий число необходимым символом, если оно короче нужной ширины */ 

void printStudent(const Student& s) {
    cout << "ФИО: " << s.fio
        << ", дата рождения: " << setw(2) << setfill('0') << s.birth.day << '.'
        << setw(2) << setfill('0') << s.birth.month << '.'
        << s.birth.year << setfill(' ')
        << ", оценки: ";
    for (int i = 0; i < 5; ++i) cout << s.marks[i] << ' ';
    cout << "(ср. " << fixed << setprecision(2) << getAverageRating(s) << ')';
    cout << '\n';
}