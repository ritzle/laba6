
#include "playLife.h"

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

std::pair<int, int> readingPlayingField(std::vector<std::vector<int>> &matrix)
{
    std::vector<std::vector<int>> newMatrix;

    std::ifstream inputFile("train.txt");
    if (!inputFile)
    {
        std::cerr << "Не удалось открыть файл input.txt" << std::endl;
        return std::make_pair(0, 0);
    }

    std::string line;
    int rows = 0;
    int maxCols = 0;
    while (std::getline(inputFile, line))
    {
        std::vector<int> rowData;
        std::stringstream ss(line);
        int value;
        while (ss >> value)
        {
            rowData.push_back(value);
        }
        newMatrix.push_back(std::move(rowData));
        int currentCols = static_cast<int>(newMatrix.back().size());
        if (currentCols > maxCols)
        {
            maxCols = currentCols;
        }
        rows++;
    }

    inputFile.close();
    matrix = std::move(newMatrix);

    return std::make_pair(rows, maxCols);
}

void playLife(vector<vector<int>> &matrix, int lines, int columns)
{
    initscr();             // Инициализация ncurses
    curs_set(0);           // Скрываем курсор
    cbreak();              // Режим постоянного ввода
    noecho();              // Отключаем вывод введенных символов
    nodelay(stdscr, true); // Отключаем блокировку getch()

    bool running = true;

    char ch;
    int sleep = 200000;

    while (running)
    {
        clear();
        fieldDisplay(matrix, lines, columns);
        updateMatrix(matrix, lines, columns);

        ch = getch(); // Считываем нажатую клавишу

        switch (ch)
        {
        case '+': // Стрелка влево
            sleep += 200000;
            break;

        case '=': // Стрелка влево
            sleep += 200000;
            break;

        case '-': // Стрелка вправо

            if (sleep - 200000 > 0)
            {
                sleep -= 20000;
            }
            break;

        case 'q':            // Клавиша "q"
            running = false; // Выставляем флаг выхода
            break;

        default:
            break;
        }

        refresh();     // Обновляем экран
        usleep(sleep); // Задержка
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
            else if (matrix[i][j] == 0)
            { // Если клетка мертвая
                if (numbeLivingCells == 3)
                { // Оживает
                    newMatrix[i][j] = 1;
                }
            }
            else
            {
                continue;
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
            if (!(x == i && y == j))
            { // Исключаем саму клетку (i, j)
                sum += matrix[x][y];
            }
        }
    }

    return sum;
}

void fieldDisplay(const std::vector<std::vector<int>> &matrix, int lines, int columns)
{
    // clear();

    // Рисуем границу вокруг игрового поля
    for (int i = 0; i < lines + 2; i++)
    {
        for (int j = 0; j < columns + 2; j++)
        {
            if (i == 0 || i == lines + 1 || j == 0 || j == columns + 1)
            {
                mvaddch(i, j, ACS_BLOCK); // Отображаем стенку
            }
            else
            {
                if (matrix[i - 1][j - 1] == 1)
                {
                    mvaddch(i, j, '@'); // Отображаем живую клетку
                }
                else
                {
                    mvaddch(i, j, ' '); // Отображаем мертвую клетку
                }
            }
        }
    }

    refresh();
}