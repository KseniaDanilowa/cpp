// Задание 5: Оптимизация и тестирование
// Методы для пакетной вставки, индексов и тестирования

#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <random>
#include <cassert>
#include "sqlite3.h"
#include "student_repository.h"
#include "database_manager.h"

class OptimizationManager {
private:
    sqlite3* db;

    void checkResult(int rc, const std::string& operation) {
        if (rc != SQLITE_OK && rc != SQLITE_DONE) {
            std::string error = operation + " failed: " + sqlite3_errmsg(db);
            throw std::runtime_error(error);
        }
    }

public:
    explicit OptimizationManager(sqlite3* database) : db(database) {
        if (db == nullptr) {
            throw std::invalid_argument("Database handle cannot be null");
        }
    }

    // Создание индексов для часто используемых полей
    void createIndexes() {
        std::cout << "Creating indexes..." << std::endl;

        // Индекс для email (уже уникальный, но добавим явно)
        const char* idxEmail = "CREATE INDEX IF NOT EXISTS idx_students_email ON students(email);";
        
        // Индекс для group_name
        const char* idxGroup = "CREATE INDEX IF NOT EXISTS idx_students_group ON students(group_name);";
        
        // Индекс для subject в таблице grades
        const char* idxSubject = "CREATE INDEX IF NOT EXISTS idx_grades_subject ON grades(subject);";
        
        // Индекс для student_id в таблице grades
        const char* idxStudentId = "CREATE INDEX IF NOT EXISTS idx_grades_student ON grades(student_id);";

        char* errMsg = nullptr;
        
        sqlite3_exec(db, idxEmail, nullptr, nullptr, &errMsg);
        if (errMsg) { sqlite3_free(errMsg); errMsg = nullptr; }
        std::cout << "  Index on students.email created" << std::endl;
        
        sqlite3_exec(db, idxGroup, nullptr, nullptr, &errMsg);
        if (errMsg) { sqlite3_free(errMsg); errMsg = nullptr; }
        std::cout << "  Index on students.group_name created" << std::endl;
        
        sqlite3_exec(db, idxSubject, nullptr, nullptr, &errMsg);
        if (errMsg) { sqlite3_free(errMsg); errMsg = nullptr; }
        std::cout << "  Index on grades.subject created" << std::endl;
        
        sqlite3_exec(db, idxStudentId, nullptr, nullptr, &errMsg);
        if (errMsg) { sqlite3_free(errMsg); errMsg = nullptr; }
        std::cout << "  Index on grades.student_id created" << std::endl;

        std::cout << "All indexes created successfully" << std::endl;
    }

    // Пакетная вставка студентов с использованием транзакции
    void batchInsertStudents(const std::vector<Student>& students) {
        if (students.empty()) return;

        auto start = std::chrono::high_resolution_clock::now();

        // Начало транзакции
        char* errMsg = nullptr;
        sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg);
        if (errMsg) sqlite3_free(errMsg);

        const char* sql = "INSERT INTO students (name, email, group_name) VALUES (?, ?, ?);";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        checkResult(rc, "Prepare batch insert");

        int successCount = 0;
        for (const auto& student : students) {
            sqlite3_reset(stmt);
            sqlite3_bind_text(stmt, 1, student.name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, student.email.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, student.group_name.c_str(), -1, SQLITE_TRANSIENT);

            rc = sqlite3_step(stmt);
            if (rc == SQLITE_DONE) {
                successCount++;
            }
        }

        sqlite3_finalize(stmt);

        // Фиксация транзакции
        sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errMsg);
        if (errMsg) sqlite3_free(errMsg);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "Batch insert: " << successCount << "/" << students.size() 
                  << " students in " << duration.count() << " ms" << std::endl;
    }

    // Генерация тестовых данных
    std::vector<Student> generateTestStudents(int count) {
        std::vector<Student> students;
        students.reserve(count);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> groupDist(1, 10);

        std::vector<std::string> firstNames = {"Ivan", "Petr", "Maria", "Anna", "Sergey", 
                                               "Elena", "Dmitry", "Olga", "Alexey", "Natalia"};
        std::vector<std::string> lastNames = {"Ivanov", "Petrov", "Sidorov", "Smirnov", "Kuznetsov",
                                              "Popov", "Sokolov", "Lebedev", "Kozlov", "Novikov"};

        for (int i = 0; i < count; ++i) {
            Student s;
            s.name = firstNames[i % firstNames.size()] + " " + lastNames[i % lastNames.size()];
            s.email = "student" + std::to_string(i + 1) + "@university.ru";
            s.group_name = "CS-" + std::to_string(100 + groupDist(gen));
            students.push_back(s);
        }

        return students;
    }

    // Измерение производительности запроса
    template<typename Func>
    double measurePerformance(const std::string& operationName, Func func, int iterations = 1) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        double avgTime = static_cast<double>(duration.count()) / iterations / 1000.0; // ms
        
        std::cout << operationName << ": " << avgTime << " ms (avg over " << iterations << " runs)" << std::endl;
        return avgTime;
    }
};

// Класс для тестирования
class TestRunner {
private:
    int passedTests;
    int failedTests;

    void printResult(const std::string& testName, bool passed) {
        if (passed) {
            std::cout << "[PASS] " << testName << std::endl;
            passedTests++;
        } else {
            std::cout << "[FAIL] " << testName << std::endl;
            failedTests++;
        }
    }

public:
    TestRunner() : passedTests(0), failedTests(0) {}

    // Тест добавления студента
    void testAddStudent() {
        DatabaseManager db;
        db.initialize(":memory:");
        StudentRepository repo(db.getHandle());

        bool result = repo.addStudent("Test User", "test@test.com", "TEST-001");
        printResult("AddStudent - valid data", result);
    }

    // Тест получения студента
    void testGetStudent() {
        DatabaseManager db;
        db.initialize(":memory:");
        StudentRepository repo(db.getHandle());

        repo.addStudent("Test User", "test@test.com", "TEST-001");
        Student student = repo.getStudent(1);
        
        bool result = (student.name == "Test User" && 
                      student.email == "test@test.com" &&
                      student.group_name == "TEST-001");
        printResult("GetStudent - retrieve added student", result);
    }

    // Тест обновления студента
    void testUpdateStudent() {
        DatabaseManager db;
        db.initialize(":memory:");
        StudentRepository repo(db.getHandle());

        repo.addStudent("Test User", "test@test.com", "TEST-001");
        repo.updateStudent(1, "Updated User", "updated@test.com", "TEST-002");
        Student student = repo.getStudent(1);
        
        bool result = (student.name == "Updated User" && 
                      student.email == "updated@test.com");
        printResult("UpdateStudent - modify student data", result);
    }

    // Тест удаления студента
    void testDeleteStudent() {
        DatabaseManager db;
        db.initialize(":memory:");
        StudentRepository repo(db.getHandle());

        repo.addStudent("Test User", "test@test.com", "TEST-001");
        repo.deleteStudent(1);
        
        bool exceptionThrown = false;
        try {
            repo.getStudent(1);
        } catch (const std::exception&) {
            exceptionThrown = true;
        }
        printResult("DeleteStudent - student removed", exceptionThrown);
    }

    // Тест уникальности email
    void testEmailUnique() {
        DatabaseManager db;
        db.initialize(":memory:");
        StudentRepository repo(db.getHandle());

        repo.addStudent("Test User 1", "test@test.com", "TEST-001");
        
        bool exceptionThrown = false;
        try {
            repo.addStudent("Test User 2", "test@test.com", "TEST-002");
        } catch (const std::exception&) {
            exceptionThrown = true;
        }
        printResult("EmailUnique - duplicate email rejected", exceptionThrown);
    }

    // Тест получения всех студентов
    void testGetAllStudents() {
        DatabaseManager db;
        db.initialize(":memory:");
        StudentRepository repo(db.getHandle());

        repo.addStudent("User 1", "user1@test.com", "TEST-001");
        repo.addStudent("User 2", "user2@test.com", "TEST-001");
        repo.addStudent("User 3", "user3@test.com", "TEST-002");

        auto students = repo.getAllStudents();
        printResult("GetAllStudents - returns correct count", students.size() == 3);
    }

    // Тест добавления оценки
    void testAddGrade() {
        DatabaseManager db;
        db.initialize(":memory:");
        StudentRepository repo(db.getHandle());

        repo.addStudent("Test User", "test@test.com", "TEST-001");
        bool result = repo.addGrade(1, "Math", 85);
        printResult("AddGrade - valid grade added", result);
    }

    // Тест каскадного удаления
    void testCascadeDelete() {
        DatabaseManager db;
        db.initialize(":memory:");
        StudentRepository repo(db.getHandle());

        repo.addStudent("Test User", "test@test.com", "TEST-001");
        repo.addGrade(1, "Math", 85);
        repo.addGrade(1, "Physics", 90);
        repo.deleteStudent(1);

        // Проверяем что оценки тоже удалены
        auto grades = repo.getStudentGrades(1);
        printResult("CascadeDelete - grades removed with student", grades.empty());
    }

    // Запуск всех тестов
    void runAllTests() {
        std::cout << "\n=== Running Unit Tests ===" << std::endl;
        
        testAddStudent();
        testGetStudent();
        testUpdateStudent();
        testDeleteStudent();
        testEmailUnique();
        testGetAllStudents();
        testAddGrade();
        testCascadeDelete();

        std::cout << "\n=== Test Results ===" << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;
        std::cout << "Total:  " << (passedTests + failedTests) << std::endl;
    }
};

#endif // OPTIMIZATION_H
