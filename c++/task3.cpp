#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>

using namespace std;

// Функция для выбора главного элемента в текущем столбце
int findPivot(const vector<vector<double>> &matrix, int col, int start_row)
{
    double max_val = 0.0;
    int pivot_row = start_row;

    for (int i = start_row; i < matrix.size(); i++)
    { // проходимся по текущей строке
        if (fabs(matrix[i][col]) > max_val)
        { // находим максимальный по модулю элемент в столбце проходясь по всем строкам
            max_val = fabs(matrix[i][col]);
            pivot_row = i; // проходимся по строке
        }
    }
    return pivot_row + 1; // возвращаем номер строки, начиная с 1
}

// Функция для решения системы линейных уравнений
vector<double> solveLinearSystem(const vector<vector<double>> &matrix)
{
    int n = matrix.size();
    int m = matrix[0].size() - 1;
    vector<vector<double>> working_matrix = matrix; // копия матрицы

    // Прямой ход
    cout << "Прямой ход:" << endl;
    for (int col = 0; col < m; col++)
    {
        int pivot_row = findPivot(working_matrix, col, col);     // находим номер строки
        double pivot_value = working_matrix[pivot_row - 1][col]; // находим главный элемент
        cout << "Выбран главный элемент " << pivot_value << " в строке " << pivot_row << endl;

        if (pivot_row != col + 1)
        {
            // меняем местами строки, если необходимо, так как галвные на диагонали
            swap(working_matrix[col], working_matrix[pivot_row - 1]); // используем индекс, начиная с 0
            cout << "Меняем местами строки " << col + 1 << " и " << pivot_row << endl;
        }

        // элементарные преобразования для получения нулей под главным элементом
        for (int i = col + 1; i < n; i++)
        {
            double factor = working_matrix[i][col] / working_matrix[col][col];
            cout << "Вычитаем из строки " << i + 1 << " " << factor << endl;

            for (int j = col; j < m + 1; j++)
            {
                working_matrix[i][j] -= factor * working_matrix[col][j];
            }
        }

        // выводим промежуточную матрицу
        cout << "Промежуточная матрица после " << col + 1 << "-го столбца:" << endl;
        for (const auto &row : working_matrix)
        {
            for (double elem : row)
            {
                if (fabs(elem) < 3e-17)
                {
                    cout << 0 << " ";
                }
                else
                {
                    cout << elem << " ";
                }
            }
            cout << endl;
        }
        cout << endl;
    }

    // Обратный ход
    cout << "Обратный ход:" << endl;
    vector<double> solution(m, 0.0);
    for (int i = n - 1; i >= 0; i--)
    {
        double sum = 0.0;
        for (int j = i + 1; j < m; j++)
        {
            sum += working_matrix[i][j] * solution[j];
        }
        solution[i] = (working_matrix[i][m] - sum) / working_matrix[i][i];
        cout << "x" << i + 1 << " = " << solution[i] << endl;
    }

    return solution;
}

void checkSpectralRadius(vector<vector<double>> &C)
{
    double spectralRadius = 0.0;
    for (int i = 0; i < 4; i++)
    {
        double sum = 0.0;
        for (int j = 0; j < 4; j++)
        {
            sum += fabs(C[i][j]);
        }
        if (sum > spectralRadius)
        {
            spectralRadius = sum;
        }
    }

    cout << "\nСпектральный радиус матрицы C: " << spectralRadius << endl;

    if (spectralRadius >= 1.0)
    {
        cout << "Условие сходимости метода простых итераций не выполняется." << endl;
    }
    else
    {
        cout << "Условие сходимости метода простых итераций выполняется." << endl;
    }
}

// Преобразование системы Ах=b к виду х=Сх+f
void formCanonicalSystem(const vector<vector<double>> &A, const vector<double> &b, vector<vector<double>> &C, vector<double> &f, int n)
{
    for (int i = 0; i < n; i++)
    {
        f[i] = b[i] / A[i][i];
        for (int j = 0; j < n; j++)
        {
            if (j != i)
            {
                C[i][j] = -A[i][j] / A[i][i];
            }
            else
            {
                C[i][j] = 0.0;
            }
        }
    }
}

// метод простых итераций
bool simpleIteration(const vector<vector<double>> &C, const vector<double> &f, vector<double> &x, int n, double epsilon)
{
    vector<double> xNew(n);
    int k = 0;
    double maxDiff = 0.0;
    int maxIteration = 10;

    // Задаем начальное приближение
    for (int i = 0; i < n; i++)
    {
        x[i] = 0.0;
    }

    cout << "N" << setw(10) << "x1" << setw(10) << "x2" << setw(10) << "x3" << setw(10) << "x4" << setw(10) << "εn" << endl;

    do
    {
        // Вычисляем новое приближение
        for (int i = 0; i < n; i++)
        {
            double sum = f[i];
            for (int j = 0; j < n; j++)
            {
                sum += C[i][j] * x[j];
            }
            xNew[i] = sum;
        }

        // Проверяем условие остановки
        maxDiff = 0.0;
        for (int i = 0; i < n; i++)
        {
            if (fabs(xNew[i] - x[i]) > maxDiff)
            {
                maxDiff = fabs(xNew[i] - x[i]);
            }
            x[i] = xNew[i];
        }
        k++;

        // Вывод результатов в таблицу
        cout << k << fixed << setprecision(3) << setw(10) << x[0] << setw(10) << x[1] << setw(10) << x[2] << setw(10) << x[3] << setw(10) << maxDiff << endl;
    } while (maxDiff > epsilon && k < maxIteration);

    cout << "Число итераций: " << k;
    if (k = 10)
    {
        cout << "(максимум)";
    }

    // Вывод сообщения о сходимости или расходимости
    if (maxDiff <= epsilon)
    {
        cout << "\nМетод сходится." << endl;
    }
    else
    {
        cout << "\nМетод расходится." << endl;
        return 1;
    }
    return 0;
}

int main()
{
    system("chcp 65001");

    vector<vector<double>> matrix = {
        {-1.13, -0.04, 0.21, -18.0, -1.24},
        {0.25, -1.23, 0.14, -0.09, 0.87},
        {-0.21, 0.14, 0.80, -0.13, 2.56},
        {0.15, -1.31, 0.06, 0.87, -1.13}};

    cout << "\tРешение СЛАУ с выбором главного элемента:\n"
         << endl;
    vector<double> solution = solveLinearSystem(matrix);
    cout << "Матрица решений СЛАУ: " << endl;
    for (double x : solution)
    {
        cout << x << " ";
    }
    cout << endl;

    vector<vector<double>> A = {
        {-1.13, -0.04, 0.21, -18.0},
        {0.25, -1.23, 0.14, -0.09},
        {-0.21, 0.14, 0.80, -0.13},
        {0.15, -1.31, 0.06, 0.87}};
    vector<double> b = {-1.24, 0.87, 2.56, -1.13};
    vector<double> x(4, 0.0);
    vector<vector<double>> C = {
        {0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0}};
    vector<double> f(4, 0.0);

    formCanonicalSystem(A, b, C, f, 4);
    checkSpectralRadius(C);

    cout << "\nРешение методом простых итераций:" << endl;
    simpleIteration(C, f, x, 4, 0.001);
    if (!simpleIteration)
    {
        cout << "x1 = " << fixed << setprecision(3) << x[0] << endl;
        cout << "x2 = " << fixed << setprecision(3) << x[1] << endl;
        cout << "x3 = " << fixed << setprecision(3) << x[2] << endl;
        cout << "x4 = " << fixed << setprecision(3) << x[3] << endl;
    }

    return 0;
}