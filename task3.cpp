#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <locale>

#define M_PI 3.14159265358979323846

using namespace std;

class Shape {
protected:
    string color;
    string name;

public:
    Shape(string n, string c) : name(n), color(c) {}
    virtual ~Shape() {}

    virtual double calculateArea() const = 0;
    virtual double calculatePerimeter() const = 0;
    virtual void draw() const = 0;

    virtual void displayInfo() const {
        cout << "Фигура: " << name << ", Цвет: " << color << endl;
    }
};

class Circle : public Shape {
private:
    double radius;

public:
    Circle(string c, double r) : Shape("Круг", c), radius(r) {}

    double calculateArea() const override {
        return M_PI * radius * radius;
    }

    double calculatePerimeter() const override {
        return 2 * M_PI * radius;
    }

    void draw() const override {
        cout << "Отрисовка круга:" << endl;
        int r = static_cast<int>(radius);
        for (int y = -r; y <= r; ++y) {
            for (int x = -r; x <= r; ++x) {
                if (x * x + y * y <= r * r + r) {  // Приблизительное условие для круга
                    cout << "* ";
                }
                else {
                    cout << "  ";
                }
            }
            cout << endl;
        }
        cout << endl;
    }
};

class Rectangle : public Shape {
private:
    double width, height;

public:
    Rectangle(string c, double w, double h) : Shape("Прямоугольник", c), width(w), height(h) {}

    double calculateArea() const override {
        return width * height;
    }

    double calculatePerimeter() const override {
        return 2 * (width + height);
    }

    void draw() const override {
        cout << "Отрисовка прямоугольника:" << endl;
        int w = static_cast<int>(width);
        int h = static_cast<int>(height);
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                cout << "* ";
            }
            cout << endl;
        }
        cout << endl;
    }
};

class Triangle : public Shape {
private:
    double sideA, sideB, sideC;

public:
    Triangle(string c, double a, double b, double c_side) : Shape("Треугольник", c), sideA(a), sideB(b), sideC(c_side) {}

    double calculateArea() const override {
        double s = (sideA + sideB + sideC) / 2;
        return sqrt(s * (s - sideA) * (s - sideB) * (s - sideC));
    }

    double calculatePerimeter() const override {
        return sideA + sideB + sideC;
    }

    void draw() const override {
        cout << "Отрисовка треугольника:" << endl;
        int h = static_cast<int>(sideB); 
        for (int i = 1; i <= h; ++i) {
            for (int j = 1; j <= i; ++j) {
                cout << "* ";
            }
            cout << endl;
        }
        cout << endl;
    }
};

int main() {
    setlocale(LC_ALL, "rus");

    vector<Shape*> shapes;
    shapes.push_back(new Circle("Красный", 4.0));
    shapes.push_back(new Rectangle("Синий", 6.0, 4.0));
    shapes.push_back(new Triangle("Зеленый", 3.0, 4.0, 5.0));

    for (auto shape : shapes) {
        shape->displayInfo();
        cout << "Площадь: " << shape->calculateArea() << endl;
        cout << "Периметр: " << shape->calculatePerimeter() << endl;
        shape->draw();  
        cout << "------------------------" << endl;
    }

    for (auto shape : shapes) {
        delete shape;
    }

    return 0;
}