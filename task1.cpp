#include "playLife.h"
using namespace std;

template <typename T>
ostream &operator<<(ostream &os, const vector<std::vector<T>> &matrix)
{
    for (const auto &row : matrix)
    {
        for (const auto &element : row)
        {
            os << element << " ";
        }
        os << "\n";
    }
    return os;
}

template <typename T>
ostream &operator<<(ostream &os, const vector<T> &vector)
{
    for (const auto &element : vector)
    {
        os << element << " ";
    }
    return os;
}

pair<int, vector<int>> task1(vector<vector<int>> &matrix, int lines, int columns);
vector<int> task2(vector<vector<int>> &matrix);
void task3();

void generateMatrix(vector<vector<int>> &matrix, int sizeMatrix);
void generateMatrix(vector<vector<int>> &matrix, int lines, int columns);

int main()
{
    // SetConsoleOutputCP(65001);

    char action;
    int sizeMatrix;
    vector<vector<int>> matrix;
    pair<int, vector<int>> resultTask1;
    vector<int> resultTask2;

    cout << "1 2 3 q = exit" << endl;
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
            task3();
            cout << endl;
            break;

        case 'q':
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

void task3() // X ИГРА В ЖИЗНЬ
{
    char action;
    int sizeMatrix;
    pair<int, int> lineRow;

    int line = 101;
    int column = 151;

    vector<vector<int>> playingField(line, vector<int>(column, 0));

    cout << "1-random 2-train q = exit" << endl;
    cout << "Enter action: ";
    cin >> action;

    switch (action)
    {
    case '1':
        generatePlayingField(playingField, line, column);

        playLife(playingField, line, column);
        break;

    case '2':
        lineRow = readingPlayingField(playingField);

        // cout << playingField << endl;
        // cout << lineRow.first << " " << lineRow.second;
        playLife(playingField, lineRow.first, lineRow.second);
        break;

    case 'q':
        return;
    default:
        cout << "unknown team\n";
    }
}
