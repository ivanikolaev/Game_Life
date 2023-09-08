#include <iostream>
#include <random>
#include <windows.h>
#include <ctime>
#include <fstream>
#include <vector>

using namespace std;

struct Cell {
	int type, age, hunger;
	bool move;
};

struct Point {
	int x, y;
};

int Size = 90, gen = 90, cntgen = 0;
int pred = 30, herb = 10, grass = 3, meteor = 10;
int starvation = 10, oldness = 20, predReproduct = 4, herbReproduct = 8;
int scale = 15;
bool autofill = 1, aging = 1;

vector<vector<Cell>> vec(Size, vector<Cell>(Size));

void filling() {
	for (int i = 0; i < Size; ++i) {
		for (int j = 0; j < Size; ++j) {
			if (rand() % pred == 0) {
				vec[i][j].type = 1;
				vec[i][j].age = 0;
				vec[i][j].hunger = 0;
				vec[i][j].move = 0;
			}
			else if (rand() % herb == 0) {
				vec[i][j].type = 2;
				vec[i][j].age = 0;
				vec[i][j].hunger = 0;
				vec[i][j].move = 0;
			}
			else if (rand() % grass == 0)
				vec[i][j].type = 3;
			else
				vec[i][j].type = 0;
		}
	}
}

void predMove(Cell& pred, Cell& herb) {
	herb.type = 1;
	herb.hunger = 0;
	herb.age = pred.age + 1;
	herb.move = 1;
	if (rand() % 3 == 0 && pred.age > predReproduct) {
		pred.age = 0;
		pred.hunger = 0;
		pred.move = 0;
	}
	else
		pred.type = 0;
}

void herbMove(Cell& herb, Cell& grass) {
	grass.type = 2;
	grass.age = herb.age + 1;
	grass.hunger = 0;
	grass.move = 1;
	if (rand() % 2 == 0 && herb.age > herbReproduct) {
		herb.age = 0;
		herb.hunger = 0;
		herb.move = 0;
	}
	else
		herb.type = 0;
}

void emptyMove(Cell& from, Cell& to) {
	to.type = from.type;
	to.age = from.age + 1;
	to.hunger = from.hunger + 1;
	to.move = 1;
	from.type = 0;
}

vector<Point> findrandom(int i, int j, int type1, int type2 = -1) {
	vector<Point> find = { { i, j } };
	if (i < Size - 1 && j < Size - 1 && (vec[i + 1][j + 1].type == type1 || vec[i + 1][j + 1].type == type2))
		find.push_back({ i + 1, j + 1 });
	if (i > 0 && j > 0 && (vec[i - 1][j - 1].type == type1 || vec[i - 1][j - 1].type == type2))
		find.push_back({ i - 1, j - 1 });
	if (i > 0 && j < Size - 1 && (vec[i - 1][j + 1].type == type1 || vec[i - 1][j + 1].type == type2))
		find.push_back({ i - 1, j + 1 });
	if (i < Size - 1 && j > 0 && (vec[i + 1][j - 1].type == type1 || vec[i + 1][j - 1].type == type2))
		find.push_back({ i + 1, j - 1 });
	if (i < Size - 1 && (vec[i + 1][j].type == type1 || vec[i + 1][j].type == type2))
		find.push_back({ i + 1, j });
	if (i > 0 && (vec[i - 1][j].type == type1 || vec[i - 1][j].type == type2))
		find.push_back({ i - 1, j });
	if (j < Size - 1 && (vec[i][j + 1].type == type1 || vec[i][j + 1].type == type2))
		find.push_back({ i, j + 1 });
	if (j > 0 && (vec[i][j - 1].type == type1 || vec[i][j - 1].type == type2))
		find.push_back({ i, j - 1 });
	return find;
}

Point finder(int x, int y, int type1, int type2 = -1) {
	Point find = { -1, -1 };
	int k = 0;
	while (find.x == -1 && k <= 10) {
		++k;
		for (int i = max(x - k, 0); i <= min(x + k, Size - 1) && y - k >= 0; ++i)
			if (vec[y - k][i].type == type1 || vec[y - k][i].type == type2)
				find = { y - k, i };
		for (int i = max(x - k, 0); i <= min(x + k, Size - 1) && y + k < Size; ++i)
			if (vec[y + k][i].type == type1 || vec[y + k][i].type == type2)
				find = { y + k, i };
		for (int i = max(y - k, 0); i <= min(y + k, Size - 1) && x - k >= 0; ++i)
			if (vec[i][x - k].type == type1 || vec[i][x - k].type == type2)
				find = { i, x - k };
		for (int i = max(x - k, 0); i <= min(x + k, Size - 1) && x + k < Size; ++i)
			if (vec[i][x + k].type == type1 || vec[i][x + k].type == type2)
				find = { i, x + k };
	}
	return find;
}

Point closest(int x1, int y1, int x2, int y2) {
	Point ans = { -1, -1 };
	if (x1 == x2)
		ans = { x1, y1 + ((y2 - y1) / abs(y2 - y1)) };
	else if (y1 == y2)
		ans = { x1 + ((x2 - x1) / (abs(x2 - x1))), y1 };
	else
		ans = { x1 + ((x2 - x1) / (abs(x2 - x1))), y1 + ((y2 - y1) / abs(y2 - y1)) };
	if (vec[ans.y][ans.x].type == 4 || (vec[ans.y][ans.x].type == 1 && vec[y1][x1].type == 2)) {
		vector<Point> find = findrandom(y1, x1, 0, 3);
		int r = rand() % find.size();
		ans = find[r];
	}
	return ans;
}

void year() {
	for (int i = 0; i < Size; ++i) {
		for (int j = 0; j < Size; ++j) {

			if (vec[i][j].type == 1 && !vec[i][j].move) {
				if (vec[i][j].age >= oldness || vec[i][j].hunger >= starvation)
					vec[i][j].type = 0;
				else {
					Point findHerb = finder(j, i, 2);
					if (findHerb.x != -1) {
						Point point = closest(j, i, findHerb.y, findHerb.x);
						if (vec[point.y][point.x].type == 2)
							predMove(vec[i][j], vec[point.y][point.x]);
						else
							emptyMove(vec[i][j], vec[point.y][point.x]);
					}
					else {
						Point findEmpty = finder(j, i, 0, 3);
						if (findEmpty.x != -1) {
							Point point = closest(j, i, findEmpty.y, findEmpty.x);
							emptyMove(vec[i][j], vec[point.y][point.x]);
						}
						else {
							++vec[i][j].age;
							++vec[i][j].hunger;
						}
					}
				}
			}

			else if (vec[i][j].type == 2 && !vec[i][j].move) {
				if (vec[i][j].age >= oldness || vec[i][j].hunger >= starvation)
					vec[i][j].type = 0;
				else {
					Point findGrass = finder(j, i, 3);
					if (findGrass.x != - 1) {
						Point point = closest(j, i, findGrass.y, findGrass.x);
						if (vec[point.y][point.x].type == 3)
							herbMove(vec[i][j], vec[point.y][point.x]);
						else
							emptyMove(vec[i][j], vec[point.y][point.x]);
					}
					else {
						Point findEmpty = finder(j, i, 0);
						if (findEmpty.x != -1) {
							Point point = closest(j, i, findEmpty.y, findEmpty.x);
							if (vec[point.y][point.x].type == 3)
								herbMove(vec[i][j], vec[point.y][point.x]);
							else
								emptyMove(vec[i][j], vec[point.y][point.x]);
						}
						else {
							++vec[i][j].age;
							++vec[i][j].hunger;
						}
					}
				}
			}

			if (vec[i][j].type == 4)
				vec[i][j].move ? vec[i][j].type = 0 : vec[i][j].move = 1;

			else if (vec[i][j].type == 0 && rand() % 100 == 0) {
				vec[i][j].type = 3;
			}
		}
	}

	if (rand() % meteor == 0) {
		int n;
		Size > 30 ? n = 5 : n = 3;
		int x = rand() % (Size - 2 * n) + n;
		int y = rand() % (Size - 2 * n) + n;
		for (int i = 0; i < n; ++i)
			for (int j = 0; j < n; ++j) {
				vec[x + i][y + j].type = 4;
				vec[x + i][y + j].move = 0;
			}
	}

	for (int i = 0; i < vec.size(); ++i)
		for (int j = 0; j < vec.size(); ++j)
			if (vec[i][j].type != 4)
				vec[i][j].move = 0;
}

int main() {
	setlocale(LC_ALL, "ru");
	srand(time(NULL));
	ofstream f("data.txt");
	f << "* - трава\n";
	f << ". - пустая клетка\n";
	f << "Px - хищник возраста x\n";
	f << "Hx - травоядное возраста x\n";
	f << "M - метеорит\n\n\n";

	HWND hwnd = GetConsoleWindow();
	HDC dc = GetDC(hwnd);
	RECT window = {};
	HBRUSH brush;

	cout << "Введите размер поля - целое число из диапазона [3; 90] (рекомендуется 30): ";
	cin >> Size;
	vec.resize(Size);
	for (int i = 0; i < Size; ++i)
		vec[i].resize(Size);
	cout << "Количество поколений: ";
	cin >> gen;
	if (Size > 30)
		scale = 5;

	cout << "Показывать процесс старения(да - 1, нет - 0): ";
	cin >> aging;
	cout << "Задание параметров пользовтелем(0) или автоматически(1): ";
	cin >> autofill;
	if (!autofill) {
		cout << "Хищники появляются в пропорции: 1/";
		cin >> pred;
		cout << "Травоядные появляются в пропорции: 1/";
		cin >> herb;
		cout << "Травa появляется в пропорции: 1/";
		cin >> grass;
		cout << "Метеорит падает с вероятностью: 1/";
		cin >> meteor;
		cout << "Через сколько лет наступает смерть от голода: ";
		cin >> starvation;
		cout << "Через сколько лет наступает смерть от старости: ";
		cin >> oldness;
		cout << "Репродуктивный возраст хищников: ";
		cin >> predReproduct;
		cout << "Репродуктивный возраст травоядных: ";
		cin >> herbReproduct;
	}
	system("cls");
	filling();

	while (cntgen != gen) {
		brush = CreateSolidBrush(RGB(125, 125, 255));
		SelectObject(dc, brush);
		Rectangle(dc, 95, 95, scale * Size + 105, scale * Size + 105);
		DeleteObject(brush);

		brush = CreateSolidBrush(RGB(0, 0, 0));
		SelectObject(dc, brush);
		Rectangle(dc, 100, 100, scale * Size + 100, scale * Size + 100);
		DeleteObject(brush);

		for (int i = 0; i < Size; ++i) {
			for (int j = 0; j < Size; ++j) {

				if (vec[i][j].type == 0)
					if (Size <= 30)
						f << ".   ";
					else
						f << ". ";

				else if (vec[i][j].type == 3) {
					brush = CreateSolidBrush(RGB(0, 255, 0));
					SelectObject(dc, brush);
					Rectangle(dc, i * scale + 100, j * scale + 100, (i + 1) * scale + 100, (j + 1) * scale + 100);
					DeleteObject(brush);
					if (Size <= 30)
						f << "*   ";
					else
						f << "* ";
				}

				else if (vec[i][j].type == 1) {
					brush = CreateSolidBrush(RGB(255 - aging * (vec[i][j].age * (oldness / 2.0)), 30, 55));
					SelectObject(dc, brush);
					Ellipse(dc, i * scale + 100, j * scale + 100, (i + 1) * scale + 100, (j + 1) * scale + 100);
					DeleteObject(brush);
					if (vec[i][j].age < 10 && Size <= 30)
						f << "P" << vec[i][j].age << "  ";
					else
						f << "P" << vec[i][j].age << " ";
				}

				else if (vec[i][j].type == 2) {
					brush = CreateSolidBrush(RGB(0, 178 - aging * (vec[i][j].age * (oldness / 2.0)), 255 - aging * (vec[i][j].age * (oldness / 2.0))));
					SelectObject(dc, brush);
					Ellipse(dc, i * scale + 100, j * scale + 100, (i + 1) * scale + 100, (j + 1) * scale + 100);
					DeleteObject(brush);
					if (vec[i][j].age < 10 && Size <= 30)
						f << "H" << vec[i][j].age << "  ";
					else
						f << "H" << vec[i][j].age << " ";
				}

				else if (vec[i][j].type == 4) {
					brush = CreateSolidBrush(RGB(255, 102, 0));
					SelectObject(dc, brush);
					Rectangle(dc, i * scale + 100, j * scale + 100, (i + 1) * scale + 100, (j + 1) * scale + 100);
					DeleteObject(brush);
					f << "M   ";
				}
			}
			f << "\n";
		}
		f << "\n\n\n";
		year();
		Sleep(500);
		int check = 0;
		for (int i = 0; i < Size; ++i)
			for (int j = 0; j < Size; ++j)
				if (vec[i][j].type == 0 || vec[i][j].type == 3 || vec[i][j].type == 4)
					++check;
		if (check == Size * Size)
			break;
		++cntgen;
		system("cls");
	}
	Sleep(1000);
	cout << "Программа завершилась на " << cntgen << "-ом поколении";
	f << "Программа завершилась на " << cntgen << "-ом поколении";
	f.clear();
	f.close();
}