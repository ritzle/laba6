#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<std::vector<T>> &matrix)
{
    for (const auto &row : matrix)
    {
        for (const auto &element : row)
        {
            os << element << "\t";
        }
        os << "\n";
    }
    return os;
}

template <typename T>
ostream &operator<<(std::ostream &os, const vector<T> &vector)
{

    for (const auto &element : vector)
    {
        os << element << " ";
    }

    return os;
}

pair<int, vector<int>> task1(vector<vector<int>> &matrix, int lines, int columns);

vector<int> task2(vector<vector<int>> &matrix);

void generateMatrix(vector<vector<int>> &matrix, int sizeMatrix);         // квадратная
void generateMatrix(vector<vector<int>> &matrix, int lines, int columns); // любая

int main()
{
    char action;
    int sizeMatrix;
    vector<vector<int>> matrix;
    pair<int, vector<int>> resultTask1;
    vector<int> resultTask2;

    cout << "1 2 3 E = exit" << endl;
    while (true)
    {
        cout << endl;
        cout << "Enter action: ";
        cin >> action;
        switch (action)
        {
        case '1':
            cout << "enter line and colums: ";
            int line, colum;
            cin >> line >> colum;
            matrix.resize(line, vector<int>(colum)); // Инициализируем матрицу
            generateMatrix(matrix, line, colum);
            cout << matrix;

            resultTask1 = task1(matrix, line, colum);
            cout << endl;

            cout << resultTask1.first << " --- " << resultTask1.second;
            cout << endl;

            break;

        case '2':
            cout << "enter size: ";
            cin >> sizeMatrix;
            matrix.resize(sizeMatrix, vector<int>(sizeMatrix)); // Инициализируем матрицу
            generateMatrix(matrix, sizeMatrix);
            cout << matrix;
            cout << endl;

            resultTask2 = task2(matrix);
            cout << resultTask2 << endl;

            break;

        case '3':
            cout << endl;
            break;

        case 'E':
            return 0;
        default:
            cout << "unknown team\n";
        }
    }
}

void generateMatrix(vector<vector<int>> &matrix, int sizeMatrix) // квадратная
{
    std::srand(std::time(nullptr));
    for (size_t i = 0; i < sizeMatrix; i++)
    {
        for (size_t j = 0; j < sizeMatrix; j++)
        {
            matrix[i][j] = 100 + std::rand() % 101;
        }
    }
}

void generateMatrix(vector<vector<int>> &matrix, int lines, int columns) // любая
{
    std::srand(std::time(nullptr));
    for (int i = 0; i < lines; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            matrix[i][j] = std::rand() % 101 - 50; // случайное число от -50 до 50
        }
    }
}

pair<int, vector<int>> task1(vector<vector<int>> &matrix, int lines, int columns)
{
    vector<int> numbers;

    int sum = 0;
    for (int i = 0; i < lines; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            if (i % 2 == 0 && j % 2 != 0 && matrix[i][j] > 0)
            {
                sum += matrix[i][j];
                numbers.push_back(matrix[i][j]);
            }
        }
    }

    return make_pair(sum, numbers);
}

vector<int> task2(vector<vector<int>> &matrix)
{
    sort(matrix.begin(), matrix.end(), [&](vector<int> vec1, vector<int> vec2)
         { return accumulate(vec1.begin(), vec1.end(), 0) > accumulate(vec2.begin(), vec2.end(), 0); });

    return matrix[1];
}
