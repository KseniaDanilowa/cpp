#include <iostream>
#include <string>
#include <vector>
#include <locale>
using namespace std;

class Book {
private:
    string title;
    string author;
    string isbn;
    int year;
    bool isAvailable;

public:
    Book(string t, string a, string i, int y) : title(t), author(a), isbn(i), year(y), isAvailable(true) {}

    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    string getIsbn() const { return isbn; }
    int getYear() const { return year; }
    bool getAvailable() const { return isAvailable; }

    void setTitle(string t) { title = t; }
    void setAuthor(string a) { author = a; }
    void setIsbn(string i) { isbn = i; }
    void setYear(int y) { year = y; }

    void borrow() {
        if (isAvailable) {
            isAvailable = false;
        }
        else {
            cout << "Книга уже занята." << endl;
        }
    }

    void returnBook() {
        if (!isAvailable) {
            isAvailable = true;
        }
        else {
            cout << "Книга уже возвращена." << endl;
        }
    }

    void displayInfo() const {
        cout << "Название: " << title << ", Автор: " << author << ", ISBN: " << isbn
            << ", Год: " << year << ", Доступна: " << (isAvailable ? "Да" : "Нет") << endl;
    }
};

class Library {
private:
    string name;
    string address;
    vector<Book> books;

public:
    Library(string n, string addr) : name(n), address(addr) {}

    void addBook(const Book& book) {
        books.push_back(book);
    }

    void removeBook(const string& isbn) {
        for (auto it = books.begin(); it != books.end(); ++it) {
            if (it->getIsbn() == isbn) {
                books.erase(it);
                cout << "Книга с ISBN " << isbn << " удалена." << endl;
                return;
            }
        }
        cout << "Книга с ISBN " << isbn << " не найдена." << endl;
    }

    const Book* findBook(const string& title) const {
        for (const auto& book : books) {
            if (book.getTitle() == title) {
                return &book;
            }
        }
        return nullptr;
    }

    void borrowBook(const string& isbn) {
        for (auto& book : books) {
            if (book.getIsbn() == isbn) {
                book.borrow();
                return;
            }
        }
        cout << "Книга с ISBN " << isbn << " не найдена." << endl;
    }

    void returnBook(const string& isbn) {
        for (auto& book : books) {
            if (book.getIsbn() == isbn) {
                book.returnBook();
                return;
            }
        }
        cout << "Книга с ISBN " << isbn << " не найдена." << endl;
    }

    void displayAllBooks() const {
        cout << "Все книги в библиотеке:" << endl;
        for (const auto& book : books) {
            book.displayInfo();
        }
    }

    void displayAvailableBooks() const {
        cout << "Доступные книги:" << endl;
        for (const auto& book : books) {
            if (book.getAvailable()) {
                book.displayInfo();
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "rus");

    Library lib("Центральная библиотека", "ул. Книжная, 1");

    lib.addBook(Book("Война и мир", "Л.Н. Толстой", "978-5-12345678-9", 1869));
    lib.addBook(Book("Преступление и наказание", "Ф.М. Достоевский", "978-5-98765432-1", 1866));

    lib.displayAllBooks();

    lib.borrowBook("978-5-12345678-9");
    lib.displayAvailableBooks();

    lib.returnBook("978-5-12345678-9");
    lib.displayAvailableBooks();

    return 0;
}