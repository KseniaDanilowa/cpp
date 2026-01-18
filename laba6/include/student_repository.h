// Задание 2: CRUD операции и подготовленные выражения
// Класс StudentRepository для работы со студентами

#ifndef STUDENT_REPOSITORY_H
#define STUDENT_REPOSITORY_H

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "sqlite3.h"

// Структура для хранения данных студента
struct Student {
    int id;
    std::string name;
    std::string email;
    std::string group_name;

    Student() : id(0) {}
    Student(int id, const std::string& name, const std::string& email, const std::string& group_name)
        : id(id), name(name), email(email), group_name(group_name) {}
};

// Структура для хранения оценки
struct Grade {
    int id;
    int student_id;
    std::string subject;
    int grade;

    Grade() : id(0), student_id(0), grade(0) {}
    Grade(const std::string& subject, int grade) 
        : id(0), student_id(0), subject(subject), grade(grade) {}
    Grade(int id, int student_id, const std::string& subject, int grade)
        : id(id), student_id(student_id), subject(subject), grade(grade) {}
};

class StudentRepository {
private:
    sqlite3* db;

    // Вспомогательный метод для проверки результата
    void checkResult(int rc, const std::string& operation) {
        if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
            std::string error = operation + " failed: " + sqlite3_errmsg(db);
            throw std::runtime_error(error);
        }
    }

public:
    // Конструктор
    explicit StudentRepository(sqlite3* database) : db(database) {
        if (db == nullptr) {
            throw std::invalid_argument("Database handle cannot be null");
        }
    }

    // Добавление студента
    bool addStudent(const std::string& name, const std::string& email, const std::string& group_name) {
        const char* sql = "INSERT INTO students (name, email, group_name) VALUES (?, ?, ?);";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        checkResult(rc, "Prepare addStudent");

        // Привязка параметров
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, group_name.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc == SQLITE_CONSTRAINT) {
            throw std::runtime_error("Email already exists: " + email);
        }
        
        if (rc != SQLITE_DONE) {
            throw std::runtime_error("Failed to add student: " + std::string(sqlite3_errmsg(db)));
        }

        std::cout << "Student '" << name << "' added with ID: " << sqlite3_last_insert_rowid(db) << std::endl;
        return true;
    }

    // Получение студента по ID
    Student getStudent(int id) {
        const char* sql = "SELECT id, name, email, group_name FROM students WHERE id = ?;";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        checkResult(rc, "Prepare getStudent");

        sqlite3_bind_int(stmt, 1, id);

        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            Student student;
            student.id = sqlite3_column_int(stmt, 0);
            student.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            
            const unsigned char* emailText = sqlite3_column_text(stmt, 2);
            if (emailText) student.email = reinterpret_cast<const char*>(emailText);
            
            const unsigned char* groupText = sqlite3_column_text(stmt, 3);
            if (groupText) student.group_name = reinterpret_cast<const char*>(groupText);

            sqlite3_finalize(stmt);
            return student;
        }

        sqlite3_finalize(stmt);
        throw std::runtime_error("Student not found with ID: " + std::to_string(id));
    }

    // Обновление студента
    bool updateStudent(int id, const std::string& newName, const std::string& newEmail, const std::string& newGroup) {
        const char* sql = "UPDATE students SET name = ?, email = ?, group_name = ? WHERE id = ?;";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        checkResult(rc, "Prepare updateStudent");

        sqlite3_bind_text(stmt, 1, newName.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, newEmail.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, newGroup.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 4, id);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc == SQLITE_CONSTRAINT) {
            throw std::runtime_error("Email already exists: " + newEmail);
        }

        if (rc != SQLITE_DONE) {
            throw std::runtime_error("Failed to update student: " + std::string(sqlite3_errmsg(db)));
        }

        int changes = sqlite3_changes(db);
        if (changes == 0) {
            throw std::runtime_error("Student not found with ID: " + std::to_string(id));
        }

        std::cout << "Student ID " << id << " updated successfully" << std::endl;
        return true;
    }

    // Удаление студента
    bool deleteStudent(int id) {
        const char* sql = "DELETE FROM students WHERE id = ?;";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        checkResult(rc, "Prepare deleteStudent");

        sqlite3_bind_int(stmt, 1, id);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc != SQLITE_DONE) {
            throw std::runtime_error("Failed to delete student: " + std::string(sqlite3_errmsg(db)));
        }

        int changes = sqlite3_changes(db);
        if (changes == 0) {
            throw std::runtime_error("Student not found with ID: " + std::to_string(id));
        }

        std::cout << "Student ID " << id << " deleted successfully" << std::endl;
        return true;
    }

    // Получение всех студентов
    std::vector<Student> getAllStudents() {
        std::vector<Student> students;
        const char* sql = "SELECT id, name, email, group_name FROM students ORDER BY id;";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        checkResult(rc, "Prepare getAllStudents");

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            Student student;
            student.id = sqlite3_column_int(stmt, 0);
            student.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            
            const unsigned char* emailText = sqlite3_column_text(stmt, 2);
            if (emailText) student.email = reinterpret_cast<const char*>(emailText);
            
            const unsigned char* groupText = sqlite3_column_text(stmt, 3);
            if (groupText) student.group_name = reinterpret_cast<const char*>(groupText);

            students.push_back(student);
        }

        sqlite3_finalize(stmt);
        return students;
    }

    // Добавление оценки студенту
    bool addGrade(int student_id, const std::string& subject, int grade) {
        const char* sql = "INSERT INTO grades (student_id, subject, grade) VALUES (?, ?, ?);";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        checkResult(rc, "Prepare addGrade");

        sqlite3_bind_int(stmt, 1, student_id);
        sqlite3_bind_text(stmt, 2, subject.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, grade);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc == SQLITE_CONSTRAINT) {
            throw std::runtime_error("Foreign key constraint failed - student does not exist");
        }

        if (rc != SQLITE_DONE) {
            throw std::runtime_error("Failed to add grade: " + std::string(sqlite3_errmsg(db)));
        }

        return true;
    }

    // Получение оценок студента
    std::vector<Grade> getStudentGrades(int student_id) {
        std::vector<Grade> grades;
        const char* sql = "SELECT id, student_id, subject, grade FROM grades WHERE student_id = ?;";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        checkResult(rc, "Prepare getStudentGrades");

        sqlite3_bind_int(stmt, 1, student_id);

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            Grade g;
            g.id = sqlite3_column_int(stmt, 0);
            g.student_id = sqlite3_column_int(stmt, 1);
            g.subject = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            g.grade = sqlite3_column_int(stmt, 3);
            grades.push_back(g);
        }

        sqlite3_finalize(stmt);
        return grades;
    }
};

#endif // STUDENT_REPOSITORY_H
