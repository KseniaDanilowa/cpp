#include <iostream>
#include <string>
using namespace std;

struct Book {
    string title;
    string author;
    int    year;
    double price;
};


void sortBooksByYear(Book* books, int size) {
    for (int i = 0; i < size - 1; ++i)
        for (int j = 0; j < size - i - 1; ++j)
            if ((books + j)->year > (books + j + 1)->year)   // доступ через указатель
                swap(*(books + j), *(books + j + 1));        // обмен структур
}


Book* findBookByAuthor(Book* books, int size, const string& author) {
    for (Book* p = books; p < books + size; ++p)
        if (p->author == author) return p;   // нашли – вернули указатель
    return nullptr;                          // специальный нулевой указатель "в никуда"
}


void printBook(const Book* b) {
    cout << "\"" << b->title << "\", " << b->author << ", " << b->year << ", " << b->price << " р.\n";
}


int main() {
    setlocale(LC_ALL, "Russian");

    // динамический массив из 5 книг (данные «вшиты»)
    const int SIZE = 5;
    Book* books = new Book[SIZE]{
        {"Война и мир",        "Л. Толстой", 1869, 899},
        {"Преступление и наказание", "Ф. Достоевский", 1866, 750},
        {"Мастер и Маргарита", "М. Булгаков", 1967, 620},
        {"Анна Каренина",      "Л. Толстой", 1877, 680},
        {"1984",               "Дж. Оруэлл", 1949, 500}
    };

    cout << "----- исходный список -----\n";
    for (Book* p = books; p < books + SIZE; ++p) printBook(p);

    sortBooksByYear(books, SIZE);
    cout << "\n----- после сортировки по году -----\n";
    for (Book* p = books; p < books + SIZE; ++p) printBook(p);

    string author = "Л. Толстой";
    Book* found = findBookByAuthor(books, SIZE, author);
    cout << "\nПоиск автора \"" << author << "\":\n";
    if (found) printBook(found);
    else       cout << "Не найдено.\n";

    delete[] books;  
    return 0;
}