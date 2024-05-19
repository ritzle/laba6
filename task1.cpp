#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <ncurses.h>
#include <unistd.h>

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

void generateMatrix(vector<vector<int>> &matrix, int sizeMatrix);
void generateMatrix(vector<vector<int>> &matrix, int lines, int columns);

void generatePlayingField(vector<vector<int>> &matrix, int lines, int columns);
void task3();
void fieldDisplay(const vector<std::vector<int>> &matrix, int lines, int columns);
int cellCounting(vector<vector<int>> &matrix, int i, int j, int lines, int columns);
void updateMatrix(vector<vector<int>> &matrix, int lines, int columns);
void playLife(vector<vector<int>> &matrix, int lines, int columns);

int main()
{
    // SetConsoleOutputCP(65001);

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
            task3();
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

void task3() // X ИГРА В ЖИЗНЬ
{
    char action;
    int sizeMatrix;

    int line = 50;
    int column = 50;

    vector<vector<int>> playingField(line, vector<int>(column, 0));

    cout << "1-random 2-train 3 E = exit" << endl;
    cout << "Enter action: ";
    cin >> action;

    switch (action)
    {
    case '1':
        generatePlayingField(playingField, line, column);

        playLife(playingField, line, column);
        break;

    case '2':
        break;

    case 'E':
        return;
    default:
        cout << "unknown team\n";
    }
}

void generatePlayingField(vector<vector<int>> &matrix, int lines, int columns)
{
    std::srand(time(nullptr));
    for (int i = 0; i < lines; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            matrix[i][j] = rand() % 2; // 0 или 1
        }
    }
}

void playLife(vector<vector<int>> &matrix, int lines, int columns)
{
    initscr();   // Инициализация ncurses
    curs_set(0); // Скрываем курсор
    cbreak();    // Режим постоянного ввода
    noecho();    // Отключаем вывод введенных символов

    while (true)
    {
        fieldDisplay(matrix, lines, columns);
        updateMatrix(matrix, lines, columns);

        refresh();      // Обновляем экран
        usleep(100000); // Задержка 100 мс
    }

    endwin(); // Завершаем работу с ncurses
}

void updateMatrix(vector<vector<int>> &matrix, int lines, int columns)
{
    vector<vector<int>> newMatrix(lines, vector<int>(columns, 0)); // Инициализируем newMatrix

    for (int i = 0; i < lines; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            int numbeLivingCells = cellCounting(matrix, i, j, lines, columns);

            if (matrix[i][j] == 1)
            { // Если клетка живая
                if (numbeLivingCells < 2 || numbeLivingCells > 3)
                { // Умирает от одиночества или перенаселения
                    newMatrix[i][j] = 0;
                }
                else
                { // Остается живой
                    newMatrix[i][j] = 1;
                }
            }
            else
            { // Если клетка мертвая
                if (numbeLivingCells == 3)
                { // Оживает
                    newMatrix[i][j] = 1;
                }
                else
                { // Остается мертвой
                    newMatrix[i][j] = 0;
                }
            }
        }
    }

    matrix = move(newMatrix); // Заменяем содержимое matrix на newMatrix
}

int cellCounting(vector<vector<int>> &matrix, int i, int j, int lines, int columns)
{
    int sum = 0;

    // Проверяем соседей клетки (i, j)
    for (int x = max(0, i - 1); x <= min(i + 1, lines - 1); x++)
    {
        for (int y = max(0, j - 1); y <= min(j + 1, columns - 1); y++)
        {
            if (x != i || y != j)
            { // Исключаем саму клетку (i, j)
                sum += matrix[x][y];
            }
        }
    }

    return sum;
}

void fieldDisplay(const vector<vector<int>> &matrix, int lines, int columns)
{

    clear();
    // Выводим матрицу в новом консольном окне
    for (int i = 0; i < lines; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            if (matrix[i][j])
            {
                mvaddch(i, j, ACS_BLOCK); // Отображаем живую клетку
            }
            else
            {
                mvaddch(i, j, ' '); // Отображаем мертвую клетку
            }
        }
        cout << endl;
    }
}
