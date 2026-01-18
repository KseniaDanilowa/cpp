// Задание 4: Безопасность и валидация данных
// Класс InputValidator для проверки входных данных

#ifndef INPUT_VALIDATOR_H
#define INPUT_VALIDATOR_H

#include <string>
#include <regex>
#include <stdexcept>
#include <algorithm>
#include <cctype>

class InputValidator {
private:
    // Максимальные длины полей
    static const size_t MAX_NAME_LENGTH = 100;
    static const size_t MAX_EMAIL_LENGTH = 255;
    static const size_t MAX_GROUP_LENGTH = 50;
    static const size_t MAX_SUBJECT_LENGTH = 100;

    // Минимальная оценка
    static const int MIN_GRADE = 0;
    // Максимальная оценка
    static const int MAX_GRADE = 100;

public:
    // Проверка корректности email
    bool isValidEmail(const std::string& email) const {
        if (email.empty()) {
            return false;
        }

        if (email.length() > MAX_EMAIL_LENGTH) {
            return false;
        }

        // Простая проверка формата email с помощью regex
        // Формат: username@domain.extension
        std::regex emailPattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
        return std::regex_match(email, emailPattern);
    }

    // Проверка длины имени
    bool isValidName(const std::string& name) const {
        if (name.empty()) {
            return false;
        }
        if (name.length() > MAX_NAME_LENGTH) {
            return false;
        }
        // Имя должно содержать хотя бы одну букву
        return std::any_of(name.begin(), name.end(), ::isalpha);
    }

    // Проверка длины названия группы
    bool isValidGroupName(const std::string& group) const {
        if (group.empty()) {
            return false;
        }
        return group.length() <= MAX_GROUP_LENGTH;
    }

    // Проверка названия предмета
    bool isValidSubject(const std::string& subject) const {
        if (subject.empty()) {
            return false;
        }
        return subject.length() <= MAX_SUBJECT_LENGTH;
    }

    // Проверка диапазона оценки (0-100)
    bool isValidGrade(int grade) const {
        return grade >= MIN_GRADE && grade <= MAX_GRADE;
    }

    // Проверка на отсутствие SQL-инъекций
    bool isSafeInput(const std::string& input) const {
        // Список потенциально опасных SQL ключевых слов и символов
        std::vector<std::string> dangerousPatterns = {
            "--",           // SQL комментарий
            ";",            // Конец запроса
            "/*",           // Начало блочного комментария
            "*/",           // Конец блочного комментария
            "DROP",         // Удаление таблицы
            "DELETE",       // Удаление данных
            "INSERT",       // Вставка данных
            "UPDATE",       // Обновление данных
            "SELECT",       // Выборка данных
            "UNION",        // Объединение запросов
            "OR 1=1",       // Типичная инъекция
            "' OR '",       // Типичная инъекция
            "\" OR \"",     // Типичная инъекция
            "EXEC",         // Выполнение команды
            "EXECUTE",      // Выполнение команды
            "xp_",          // Системные процедуры
            "sp_"           // Хранимые процедуры
        };

        // Преобразуем в верхний регистр для проверки
        std::string upperInput = input;
        std::transform(upperInput.begin(), upperInput.end(), upperInput.begin(), ::toupper);

        for (const auto& pattern : dangerousPatterns) {
            std::string upperPattern = pattern;
            std::transform(upperPattern.begin(), upperPattern.end(), upperPattern.begin(), ::toupper);
            
            if (upperInput.find(upperPattern) != std::string::npos) {
                return false;
            }
        }

        return true;
    }

    // Валидация имени с выбросом исключения
    void validateName(const std::string& name) const {
        if (!isValidName(name)) {
            throw std::invalid_argument("Invalid name: must be 1-" + 
                std::to_string(MAX_NAME_LENGTH) + " characters and contain letters");
        }
        if (!isSafeInput(name)) {
            throw std::invalid_argument("Potential SQL injection detected in name");
        }
    }

    // Валидация email с выбросом исключения
    void validateEmail(const std::string& email) const {
        if (!isValidEmail(email)) {
            throw std::invalid_argument("Invalid email format");
        }
        if (!isSafeInput(email)) {
            throw std::invalid_argument("Potential SQL injection detected in email");
        }
    }

    // Валидация группы с выбросом исключения
    void validateGroupName(const std::string& group) const {
        if (!isValidGroupName(group)) {
            throw std::invalid_argument("Invalid group name: must be 1-" + 
                std::to_string(MAX_GROUP_LENGTH) + " characters");
        }
        if (!isSafeInput(group)) {
            throw std::invalid_argument("Potential SQL injection detected in group name");
        }
    }

    // Валидация предмета с выбросом исключения
    void validateSubject(const std::string& subject) const {
        if (!isValidSubject(subject)) {
            throw std::invalid_argument("Invalid subject: must be 1-" + 
                std::to_string(MAX_SUBJECT_LENGTH) + " characters");
        }
        if (!isSafeInput(subject)) {
            throw std::invalid_argument("Potential SQL injection detected in subject");
        }
    }

    // Валидация оценки с выбросом исключения
    void validateGrade(int grade) const {
        if (!isValidGrade(grade)) {
            throw std::invalid_argument("Invalid grade: must be between " + 
                std::to_string(MIN_GRADE) + " and " + std::to_string(MAX_GRADE));
        }
    }

    // Полная валидация данных студента
    void validateStudentData(const std::string& name, const std::string& email, 
                            const std::string& group) const {
        validateName(name);
        validateEmail(email);
        validateGroupName(group);
    }

    // Полная валидация данных оценки
    void validateGradeData(const std::string& subject, int grade) const {
        validateSubject(subject);
        validateGrade(grade);
    }
};

#endif // INPUT_VALIDATOR_H
