#include <iostream>
#include <string>
#include <vector>
#include <memory>
using namespace std;

class UniversityMember {
protected:
    string name;
    string id;
    string email;

public:
    UniversityMember(string n, string i, string e) : name(n), id(i), email(e) {}
    virtual ~UniversityMember() {}

    virtual void displayInfo() const = 0;
    virtual void work() const = 0;
    virtual string getRole() const = 0;
};

class Student : public UniversityMember {
private:
    string major;
    int year;
    double gpa;
    vector<string> courses;

public:
    Student(string n, string i, string e, string m, int y)
        : UniversityMember(n, i, e), major(m), year(y), gpa(0.0) {
    }

    void displayInfo() const override {
        cout << "Студент: " << name << " (" << id << ")" << endl;
        cout << "Специальность: " << major << ", Курс: " << year << endl;
        cout << "GPA: " << gpa << ", Курсов: " << courses.size() << endl;
    }

    void work() const override {
        cout << name << " учится и посещает лекции" << endl;
    }

    string getRole() const override { return "Студент"; }

    void enrollCourse(const string& course) {
        courses.push_back(course);
    }

    void dropCourse(const string& course) {
        for (auto it = courses.begin(); it != courses.end(); ++it) {
            if (*it == course) {
                courses.erase(it);
                break;
            }
        }
    }

    void calculateGPA() {
        // Упрощенный расчет GPA
        gpa = 3.0 + (courses.size() * 0.1);
    }
};

class Professor : public UniversityMember {
private:
    string department;
    string office;
    double salary;
    vector<string> teachingCourses;

public:
    Professor(string n, string i, string e, string dept, string off, double sal)
        : UniversityMember(n, i, e), department(dept), office(off), salary(sal) {
    }

    void displayInfo() const override {
        cout << "Профессор: " << name << " (" << id << ")" << endl;
        cout << "Кафедра: " << department << ", Кабинет: " << office << endl;
        cout << "Курсов: " << teachingCourses.size() << endl;
    }

    void work() const override {
        cout << name << " преподает и проводит исследования" << endl;
    }

    string getRole() const override { return "Профессор"; }

    void assignCourse(const string& course) {
        teachingCourses.push_back(course);
    }

    void removeCourse(const string& course) {
        for (auto it = teachingCourses.begin(); it != teachingCourses.end(); ++it) {
            if (*it == course) {
                teachingCourses.erase(it);
                break;
            }
        }
    }

    double calculateSalary() const {
        return salary + (teachingCourses.size() * 5000);
    }
};

class Course {
private:
    string courseCode;
    string courseName;
    int credits;
    Professor* instructor;
    vector<Student*> enrolledStudents;

public:
    Course(string code, string name, int cred)
        : courseCode(code), courseName(name), credits(cred), instructor(nullptr) {
    }

    void addStudent(Student* student) {
        enrolledStudents.push_back(student);
    }

    void removeStudent(const string& studentId) {
        for (auto it = enrolledStudents.begin(); it != enrolledStudents.end(); ++it) {
            if ((*it)->getRole() == "Студент") {
                enrolledStudents.erase(it);
                break;
            }
        }
    }

    void setInstructor(Professor* prof) {
        instructor = prof;
    }

    void displayCourseInfo() const {
        cout << "Курс: " << courseCode << " - " << courseName << endl;
        cout << "Кредиты: " << credits << endl;
        if (instructor) {
            cout << "Преподаватель: " << instructor->getRole() << endl;
        }
        cout << "Студентов: " << enrolledStudents.size() << endl;
    }
};

class University {
private:
    string name;
    vector<unique_ptr<UniversityMember>> members;
    vector<unique_ptr<Course>> courses;
    static int totalUniversities;

public:
    University(string n) : name(n) {
        totalUniversities++;
    }

    ~University() {
        totalUniversities--;
    }

    void addMember(unique_ptr<UniversityMember> member) {
        members.push_back(move(member));
    }

    void addCourse(unique_ptr<Course> course) {
        courses.push_back(move(course));
    }

    void displayAllMembers() const {
        cout << "Члены университета " << name << ":" << endl;
        for (const auto& member : members) {
            member->displayInfo();
            cout << "---" << endl;
        }
    }

    static int getTotalUniversities() {
        return totalUniversities;
    }
};

int University::totalUniversities = 0;

int main() {

    setlocale(LC_ALL, "rus");

    University uni("Технический Университет");

    auto student1 = make_unique<Student>("Иван", "S001", "ivan@edu.ru", "Информатика", 2);
    auto student2 = make_unique<Student>("Мария", "S002", "maria@edu.ru", "Математика", 1);
    auto professor = make_unique<Professor>("Доктор Смит", "P001", "smith@edu.ru", "Компьютерные науки", "A101", 80000);

    student1->enrollCourse("Программирование");
    student1->enrollCourse("Алгоритмы");
    student1->calculateGPA();

    professor->assignCourse("Программирование");
    professor->assignCourse("Базы данных");

    uni.addMember(move(student1));
    uni.addMember(move(student2));
    uni.addMember(move(professor));

    auto course = make_unique<Course>("CS101", "Программирование", 4);
    uni.addCourse(move(course));

    cout << "Университетов в системе: " << University::getTotalUniversities() << endl;
    uni.displayAllMembers();

    return 0;
}