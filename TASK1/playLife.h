#ifndef PLAYLIFE_H
#define PLAYLIFE_H

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <ncurses.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

using namespace std;

void generatePlayingField(vector<vector<int>> &matrix, int lines, int columns);
pair<int, int> readingPlayingField(vector<std::vector<int>> &matrix);
void fieldDisplay(const vector<std::vector<int>> &matrix, int lines, int columns);
int cellCounting(vector<vector<int>> &matrix, int i, int j, int lines, int columns);
void updateMatrix(vector<vector<int>> &matrix, int lines, int columns);
void playLife(vector<vector<int>> &matrix, int lines, int columns);

#endif PLAYLIFE_H