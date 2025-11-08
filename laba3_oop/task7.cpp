#include <iostream>
#include <string>
#include <locale>
using namespace std;

class Complex {
private:
    double real;
    double imaginary;

public:
    Complex(double r = 0, double i = 0) : real(r), imaginary(i) {}

    double getReal() const { return real; }
    double getImaginary() const { return imaginary; }

    void setReal(double r) { real = r; }
    void setImaginary(double i) { imaginary = i; }

    Complex operator+(const Complex& other) const {
        return Complex(real + other.real, imaginary + other.imaginary);
    }

    Complex operator-(const Complex& other) const {
        return Complex(real - other.real, imaginary - other.imaginary);
    }

    Complex operator*(const Complex& other) const {
        double newReal = real * other.real - imaginary * other.imaginary;
        double newImaginary = real * other.imaginary + imaginary * other.real;
        return Complex(newReal, newImaginary);
    }

    bool operator==(const Complex& other) const {
        return (real == other.real) && (imaginary == other.imaginary);
    }

    friend ostream& operator<<(ostream& os, const Complex& c) {
        os << c.real << " + " << c.imaginary << "i";
        return os;
    }

    friend istream& operator>>(istream& is, Complex& c) {
        cout << "Введите действительную и мнимую части: ";
        is >> c.real >> c.imaginary;
        return is;
    }

    void display() const {
        cout << real << " + " << imaginary << "i" << endl;
    }
};

int main() {
    setlocale(LC_ALL, "rus");

    Complex c1(3, 4);
    Complex c2(1, 2);

    cout << "c1: " << c1 << endl;
    cout << "c2: " << c2 << endl;

    Complex sum = c1 + c2;
    cout << "c1 + c2 = " << sum << endl;

    Complex diff = c1 - c2;
    cout << "c1 - c2 = " << diff << endl;

    Complex prod = c1 * c2;
    cout << "c1 * c2 = " << prod << endl;

    cout << "c1 == c2? " << (c1 == c2 ? "Да" : "Нет") << endl;

    Complex c3;
    cin >> c3;
    cout << "Вы ввели: " << c3 << endl;

    return 0;
}