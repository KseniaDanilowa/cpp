#include <iostream>
using namespace std;

struct Complex {
    double real;   
    double imaginary;   
};


Complex add(Complex a, Complex b) {
    return { a.real + b.real, a.imaginary + b.imaginary };
}

Complex subtract(Complex a, Complex b) {
    return { a.real - b.real, a.imaginary - b.imaginary };
}

Complex multiply(Complex a, Complex b) {
    return { a.real * b.real - a.imaginary * b.imaginary,
            a.real * b.imaginary + a.imaginary * b.real };
}


void print(Complex c) {
    cout << '(' << c.real;
    if (c.imaginary >= 0) cout << " + " << c.imaginary << 'i';
    else           cout << " - " << -c.imaginary << 'i';
    cout << ')';
}


int main() {

    //Пример 1

    Complex x{ 3, 4 };   
    Complex y{ 1, -2 };  

    cout << "x = "; print(x); cout << '\n';
    cout << "y = "; print(y); cout << "\n\n";

    Complex s = add(x, y);
    cout << "x + y = "; print(s); cout << '\n';

    Complex d = subtract(x, y);
    cout << "x - y = "; print(d); cout << '\n';

    Complex p = multiply(x, y);
    cout << "x * y = "; print(p); cout << '\n';

    //Пример 2

    Complex a{ 2, -3 };  
    Complex b{ -2, 5 };  

    cout << "\na = "; print(a); cout << '\n';
    cout << "b = "; print(b); cout << "\n\n";

    cout << "a + b = "; print(add(a, b)); cout << '\n';
    cout << "a - b = "; print(subtract(a, b)); cout << '\n';
    cout << "a * b = "; print(multiply(a, b)); cout << '\n';

    return 0;
}