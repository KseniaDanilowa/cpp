#include <iostream>
using namespace std;


int main() {
    while (true) {
        setlocale(LC_ALL, "Russian");

        int N;
        cout << "Введите количество дней: ";
        cin >> N;
        if (N <= 0) break;

        double sum = 0.0;
        double* temperatures = new double[N];
        for (int i = 0; i < N; i++) {
            temperatures[i] = i;
            cin >> temperatures[i];
            sum += temperatures[i];
        }

        double avg_temp = sum / N;

        double min_temp = temperatures[0];
        double max_temp = temperatures[0];
        for (int i = 0; i < N; i++) {
            if (temperatures[i] < min_temp)
                min_temp = temperatures[i];
            if (temperatures[i] > max_temp)
                max_temp = temperatures[i];
        }

        int days_below_avg = 0;
        for (int i = 0; i < N; i++) {
            if (temperatures[i] < avg_temp)
                days_below_avg++;
        }

        cout << "\nСредняя температура за период = " << avg_temp;
        cout << "\nМинимальная температура = " << min_temp;
        cout << "\nМаксимальная температура = " << max_temp;
        cout << "\nКоличество дней, когда температура была ниже средней = " << days_below_avg << "\n";

        delete[] temperatures;
    }
    return 0;
}

