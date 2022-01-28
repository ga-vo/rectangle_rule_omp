//Gabriel Felipe Vega Ochoa - 2170122 

#include <iostream>
#include <math.h>
#include <omp.h>
#include <chrono>
#include <string.h>
#include <math.h>

using namespace std;
double a, b;
long n;

/**
 * Algoritmo de la regla de rectángulos para el cálculo de la integral Sin(x) entre los límites a y b
 * @param a Límite inferior de la integral
 * @param b Límite superior de la integral
 * @param n Número de iteraciones del algoritmo de la regla de los rectángulos
 * @param n_threads Número de hilos utilizados
 * @return double
 */
double integral(double a, double b, long n, int n_threads)
{
    double sum = 0;
    double delta = (b - a) / n;
    double k;

// Se utilizó el pragma paraller for con el fin de dividir las iteraciones del for entre hilos
/**
 * Se utilizó una reducción de sum con la operación de suma debido a que
 * todos los hilos escribirían esta variable. Con reduction se eliminan
 * los problemas de data race, pues cada hilo hace una reducción parcial
 * del valor operando, y al finalizar todos los hilos se combinan en la
 * variable inicial
 **/
#pragma parallel for reduction(+ \
                               : sum) num_threads(n_threads)
    for (long i = 1; i <= n; i++)
    {
        k = i - (1 / 2);
        sum += sin(a + k * delta) * delta;
    }
    return sum;
}

struct argsP
{
    double a;
    double b;
    double *sum;
};

int main(int argc, char *argv[])
{
    int abCount = 0;
    // Valor por defecto de iteraciones
    n = 10000000;
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (!strcmp(argv[i], "-a"))
            {
                try
                {
                    a = stod(argv[i + 1]);
                    abCount += 1;
                }
                catch (invalid_argument e)
                {
                    cout << "Please provide a valid number for a value " << argv[i + 1] << endl;
                    return 0;
                }
            }
            else if (!strcmp(argv[i], "-b"))
            {
                try
                {
                    b = stod(argv[i + 1]);
                    abCount += 1;
                }
                catch (invalid_argument e)
                {
                    cout << "Please provide a valid number for b value" << endl;
                    return 0;
                }
            }
            else if (!strcmp(argv[i], "-n"))
            {
                try
                {
                    n = stol(argv[i + 1]);
                }
                catch (invalid_argument e)
                {
                    cout << "Please provide a valid number for n value" << endl;
                    return 0;
                }
            }
            else if (!strcmp(argv[i], "--help"))
            {
                cout << "Use rectangleRule -a <limit inferior> -b <limit superior> -n <|n iterations>" << endl;
            }
        }
    }
    if (abCount != 2)
    {
        cout << "Please provide a and b value using -a and -b flags: rectangleRule -a 0 -b 3.1415926539" << endl;
        return 1;
    }
    cout << "a:" << a << " b:" << b << endl;

    cout << "Using " << omp_get_max_threads() << " threads" << endl;
    auto startPar = std::chrono::high_resolution_clock::now();
    double result = integral(a, b, n, omp_get_max_threads());
    auto stopPar = std::chrono::high_resolution_clock::now();
    auto durationPar = std::chrono::duration_cast<std::chrono::microseconds>(stopPar - startPar);
    cout << "result: " << result << endl;
    cout << "duración: " << durationPar.count() << " [uS]" << endl;

    // Establecer el número de hilos a 1 para comparar con n hilos
    omp_set_num_threads(1);

    cout << "\nUsing " << omp_get_max_threads() << " threads" << endl;
    auto startSin = std::chrono::high_resolution_clock::now();
    double result1 = integral(a, b, n, omp_get_max_threads());
    auto stopSin = std::chrono::high_resolution_clock::now();
    auto durationSin = std::chrono::duration_cast<std::chrono::microseconds>(stopSin - startSin);
    cout << "result: " << result1 << endl;
    cout << "duración: " << durationSin.count() << " [uS]" << endl;

    return 0;
}