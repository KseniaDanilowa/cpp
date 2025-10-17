#include <iostream>
#include <string>
using namespace std;

struct Book {
    string title;
    string author;
    int    year;
    double price;
};


void printBooks(Book* books, int size) {
    cout << "\n----- Список книг -----\n";
    for (int i = 0; i < size; ++i) {
        cout << "Книга #" << (i + 1) << ": "
            << books[i].title << ", "
            << books[i].author << ", "
            << books[i].year << ", "
            << books[i].price << " руб.\n";
    }
}


void printMostExpensive(Book* books, int size) {
    if (size <= 0) return;
    int mx = 0;
    for (int i = 1; i < size; ++i)
        if (books[i].price > books[mx].price) mx = i;

    cout << "\nСамая дорогая книга:\n"
        << books[mx].title << ", "
        << books[mx].author << ", "
        << books[mx].year << ", "
        << books[mx].price << " руб.\n";
}

int main() {
    
    setlocale(LC_ALL, "Russian");

    int n;
    cout << "Сколько книг в библиотеке? ";
    cin >> n;
    cin.ignore();         

    if (n <= 0) {
        cout << "Нет книг\n";
        return 0;
    }

    Book* library = new Book[n];

    for (int i = 0; i < n; ++i) {
        cout << "\nКнига #" << i + 1 << '\n';

        cout << " Название: ";
        getline(cin, library[i].title);

        cout << " Автор: ";
        getline(cin, library[i].author);

        cout << " Год издания: ";
        cin >> library[i].year;

        cout << " Цена (руб): ";
        cin >> library[i].price;
        cin.ignore();          
    }

    printBooks(library, n);
    printMostExpensive(library, n);

    delete[] library;
    return 0;
}