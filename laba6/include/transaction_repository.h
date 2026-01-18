// Задание 3: Работа с транзакциями и сложными запросами
// Расширенные методы для транзакций и статистики

#ifndef TRANSACTION_REPOSITORY_H
#define TRANSACTION_REPOSITORY_H

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "sqlite3.h"
#include "student_repository.h"

// Структура для студента с оценками
struct StudentWithGrades {
    Student student;
    std::vector<Grade> grades;
    double averageGrade;

    StudentWithGrades() : averageGrade(0.0) {}
};

// Структура для топ-студента
struct TopStudent {
    int id;
    std::string name;
    std::string group_name;
    double averageGrade;
    int gradesCount;
};

class TransactionRepository {
private:
    sqlite3* db;

    void checkResult(int rc, const std::string& operation) {
        if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
            std::string error = operation + " failed: " + sqlite3_errmsg(db);
            throw std::runtime_error(error);
        }
    }

    // Начало транзакции
    void beginTransaction() {
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::string error = "Begin transaction failed: ";
            error += errMsg;
            sqlite3_free(errMsg);
            throw std::runtime_error(error);
        }
    }

    // Фиксация транзакции
    void commitTransaction() {
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::string error = "Commit failed: ";
            error += errMsg;
            sqlite3_free(errMsg);
            throw std::runtime_error(error);
        }
    }

    // Откат транзакции
    void rollbackTransaction() {
        char* errMsg = nullptr;
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, &errMsg);
        if (errMsg) sqlite3_free(errMsg);
    }

public:
    explicit TransactionRepository(sqlite3* database) : db(database) {
        if (db == nullptr) {
            throw std::invalid_argument("Database handle cannot be null");
        }
    }

    // Добавление студента с оценками в одной транзакции
    bool addStudentWithGrades(const std::string& name, const std::string& email,
                              const std::string& group_name, const std::vector<Grade>& grades) {
        try {
            beginTransaction();

            // Добавление студента
            const char* sqlStudent = "INSERT INTO students (name, email, group_name) VALUES (?, ?, ?);";
            sqlite3_stmt* stmt;

            int rc = sqlite3_prepare_v2(db, sqlStudent, -1, &stmt, nullptr);
            checkResult(rc, "Prepare student insert");

            sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, group_name.c_str(), -1, SQLITE_TRANSIENT);

            rc = sqlite3_step(stmt);
            sqlite3_finalize(stmt);

            if (rc != SQLITE_DONE) {
                throw std::runtime_error("Failed to add student");
            }

            int studentId = static_cast<int>(sqlite3_last_insert_rowid(db));

            // Добавление оценок
            const char* sqlGrade = "INSERT INTO grades (student_id, subject, grade) VALUES (?, ?, ?);";
            
            for (const auto& grade : grades) {
                rc = sqlite3_prepare_v2(db, sqlGrade, -1, &stmt, nullptr);
                checkResult(rc, "Prepare grade insert");

                sqlite3_bind_int(stmt, 1, studentId);
                sqlite3_bind_text(stmt, 2, grade.subject.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_int(stmt, 3, grade.grade);

                rc = sqlite3_step(stmt);
                sqlite3_finalize(stmt);

                if (rc != SQLITE_DONE) {
                    throw std::runtime_error("Failed to add grade");
                }
            }

            commitTransaction();
            std::cout << "Student '" << name << "' added with " << grades.size() 
                      << " grades (ID: " << studentId << ")" << std::endl;
            return true;

        } catch (const std::exception& e) {
            rollbackTransaction();
            throw std::runtime_error("Transaction failed: " + std::string(e.what()));
        }
    }

    // Получение студентов по группе с JOIN запросом
    std::vector<StudentWithGrades> getStudentsByGroup(const std::string& group_name) {
        std::vector<StudentWithGrades> result;

        const char* sql = R"(
            SELECT s.id, s.name, s.email, s.group_name,
                   g.id as grade_id, g.subject, g.grade
            FROM students s
            LEFT JOIN grades g ON s.id = g.student_id
            WHERE s.group_name = ?
            ORDER BY s.id, g.id;
        )";

        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        checkResult(rc, "Prepare getStudentsByGroup");

        sqlite3_bind_text(stmt, 1, group_name.c_str(), -1, SQLITE_TRANSIENT);

        int currentStudentId = -1;
        StudentWithGrades currentStudent;

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            int studentId = sqlite3_column_int(stmt, 0);

            if (studentId != currentStudentId) {
                if (currentStudentId != -1) {
                    // Вычисление среднего балла
                    if (!currentStudent.grades.empty()) {
                        double sum = 0;
                        for (const auto& g : currentStudent.grades) sum += g.grade;
                        currentStudent.averageGrade = sum / currentStudent.grades.size();
                    }
                    result.push_back(currentStudent);
                }

                currentStudentId = studentId;
                currentStudent = StudentWithGrades();
                currentStudent.student.id = studentId;
                currentStudent.student.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                
                const unsigned char* emailText = sqlite3_column_text(stmt, 2);
                if (emailText) currentStudent.student.email = reinterpret_cast<const char*>(emailText);
                
                const unsigned char* groupText = sqlite3_column_text(stmt, 3);
                if (groupText) currentStudent.student.group_name = reinterpret_cast<const char*>(groupText);
            }

            // Добавление оценки если есть
            if (sqlite3_column_type(stmt, 4) != SQLITE_NULL) {
                Grade g;
                g.id = sqlite3_column_int(stmt, 4);
                g.student_id = studentId;
                g.subject = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
                g.grade = sqlite3_column_int(stmt, 6);
                currentStudent.grades.push_back(g);
            }
        }

        // Добавление последнего студента
        if (currentStudentId != -1) {
            if (!currentStudent.grades.empty()) {
                double sum = 0;
                for (const auto& g : currentStudent.grades) sum += g.grade;
                currentStudent.averageGrade = sum / currentStudent.grades.size();
            }
            result.push_back(currentStudent);
        }

        sqlite3_finalize(stmt);
        return result;
    }

    // Получение средней оценки по предмету
    double getAverageGradeBySubject(const std::string& subject) {
        const char* sql = "SELECT AVG(grade) FROM grades WHERE subject = ?;";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        checkResult(rc, "Prepare getAverageGradeBySubject");

        sqlite3_bind_text(stmt, 1, subject.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        double avg = 0.0;

        if (rc == SQLITE_ROW && sqlite3_column_type(stmt, 0) != SQLITE_NULL) {
            avg = sqlite3_column_double(stmt, 0);
        }

        sqlite3_finalize(stmt);
        return avg;
    }

    // Получение топ-студентов по среднему баллу
    std::vector<TopStudent> getTopStudents(int limit) {
        std::vector<TopStudent> result;

        const char* sql = R"(
            SELECT s.id, s.name, s.group_name, 
                   AVG(g.grade) as avg_grade,
                   COUNT(g.id) as grades_count
            FROM students s
            INNER JOIN grades g ON s.id = g.student_id
            GROUP BY s.id
            HAVING grades_count > 0
            ORDER BY avg_grade DESC
            LIMIT ?;
        )";

        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        checkResult(rc, "Prepare getTopStudents");

        sqlite3_bind_int(stmt, 1, limit);

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            TopStudent ts;
            ts.id = sqlite3_column_int(stmt, 0);
            ts.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            
            const unsigned char* groupText = sqlite3_column_text(stmt, 2);
            if (groupText) ts.group_name = reinterpret_cast<const char*>(groupText);
            
            ts.averageGrade = sqlite3_column_double(stmt, 3);
            ts.gradesCount = sqlite3_column_int(stmt, 4);
            result.push_back(ts);
        }

        sqlite3_finalize(stmt);
        return result;
    }
};

#endif // TRANSACTION_REPOSITORY_H
