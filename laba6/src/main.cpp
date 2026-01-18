// Лабораторная работа: Работа с базами данных в C++
// Демонстрация всех заданий

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

#include "database_manager.h"
#include "student_repository.h"
#include "transaction_repository.h"
#include "input_validator.h"
#include "optimization.h"

// Функция для вывода разделителя
void printSeparator() {
    std::cout << std::string(60, '=') << std::endl;
}

// Задание 1: Демонстрация базовой настройки и создания таблиц
void runTask1(DatabaseManager& db) {
    std::cout << "=== Task 1: Database Setup and Table Creation ===" << std::endl;
    std::cout << "Database initialized with optimizations:" << std::endl;
    std::cout << "  - Foreign keys enabled" << std::endl;
    std::cout << "  - WAL journal mode" << std::endl;
    std::cout << "  - Normal synchronous mode" << std::endl;
    std::cout << "  - Increased cache size" << std::endl;
    std::cout << "  - Memory temp store" << std::endl;
    std::cout << std::endl;
}

// Задание 2: Демонстрация CRUD операций
void runTask2(DatabaseManager& db) {
    std::cout << "=== Task 2: CRUD Operations ===" << std::endl;
    
    StudentRepository repo(db.getHandle());
    
    // CREATE - добавление студентов
    std::cout << "\n--- Adding students ---" << std::endl;
    repo.addStudent("Ivan Ivanov", "ivan@university.ru", "CS-101");
    repo.addStudent("Maria Petrova", "maria@university.ru", "CS-101");
    repo.addStudent("Sergey Sidorov", "sergey@university.ru", "CS-102");
    
    // READ - получение студента
    std::cout << "\n--- Getting student by ID ---" << std::endl;
    Student student = repo.getStudent(1);
    std::cout << "Student ID 1: " << student.name << " (" << student.email << ")" << std::endl;
    
    // READ - получение всех студентов
    std::cout << "\n--- All students ---" << std::endl;
    auto students = repo.getAllStudents();
    for (const auto& s : students) {
        std::cout << "  ID: " << s.id << ", Name: " << s.name 
                  << ", Group: " << s.group_name << std::endl;
    }
    
    // UPDATE - обновление студента
    std::cout << "\n--- Updating student ---" << std::endl;
    repo.updateStudent(1, "Ivan Ivanovich Ivanov", "ivan.ivanov@university.ru", "CS-101");
    student = repo.getStudent(1);
    std::cout << "Updated student: " << student.name << " (" << student.email << ")" << std::endl;
    
    // Добавление оценок
    std::cout << "\n--- Adding grades ---" << std::endl;
    repo.addGrade(1, "Math", 85);
    repo.addGrade(1, "Physics", 90);
    repo.addGrade(2, "Math", 95);
    repo.addGrade(2, "Physics", 88);
    repo.addGrade(3, "Math", 78);
    std::cout << "Grades added successfully" << std::endl;
    
    // DELETE - удаление студента (закомментировано для сохранения данных)
    // std::cout << "\n--- Deleting student ---" << std::endl;
    // repo.deleteStudent(3);
    
    std::cout << std::endl;
}

// Задание 3: Демонстрация транзакций и сложных запросов
void runTask3(DatabaseManager& db) {
    std::cout << "=== Task 3: Transactions and Complex Queries ===" << std::endl;
    
    TransactionRepository transRepo(db.getHandle());
    
    // Добавление студента с оценками в одной транзакции
    std::cout << "\n--- Adding student with grades (transaction) ---" << std::endl;
    std::vector<Grade> grades = {
        Grade("Math", 92),
        Grade("Physics", 88),
        Grade("Programming", 95)
    };
    transRepo.addStudentWithGrades("Petr Petrov", "petr@university.ru", "CS-101", grades);
    
    // Получение студентов по группе
    std::cout << "\n--- Students in group CS-101 ---" << std::endl;
    auto groupStudents = transRepo.getStudentsByGroup("CS-101");
    for (const auto& sw : groupStudents) {
        std::cout << "  " << sw.student.name << " - Avg grade: " 
                  << std::fixed << std::setprecision(2) << sw.averageGrade << std::endl;
    }
    
    // Средняя оценка по предмету
    std::cout << "\n--- Average grade by subject ---" << std::endl;
    double avgMath = transRepo.getAverageGradeBySubject("Math");
    double avgPhysics = transRepo.getAverageGradeBySubject("Physics");
    std::cout << "  Math: " << std::fixed << std::setprecision(2) << avgMath << std::endl;
    std::cout << "  Physics: " << std::fixed << std::setprecision(2) << avgPhysics << std::endl;
    
    // Топ студентов
    std::cout << "\n--- Top 5 students ---" << std::endl;
    auto topStudents = transRepo.getTopStudents(5);
    int rank = 1;
    for (const auto& ts : topStudents) {
        std::cout << "  " << rank++ << ". " << ts.name 
                  << " (Group: " << ts.group_name << ") - Avg: " 
                  << std::fixed << std::setprecision(2) << ts.averageGrade 
                  << " (" << ts.gradesCount << " grades)" << std::endl;
    }
    
    std::cout << std::endl;
}

// Задание 4: Демонстрация валидации данных
void runTask4() {
    std::cout << "=== Task 4: Input Validation and Security ===" << std::endl;
    
    InputValidator validator;
    
    // Тестирование валидации email
    std::cout << "\n--- Email validation ---" << std::endl;
    std::vector<std::string> emails = {
        "valid@email.com",
        "invalid.email",
        "test@domain.ru",
        "@nodomain.com",
        "noemail"
    };
    
    for (const auto& email : emails) {
        std::cout << "  '" << email << "' - " 
                  << (validator.isValidEmail(email) ? "VALID" : "INVALID") << std::endl;
    }
    
    // Тестирование проверки на SQL-инъекции
    std::cout << "\n--- SQL injection detection ---" << std::endl;
    std::vector<std::string> inputs = {
        "Normal text",
        "John Doe",
        "'; DROP TABLE students;--",
        "1 OR 1=1",
        "Robert'); DELETE FROM users;--",
        "Test User"
    };
    
    for (const auto& input : inputs) {
        std::cout << "  '" << input << "' - " 
                  << (validator.isSafeInput(input) ? "SAFE" : "DANGEROUS") << std::endl;
    }
    
    // Тестирование валидации оценок
    std::cout << "\n--- Grade validation ---" << std::endl;
    std::vector<int> gradeValues = {-10, 0, 50, 100, 150};
    for (int grade : gradeValues) {
        std::cout << "  Grade " << grade << " - " 
                  << (validator.isValidGrade(grade) ? "VALID" : "INVALID") << std::endl;
    }
    
    // Пример использования валидации с исключениями
    std::cout << "\n--- Exception handling example ---" << std::endl;
    try {
        validator.validateEmail("invalid-email");
    } catch (const std::invalid_argument& e) {
        std::cout << "  Caught exception: " << e.what() << std::endl;
    }
    
    try {
        validator.validateGrade(150);
    } catch (const std::invalid_argument& e) {
        std::cout << "  Caught exception: " << e.what() << std::endl;
    }
    
    std::cout << std::endl;
}

// Задание 5: Демонстрация оптимизации и тестирования
void runTask5(DatabaseManager& db) {
    std::cout << "=== Task 5: Optimization and Testing ===" << std::endl;
    
    OptimizationManager optManager(db.getHandle());
    StudentRepository repo(db.getHandle());
    
    // Создание индексов
    std::cout << "\n--- Creating indexes ---" << std::endl;
    optManager.createIndexes();
    
    // Пакетная вставка
    std::cout << "\n--- Batch insert performance test ---" << std::endl;
    auto testStudents = optManager.generateTestStudents(1000);
    optManager.batchInsertStudents(testStudents);
    
    // Измерение производительности запросов
    std::cout << "\n--- Query performance ---" << std::endl;
    
    optManager.measurePerformance("GetAllStudents", [&repo]() {
        auto students = repo.getAllStudents();
    }, 10);
    
    TransactionRepository transRepo(db.getHandle());
    optManager.measurePerformance("GetTopStudents(10)", [&transRepo]() {
        auto top = transRepo.getTopStudents(10);
    }, 10);
    
    optManager.measurePerformance("GetStudentsByGroup", [&transRepo]() {
        auto students = transRepo.getStudentsByGroup("CS-101");
    }, 10);
    
    // Запуск unit-тестов
    std::cout << std::endl;
    TestRunner testRunner;
    testRunner.runAllTests();
    
    std::cout << std::endl;
}

// Главное меню
void showMenu() {
    std::cout << "\nSelect task to run:" << std::endl;
    std::cout << "1. Task 1: Database Setup and Table Creation" << std::endl;
    std::cout << "2. Task 2: CRUD Operations" << std::endl;
    std::cout << "3. Task 3: Transactions and Complex Queries" << std::endl;
    std::cout << "4. Task 4: Input Validation and Security" << std::endl;
    std::cout << "5. Task 5: Optimization and Testing" << std::endl;
    std::cout << "6. Run all tasks" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Your choice: ";
}

int main() {
    std::cout << "Lab Work: Working with Databases in C++" << std::endl;
    printSeparator();
    
    // Инициализация базы данных
    DatabaseManager db;
    try {
        db.initialize("university.db");
        std::cout << "Database 'university.db' initialized successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize database: " << e.what() << std::endl;
        return 1;
    }
    
    int choice;
    do {
        showMenu();
        std::cin >> choice;
        std::cout << std::endl;
        
        try {
            switch (choice) {
                case 1:
                    printSeparator();
                    runTask1(db);
                    printSeparator();
                    break;
                case 2:
                    printSeparator();
                    runTask2(db);
                    printSeparator();
                    break;
                case 3:
                    printSeparator();
                    runTask3(db);
                    printSeparator();
                    break;
                case 4:
                    printSeparator();
                    runTask4();
                    printSeparator();
                    break;
                case 5:
                    printSeparator();
                    runTask5(db);
                    printSeparator();
                    break;
                case 6:
                    // Для полного запуска пересоздаем БД
                    db.close();
                    std::remove("university.db");
                    db.initialize("university.db");
                    
                    printSeparator();
                    runTask1(db);
                    printSeparator();
                    std::cout << std::endl;
                    
                    printSeparator();
                    runTask2(db);
                    printSeparator();
                    std::cout << std::endl;
                    
                    printSeparator();
                    runTask3(db);
                    printSeparator();
                    std::cout << std::endl;
                    
                    printSeparator();
                    runTask4();
                    printSeparator();
                    std::cout << std::endl;
                    
                    printSeparator();
                    runTask5(db);
                    printSeparator();
                    break;
                case 0:
                    std::cout << "Exiting..." << std::endl;
                    break;
                default:
                    std::cout << "Invalid choice. Try again." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        
    } while (choice != 0);
    
    return 0;
}