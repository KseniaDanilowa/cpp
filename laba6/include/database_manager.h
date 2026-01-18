// Задание 1: Базовая настройка и создание таблиц
// Класс DatabaseManager для работы с базой данных SQLite

#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <stdexcept>
#include <iostream>
#include "sqlite3.h"

class DatabaseManager {
private:
    sqlite3* db;
    bool isConnected;

public:
    // Конструктор
    DatabaseManager() : db(nullptr), isConnected(false) {}

    // Деструктор - закрытие соединения
    ~DatabaseManager() {
        close();
    }

    // Запрет копирования
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    // Инициализация базы данных
    bool initialize(const std::string& dbPath) {
        // Открытие базы данных
        int rc = sqlite3_open(dbPath.c_str(), &db);
        if (rc != SQLITE_OK) {
            std::string error = "Cannot open database: ";
            error += sqlite3_errmsg(db);
            sqlite3_close(db);
            db = nullptr;
            throw std::runtime_error(error);
        }
        isConnected = true;

        // Базовая оптимизация базы данных
        optimizeDatabase();

        // Создание таблиц
        createTables();

        return true;
    }

    // Получение handle базы данных
    sqlite3* getHandle() const {
        if (!isConnected || db == nullptr) {
            throw std::runtime_error("Database not connected");
        }
        return db;
    }

    // Проверка подключения
    bool connected() const {
        return isConnected && db != nullptr;
    }

    // Закрытие соединения
    void close() {
        if (db != nullptr) {
            sqlite3_close(db);
            db = nullptr;
            isConnected = false;
        }
    }

    // Выполнение произвольного SQL запроса
    bool execute(const std::string& sql) {
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::string error = "SQL error: ";
            error += errMsg;
            sqlite3_free(errMsg);
            throw std::runtime_error(error);
        }
        return true;
    }

private:
    // Оптимизация настроек базы данных
    void optimizeDatabase() {
        // Включение внешних ключей
        execute("PRAGMA foreign_keys = ON;");
        
        // Режим журналирования WAL для лучшей производительности
        execute("PRAGMA journal_mode = WAL;");
        
        // Синхронный режим NORMAL для баланса скорости и надежности
        execute("PRAGMA synchronous = NORMAL;");
        
        // Увеличение размера кэша
        execute("PRAGMA cache_size = 10000;");
        
        // Хранение временных таблиц в памяти
        execute("PRAGMA temp_store = MEMORY;");
    }

    // Создание таблиц students и grades
    void createTables() {
        // Таблица студентов
        const char* createStudents = R"(
            CREATE TABLE IF NOT EXISTS students (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                email TEXT UNIQUE,
                group_name TEXT
            );
        )";

        // Таблица оценок с внешним ключом на students
        const char* createGrades = R"(
            CREATE TABLE IF NOT EXISTS grades (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                student_id INTEGER NOT NULL,
                subject TEXT NOT NULL,
                grade INTEGER NOT NULL CHECK(grade >= 0 AND grade <= 100),
                FOREIGN KEY(student_id) REFERENCES students(id) ON DELETE CASCADE
            );
        )";

        execute(createStudents);
        execute(createGrades);

        std::cout << "Tables 'students' and 'grades' created successfully" << std::endl;
    }
};

#endif // DATABASE_MANAGER_H
