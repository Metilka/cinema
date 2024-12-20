﻿#ifndef HEADER_H
#define HEADER_H

#include <windows.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <string> // Для работы со строками
#include <algorithm>
#include <chrono>
#include <fstream>
#include <sstream>
#include <thread>
#include <codecvt>      // Äëÿ ïðåîáðàçîâàíèÿ UTF-16 â wide string
#include <io.h>         // Äëÿ ðàáîòû ñ _setmode
#include <locale>       // Äëÿ ðàáîòû ñ êîäèðîâêàìè
#include <fcntl.h>      // Äëÿ ðåæèìà _O_U16TEXT
#include <sstream>
#include <cctype>


void displayFilmDescription(const std::wstring& filmName);

using namespace std;

// Конфигурация программы
namespace Config {
	int y = 0;
	constexpr const int BOX_WIDTH = 6;              // Ширина бокса
	constexpr const int BOX_HEIGHT = 3;             // Высота бокса
	constexpr const int CHANSE_NOT_FREE_PLACES = 8; // Вероятность занятого места
	constexpr const int row_distance = 3; // дистанция для генерации дорогих мест для ряда
	constexpr const int column_distance = 5; // дистанция для генерации дорогих мест для мест 
	constexpr const int expensive_place = 450; // стоимость дорогого места
	constexpr const int cheap_place = 350; // стоимость дешёвого места
	const int rowCount = 8; // количества рядов
	const int placeCount = 18;// 16 и 2 для отрисовки номера ряда с двух сторон


}
using namespace Config;

void sessionSelection(int day);

void setCursorPosition(int x, int y) {
	COORD coord = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Структура для представления одного места
struct Seat {
	wstring status; // "x" — занято, "0" — номер ряда, число — номер места
	wstring color; // цвет места 
	int cost;
};

// Структура для ряда
struct Row {
	vector<Seat> seats;
};

// Структура зала
struct Session {
	vector<Row> rows;
	wstring film_name;
	wstring time_film;
	wstring genre;
	wstring duration;
};

//структура Сеанса
struct Day {
	vector<Session> Cinema_room_1;
	vector<Session> Cinema_room_2;
	vector<Session> Cinema_room_3;
};
struct TrioDays {
	vector <Day> trio_days;
};



void SetColor(int text, int background) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (background << 4) | text);
}

void setMode16() {
	_setmode(_fileno(stdout), _O_U16TEXT);
	_setmode(_fileno(stdin), _O_U16TEXT);
}

// Проверка и преобразование строк
// Проверка, является ли строка числом
bool isNumber(const wstring& str) {
	for (wchar_t c : str) {
		if (!iswdigit(c)) {
			return false;
		}
	}
	return true;
}
// Преобразование строки в число
int stringToInt(const wstring& str) {
	return stoi(str);
}


// Функции отрисовки
// Функция отрисовки занятого места
void drawOccupiedBox(int& x, int y, wstring& clr) {
	if (clr == L"red")
		SetColor(12, 12);
	else
		if (clr == L"violet") {
			SetColor(13, 13);
		}


	setCursorPosition(x, y + 1);
	wcout << L"|    |";
	setCursorPosition(x, y + 2);
	wcout << L"| " << L" " << L" " << L" |";
	setCursorPosition(x, y + 3);
	wcout << L"|____|";
	SetColor(15, 0);
	x += BOX_WIDTH + 1;
}

// Функция отрисовки номера ряда
void drawRowNumberBox(int& x, int y, int rowNumber) {
	x += 1;
	setCursorPosition(x, y + 2);
	wcout << L"Row " << rowNumber;
	x += BOX_WIDTH + 1;
}



// Функция отрисовки дешёвого свободного места
void drawWhiteAvailableBox(int& x, int y, const wstring& number) {
	SetColor(15, 0);
	setCursorPosition(x, y);
	wcout << L" ____ ";
	setCursorPosition(x, y + 1);
	wcout << L"|    |";
	setCursorPosition(x, y + 2);
	wcout << L"| " << (stringToInt(number) < 10 ? L" " : L"") << number << L" |";
	setCursorPosition(x, y + 3);
	wcout << L"|____|";
	x += BOX_WIDTH + 1;
}

// Функция отрисовки дорогого свободного места
void drawYellowAvailableBox(int& x, int y, const wstring& number) {
	SetColor(14, 0);
	setCursorPosition(x, y);
	wcout << L" ____ ";
	setCursorPosition(x, y + 1);
	wcout << L"|    |";
	setCursorPosition(x, y + 2);
	wcout << L"| " << (stringToInt(number) < 10 ? L" " : L"") << number << L" |";
	setCursorPosition(x, y + 3);
	wcout << L"|____|";
	x += BOX_WIDTH + 1;
	SetColor(15, 0);
}

// Рисуем рамку для конкретного места
void drawBox(int& x, int y, const Seat& seat, int rowNumber) {
	if (isNumber(seat.status) && seat.status != L"0" && seat.color == L"white") {
		drawWhiteAvailableBox(x, y, seat.status);
	}
	else if (seat.status == L"x") {
		wstring clr = seat.color;
		drawOccupiedBox(x, y, clr);
	}
	else if (seat.status == L"0") {
		drawRowNumberBox(x, y, rowNumber);
	}
	else if (isNumber(seat.status) && seat.status != L"0" && seat.color == L"yellow") {
		drawYellowAvailableBox(x, y, seat.status);
	}
}

//// Отрисовка ряда
void drawRow(int y, const Row& row, int rowNumber) {
	int x = 0;
	for (const auto& seat : row.seats) {
		drawBox(x, y, seat, rowNumber);
	}
}

/// <summary>
/// Эта функция отрисовывает весь зал
/// </summary>
/// <param name="session">сеанс</param>
/// <param name="rowCount"> количество рядов</param>
/// <param name="placeCount">количество мест</param>
void DrawSession(Session& session, int rowCount, int placeCount) {
	wcout << setw(65) << session.time_film << endl;
	++y;
	wcout << setw(67) << session.film_name << endl;
	++y;
	for (int i = session.rows.size() - 1; i >= 0; --i) {
		drawRow(y, session.rows[i], i + 1);
		y += BOX_HEIGHT + 2;
	}
	setCursorPosition(0, y);

}

// Генерация зала
void GenerationRoom(Session& session, const int rowCount, const int placeCount, wstring name, wstring time_f, wstring genre, wstring duration) {
	session.rows.resize(rowCount);
	session.film_name = name;
	session.time_film = time_f;
	session.genre = genre;
	session.duration = duration;
	for (int i = 0; i < rowCount; ++i) {
		session.rows[i].seats.resize(placeCount);
		for (int j = 0; j < placeCount; ++j) {
			if (j == 0 || j == placeCount - 1) {
				session.rows[i].seats[j].status = L"0";
			}
			else {
				int rand_not_free = rand() % CHANSE_NOT_FREE_PLACES; // шанс что место уже занято 
				if (rand_not_free == 0) {
					session.rows[i].seats[j].status = L"x"; // сохранение состояния текущего места
					session.rows[i].seats[j].color = L"red"; // установка цвета текущего места
				}
				else {
					session.rows[i].seats[j].status = to_wstring(j); // сохранение состояния текущего места

					if ((i <= (rowCount - row_distance) && i >= (row_distance - 1)) && (j >= column_distance && j <= (placeCount - column_distance - 1))) {
						session.rows[i].seats[j].color = L"yellow"; // установка цвета текущего места
						session.rows[i].seats[j].cost = expensive_place; //установка цены текущего места
					}
					else {
						session.rows[i].seats[j].color = L"white"; // установка цвета текущего места
						session.rows[i].seats[j].cost = cheap_place; //установка цены текущего места
					}
				}

			}
		}
	}
}

void fullScreen() {
	COORD coord;
	SetConsoleDisplayMode(GetStdHandle(STD_OUTPUT_HANDLE), CONSOLE_FULLSCREEN_MODE, &coord);
	keybd_event(VK_MENU, 0, 0, 0); // Нажатие Alt
	keybd_event(VK_RETURN, 0, 0, 0); // Нажатие Enter
	keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0); // Отпуск Enter
	keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0); // Отпуск Alt
	this_thread::sleep_for(chrono::milliseconds(100));
}

wstring replaceDash(wstring str) {
	replace(str.begin(), str.end(), L'—', L'-');
	return str;
}

void cleanString(wstring& str) {
	// Удаляем переносы строк и другие невидимые символы
	str.erase(remove(str.begin(), str.end(), L'\r'), str.end());
	str.erase(remove(str.begin(), str.end(), L'\n'), str.end());
	str.erase(remove_if(str.begin(), str.end(), [](wchar_t c) {
		return iswspace(c) && c != L' ';
		}), str.end());
}

// Функция для удаления "\r" последующей очистки данных
void removeCarriageReturn(wstring& line) {
	line.erase(remove(line.begin(), line.end(), L'\r'), line.end());
	cleanString(line); // Чистим строку после удаления символов
}


/// <summary>
/// Генерация данных для сеансов кино на основе данных из файла.
/// Считывает информацию о залах, сеансах и их деталях из файла и загружает их в структуру данных.
/// </summary>
/// <param name="day">Структура данных, содержащая информацию о всех сеансах в трех залах</param>
/// <param name="filename">Имя файла, содержащего данные о сеансах</param>
/// <param name="rowCount">Количество рядов в зале (используется для размещения мест)</param>
/// <param name="placeCount">Количество мест в ряду (используется для размещения мест)</param>
void GenerationDay(Day& day, const wstring& filename, int rowCount, int placeCount) {
	wstringstream file(filename);


	// Устанавливаем кодировку UTF-8
	file.imbue(locale(locale(), new codecvt_utf8<wchar_t>));

	wstring line;
	vector<Session>* currentSessionGroup = nullptr;

	wcout << L"Содержимое файла:\n";
	while (getline(file, line)) {
		removeCarriageReturn(line);
		line = replaceDash(line);

		if (line.empty()) continue;

		// Обработка залов
		if (line.find(L"Cinema room 1") != wstring::npos) {
			currentSessionGroup = &day.Cinema_room_1;
			continue;
		}
		if (line.find(L"Cinema room 2") != wstring::npos) {
			currentSessionGroup = &day.Cinema_room_2;
			continue;
		}
		if (line.find(L"Cinema room 3") != wstring::npos) {
			currentSessionGroup = &day.Cinema_room_3;
			continue;
		}

		// Обработка количества сеансов
		if (currentSessionGroup && iswdigit(line[0])) {
			int sessionCount = 0;
			try {
				sessionCount = stoi(line);
				currentSessionGroup->resize(sessionCount);
			}
			catch (const exception&) {
				wcerr << L"Ошибка: Некорректное количество сеансов." << endl;
				return;
			}

			for (int i = 0; i < sessionCount; ++i) {
				Session session;

				if (!getline(file, session.time_film) || session.time_film.empty()) {
					wcerr << L"Ошибка: Некорректное или отсутствует время фильма." << endl;
					return;
				}

				if (!getline(file, session.film_name) || session.film_name.empty()) {
					wcerr << L"Ошибка: Некорректное или отсутствует название фильма." << endl;
					return;
				}

				if (!getline(file, session.genre) || session.genre.empty()) {
					wcerr << L"Ошибка: Некорректный или отсутствует жанр фильма." << endl;
					return;
				}

				if (!getline(file, session.duration) || session.duration.empty()) {
					wcerr << L"Ошибка: Некорректная или отсутствует продолжительность фильма." << endl;
					return;
				}

				cleanString(session.time_film);
				cleanString(session.film_name);
				cleanString(session.genre);
				cleanString(session.duration);

				GenerationRoom(session, rowCount, placeCount, session.film_name, session.time_film, session.genre, session.duration);
				(*currentSessionGroup)[i] = session;
			}
		}
	}
}




//Очистка экрана :
void ClearScreen() {
	y = 0;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD count, cellCount;
	COORD homeCoords = { 0, 0 };

	if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
		cellCount = csbi.dwSize.X * csbi.dwSize.Y; // Исправлено
		FillConsoleOutputCharacter(hConsole, ' ', cellCount, homeCoords, &count);
		FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords, &count);
		SetConsoleCursorPosition(hConsole, homeCoords);
	}
}


/// <summary>
/// Очистка экрана с определённой позиции и до конца
/// </summary>
/// <param name="startX">Позиция Х</param>
/// <param name="startY">Позиция Y</param>
void ClearScreenFromPosition(int startX, int startY) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD count, cellCount;
	COORD startCoord = { (SHORT)startX, (SHORT)startY };

	if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
		// Вычисляем количество ячеек, которые нужно очистить
		cellCount = (csbi.dwSize.X - startX) * (csbi.dwSize.Y - startY);

		// Заполняем пространство пробелами
		FillConsoleOutputCharacter(hConsole, ' ', cellCount, startCoord, &count);
		FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, startCoord, &count);

		// Устанавливаем курсор в начальную позицию
		setCursorPosition(startX, startY);
	}
}

void closeWindow() {
	// Нажимаем Alt
	keybd_event(VK_MENU, 0x38, 0, 0);
	// Нажимаем F4
	keybd_event(VK_F4, 0x3E, 0, 0);
	// Отпускаем F4
	keybd_event(VK_F4, 0x3E, KEYEVENTF_KEYUP, 0);
	// Отпускаем Alt
	keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
}

// функиция смены масштаба
void PressCtrlMinus() {
	// Эмуляция нажатия клавиши Ctrl
	keybd_event(VK_CONTROL, 0, 0, 0);
	// Эмуляция нажатия клавиши "-"
	keybd_event(VK_OEM_MINUS, 0, 0, 0);
	// Эмуляция отпускания клавиши "-"
	keybd_event(VK_OEM_MINUS, 0, KEYEVENTF_KEYUP, 0);
	// Эмуляция отпускания клавиши Ctrl
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
}

void waitForInput() {
	system("pause");
	cin.clear();
}

// Функция для проверки ввода числа
bool correctInput(int& number) {
	wstring input;
	getline(wcin, input); // Читаем строку
	if (input.empty()) {
		return false; // Проверка на пустой ввод
	}
	try {
		size_t pos;
		number = stoi(input, &pos); // Пробуем преобразовать строку в число
		if (pos != input.size()) {
			return false; // Если есть лишние символы
		}
	}
	catch (const invalid_argument&) {
		return false; // Если преобразование не удалось
	}
	catch (const out_of_range&) {
		return false; // Если число выходит за пределы
	}
	return true; // Успех
}

void choosingPlace(Session& session, int day);
/// <summary>
/// Функция фильтрует сеансы кинотеатра по запросу пользователя, предоставляя возможность 
/// многократного использования фильтра через интерактивный интерфейс.
/// </summary>
/// <param name="trio_days">Ссылка на объект TrioDays, содержащий данные о сеансах за три дня.</param>
/// <remarks>
/// Пользователь вводит ключевое слово (например, часть названия фильма или жанра), 
/// а функция ищет совпадения среди всех доступных сеансов. Результаты отображаются с указанием 
/// дня, зала, времени, названия и других данных о сеансе.
/// После каждой фильтрации пользователю предлагается возможность повторить поиск или завершить фильтрацию.
/// </remarks>
void filterSessions(const TrioDays& trio_days) {
	bool repeatFiltering = true; // Флаг для управления повторением фильтрации

	while (repeatFiltering) { // Цикл для многократного использования фильтра
		ClearScreen(); // Очистка экрана перед началом нового ввода

		// Меню фильтрации
		wcout << L"╔═════════════════════════════════════╗\n";
		wcout << L"║          Фильтр фильмов             ║\n";
		wcout << L"╠═════════════════════════════════════╣\n";
		wcout << L"║  Поиск: _________________________   ║\n";
		wcout << L"║                                     ║\n";
		wcout << L"║  Доступно:                          ║\n";
		wcout << L"║  1. Поиск по ЖАНРУ                  ║\n";
		wcout << L"║  2. Поиск по НАЗВАНИЮ               ║\n";
		wcout << L"║                                     ║\n";
		wcout << L"╚═════════════════════════════════════╝\n";

		wcout << L"Ваш выбор: ";
		int searchOption;
		while (true) {
			wstring input;
			getline(wcin, input);
			try {
				searchOption = stoi(input); // Преобразуем строку в целое число
				if (searchOption == 0 || searchOption == 1 || searchOption == 2) break; // Разрешаем только 0, 1, 2
				wcout << L"Некорректный выбор. Пожалуйста, выберите 1 или 2.\n";
			}
			catch (const invalid_argument&) {
				wcout << L"Ошибка: введите число.\n";
			}
			catch (const out_of_range&) {
				wcout << L"Ошибка: число слишком большое.\n";
			}
		}

		// Если пользователь выбрал 0, выходим из цикла
		if (searchOption == 0) {
			repeatFiltering = false;
			continue;
		}

		wcout << L"Введите ключевое слово для поиска (например, название фильма или жанр): ";
		wstring query;
		while (true) {
			getline(wcin, query);
			if (query.empty()) {
				wcout << L"Ошибка: ключевое слово не может быть пустым. Попробуйте снова.\n";
				continue;
			}
			break;
		}

		// Преобразование запроса в нижний регистр
		locale loc("ru_RU.UTF-8");
		transform(query.begin(), query.end(), query.begin(),
			[&loc](wchar_t c) { return tolower(c, loc); });

		vector<wstring> foundFilms;
		vector<Session> sessionsFound; // Храним найденные сеансы для выбора мест

		// Поиск совпадений
		for (size_t dayIndex = 0; dayIndex < trio_days.trio_days.size(); ++dayIndex) {
			const Day& day = trio_days.trio_days[dayIndex];
			vector<vector<Session>> allRooms = { day.Cinema_room_1, day.Cinema_room_2, day.Cinema_room_3 };

			for (size_t roomIndex = 0; roomIndex < allRooms.size(); ++roomIndex) {
				const vector<Session>& room = allRooms[roomIndex];
				for (const auto& session : room) {
					// Преобразуем название фильма и жанр в нижний регистр
					wstring sessionFilmLower = session.film_name;
					wstring sessionGenreLower = session.genre;
					cleanString(sessionFilmLower);
					cleanString(sessionGenreLower);
					transform(sessionFilmLower.begin(), sessionFilmLower.end(), sessionFilmLower.begin(),
						[&loc](wchar_t c) { return tolower(c, loc); });
					transform(sessionGenreLower.begin(), sessionGenreLower.end(), sessionGenreLower.begin(),
						[&loc](wchar_t c) { return tolower(c, loc); });

					// Проверка на совпадения
					if (sessionFilmLower.find(query) != wstring::npos || sessionGenreLower.find(query) != wstring::npos) {
						foundFilms.push_back(L"День: " + to_wstring(dayIndex + 1) +
							L", Зал: " + to_wstring(roomIndex + 1) +
							L", Название: " + session.film_name +
							L", Жанр: " + session.genre +
							L", Время: " + session.time_film +
							L", Продолжительность: " + session.duration);
						sessionsFound.push_back(session); // Сохраняем сеанс для последующего выбора
					}
				}
			}
		}

		if (!foundFilms.empty()) {
			wcout << L"╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
			wcout << L"║         СПИСОК ПОДОБРАННЫХ ФИЛЬМОВ                                                                                                                                                       ║\n";
			wcout << L"╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n";
			for (size_t i = 0; i < foundFilms.size(); ++i) {
				wcout << L"║ [" << (i + 1) << L"] " << foundFilms[i] << L"\n";
			}
			wcout << L"╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n";

			// Меню выбора действия
			wcout << L"\nЧто вы хотите сделать дальше?\n";
			wcout << L"1. Перейти к описанию найденного фильма\n";
			wcout << L"2. Перейти к выбору мест для сеанса\n";
			wcout << L"3. Заново воспользоваться фильтром\n";
			wcout << L"4. Вернуться в меню\n";
			wcout << L"Введите ваш выбор (1-4): ";

			int choice = -1;
			while (choice < 1 || choice > 4) {
				wstring input;
				getline(wcin, input);
				try {
					choice = stoi(input);
					if (choice < 1 || choice > 4) {
						wcout << L"Некорректный выбор. Пожалуйста, выберите 1, 2, 3 или 4.\n";
					}
				}
				catch (const invalid_argument&) {
					wcout << L"Ошибка: введите число.\n";
				}
				catch (const out_of_range&) {
					wcout << L"Ошибка: число слишком большое.\n";
				}
			}

			switch (choice) {
			case 1: {
				wcout << L"Введите номер фильма для получения описания: ";
				int filmIndex = -1;
				while (filmIndex < 1 || filmIndex > foundFilms.size()) {
					wstring input;
					getline(wcin, input);
					try {
						filmIndex = stoi(input);
						if (filmIndex < 1 || filmIndex > foundFilms.size()) {
							wcout << L"Некорректный номер фильма. Введите число от 1 до " << foundFilms.size() << L".\n";
						}
					}
					catch (const invalid_argument&) {
						wcout << L"Ошибка: введите число.\n";
					}
					catch (const out_of_range&) {
						wcout << L"Ошибка: число слишком большое.\n";
					}
				}
				displayFilmDescription(sessionsFound[filmIndex - 1].film_name);
				break;
			}
			case 2: { // Переход к выбору мест
				wcout << L"Введите номер сеанса для выбора мест: ";
				int filmIndex = -1;
				while (filmIndex < 1 || filmIndex > sessionsFound.size()) {
					wstring input;
					getline(wcin, input);
					try {
						filmIndex = stoi(input);
						if (filmIndex < 1 || filmIndex > sessionsFound.size()) {
							wcout << L"Некорректный номер сеанса. Введите число от 1 до " << sessionsFound.size() << L".\n";
						}
					}
					catch (const invalid_argument&) {
						wcout << L"Ошибка: введите число.\n";
					}
					catch (const out_of_range&) {
						wcout << L"Ошибка: число слишком большое.\n";
					}
				}
				ClearScreen();
				choosingPlace(sessionsFound[filmIndex - 1], 0); // Передача выбранного сеанса и дня (заменить 0 на нужный день)
				break;
			}
			case 3:
				continue; // Повторить фильтрацию
			case 4:
				repeatFiltering = false; // Выход из фильтра
				break;
			}
		}
		else {
			wcout << L"Совпадений не найдено.\n";
		}
	}
}



bool aoutoChoosingPlace(Session& session, int cnt_places, int& bookedRow, vector<int>& bookedRows, vector<int>& bookedPlaces, double& totalCost) {
	for (int i = 0; i < session.rows.size(); ++i) {
		int cnt = 0;  // Счётчик свободных мест подряд
		int start_index = -1; // Индекс начала первого подходящего участка

		for (int j = 1; j < session.rows[i].seats.size() - 1; ++j) { // Проход от 1 до предпоследнего индекса
			if (session.rows[i].seats[j].status != L"x") {
				// Если место свободно, увеличиваем счётчик
				if (cnt == 0) start_index = j; // Устанавливаем начало участка
				++cnt;

				if (cnt == cnt_places) { // Если нашли подходящий участок
					// Помечаем места как занятые
					for (int k = start_index; k < start_index + cnt_places; ++k) {
						session.rows[i].seats[k].status = L"x";
						session.rows[i].seats[k].color = L"violet";
						bookedRows.push_back(i); // Добавляем ряд в список забронированных
						bookedPlaces.push_back(k); // Добавляем место в список забронированных
						totalCost += session.rows[i].seats[k].cost;
					}
					ClearScreen();
					DrawSession(session, session.rows.size(), session.rows[0].seats.size());
					for (int k = start_index; k < start_index + cnt_places; ++k) {
						session.rows[i].seats[k].color = L"red";
					}
					return true; // Возвращаем успех
				}
			}
			else {
				// Если место занято, сбрасываем счётчик
				cnt = 0;
				start_index = -1;
			}
		}
	}
	return false; // Если ни одного подходящего участка не найдено
}
// Функция для вывода всех деталей билета
void printTicketDetails(const vector<int>& bookedRows, const vector<int>& bookedPlaces, int cnt_places, double totalCost, const wstring& filmName, const wstring& filmTime, const wstring& genre, const wstring& duration) {
	wcout << L"\n---------- Билет ----------\n";
	wcout << L"Фильм: " << filmName << L"\n";
	wcout << L"Жанр: " << genre << L"\n";
	wcout << L"Сеанс: " << filmTime << L"\n";
	wcout << L"Продолжительность: " << duration << L"\n\n";

	wcout << L"Забронированные места:\n";
	for (size_t i = 0; i < bookedRows.size(); ++i) {
		wcout << L"Ряд: " << bookedRows[i] + 1
			<< L", Место: " << bookedPlaces[i];
		if (i < bookedRows.size() - 1) {
			wcout << L" | ";
		}
	}

	wcout << L"\nКоличество билетов: " << cnt_places << L"\n";
	wcout << L"Общая стоимость: " << totalCost << L" рублей.\n";
	wcout << L"--------------------------------\n";
}


void chooseAdditionalItems(double& totalAmount) {
	short int itemChoice;
	bool moreItems = true; // Флаг для продолжения выбора товаров

	while (moreItems) {
		wcout << L"--- Дополнительные товары ---\n";
		wcout << L"1. Кола (150 рублей)\n";
		wcout << L"2. Попкорн (100 рублей)\n";
		wcout << L"3. Чипсы (120 рублей)\n";
		wcout << L"4. Соки (130 рублей)\n";
		wcout << L"5. Нет, спасибо\n";
		wcout << L"Введите номер выбранного товара: ";
		wcin >> itemChoice;

		switch (itemChoice) {
		case 1:
			wcout << L"Вы выбрали Кола.\n";
			totalAmount += 150.0;
			break;
		case 2:
			wcout << L"Вы выбрали Попкорн.\n";
			totalAmount += 100.0;
			break;
		case 3:
			wcout << L"Вы выбрали Чипсы.\n";
			totalAmount += 120.0;
			break;
		case 4:
			wcout << L"Вы выбрали Соки.\n";
			totalAmount += 130.0;
			break;
		case 5:
			wcout << L"Вы выбрали пропустить этот шаг.\n";
			moreItems = false; // Заканчиваем выбор товаров
			break;
		default:
			wcout << L"Некорректный выбор. Пожалуйста, попробуйте снова.\n";
			continue;
		}

		// Запрос о продолжении выбора
		if (itemChoice != 5) { // Пропускаем вопрос, если выбрано "Нет, спасибо"
			wstring addMore;
			wcout << L"Хотите добавить ещё товары? (Y/N): ";
			wcin >> addMore;
			if (addMore != L"Yes" && addMore != L"yes") {
				moreItems = false; // Заканчиваем выбор товаров
			}
		}

		wcout << L"Общая сумма с учетом выбранных товаров: " << totalAmount << L" рублей.\n";
	}
}


// Функция для выбора способа оплаты
void choosePaymentMethod(double totalAmount) {
	int paymentChoice;
	wcout << L"--- Способы оплаты ---\n";
	wcout << L"1. Наличными\n";
	wcout << L"2. Картой\n";
	wcout << L"3. Электронный кошелёк\n";
	wcout << L"Введите номер выбранного способа оплаты: ";
	wcin >> paymentChoice;

	switch (paymentChoice) {
	case 1:
		wcout << L"Вы выбрали оплату наличными. Общая сумма: " << totalAmount << L" рублей.\n";
		break;
	case 2:
		wcout << L"Вы выбрали оплату картой. Общая сумма: " << totalAmount << L" рублей.\n";
		break;
	case 3:
		wcout << L"Вы выбрали оплату электронным кошельком. Общая сумма: " << totalAmount << L" рублей.\n";
		break;
	default:
		wcout << L"Некорректный выбор. Пожалуйста, попробуйте снова.\n";
		choosePaymentMethod(totalAmount);
		return;
	}

	wcout << L"Спасибо за ваш выбор! Транзакция завершена.\n";
}

void choosingPlace(Session& session, int day) {
	DrawSession(session, session.rows.size(), session.rows[0].seats.size());
	setCursorPosition(0, y);

	int choice;
	int cnt_error_messeg = 0;
	double totalCost = 0;
	// Выбор способа бронирования
	while (true) {

		if (cnt_error_messeg >= 3) {
			ClearScreenFromPosition(0, 42);
			cnt_error_messeg = 0;
			continue;
		}
		wcout << L"Выберите способ бронирования мест:\n";
		wcout << L"1. Автоподбор мест\n";
		wcout << L"2. Ручной выбор мест\n";
		wcout << L"0 Вернуться назад\n";
		wcout << L"Введите ваш выбор: ";

		if (!correctInput(choice)) {
			if (choice != 1 && choice != 2) {
				cnt_error_messeg += 3;
				wcout << L"Некорректный ввод. Введите 1 или 2.\n";
			}
			continue;
		}

		if (choice == 0)
			sessionSelection(day);

		if (choice == 1) { // Автоподбор мест
			int cnt_places, bookedRow;
			vector<int> bookedRows; // Список забронированных рядов
			vector<int> bookedPlaces; // Список забронированных мест
			while (true) {
				if (cnt_error_messeg > 3) {
					ClearScreenFromPosition(0, 42);
					cnt_error_messeg = 0;
					continue;
				}
				wcout << L"Введите количество мест (меньше 16): ";

				if (!correctInput(cnt_places)) {
					++cnt_error_messeg;
					wcout << L"Некорректный ввод. Введите количество мест заново.\n";
					continue;
				}

				if (cnt_places <= 0 || cnt_places > 16) {
					++cnt_error_messeg;
					wcout << L"Количество мест вне диапазона. Пожалуйста, введите корректное количество.\n";
					continue;
				}

				if (!aoutoChoosingPlace(session, cnt_places, bookedRow, bookedRows, bookedPlaces, totalCost)) {
					wcout << L"К сожалению, не удалось найти " << cnt_places << L" свободных рядом мест.\n";
					++cnt_error_messeg;
					continue;
				}


				wcout << L"Места успешно забронированы.\n";

				// Вывод всех деталей билета
				printTicketDetails(bookedRows, bookedPlaces, cnt_places, totalCost, session.film_name, session.time_film, session.genre, session.duration);
				chooseAdditionalItems(totalCost);
				choosePaymentMethod(totalCost);
				break;
			}
			break;

		}
		else if (choice == 2) { // Ручной выбор мест
			int cnt_places, row, place;
			vector<int> bookedRows; // Список забронированных рядов
			vector<int> bookedPlaces; // Список забронированных мест
			while (true) {
				if (cnt_error_messeg > 3) {
					ClearScreenFromPosition(0, 42);
					cnt_error_messeg = 0;
					continue;
				}
				wcout << L"Сколько мест вы хотите купить? ";
				if (!correctInput(cnt_places)) {
					++cnt_error_messeg;
					wcout << L"Некорректный ввод. Введите количество мест заново.\n";
					continue;
				}

				if (cnt_places <= 0 || cnt_places > 16) {
					++cnt_error_messeg;
					wcout << L"Количество мест вне диапазона. Пожалуйста, введите корректное количество.\n";
					continue;
				}

				for (int i = 0; i < cnt_places; ++i) {
					while (true) {
						if (cnt_error_messeg > 3) {
							ClearScreenFromPosition(0, 42);
							cnt_error_messeg = 0;
							continue;
						}
						wcout << L"Введите номер ряда: ";
						if (!correctInput(row)) {
							++cnt_error_messeg;
							wcout << L"Некорректный ввод. Введите номер ряда заново.\n";
							continue;
						}

						--row; // Приведение к индексации с 0
						if (row < 0 || row >= session.rows.size()) {
							++cnt_error_messeg;
							wcout << L"Номер ряда вне диапазона. Введите корректный номер ряда.\n";
							continue;
						}

						wcout << L"Введите номер места: ";
						if (!correctInput(place)) {
							++cnt_error_messeg;
							wcout << L"Некорректный ввод. Введите номер места заново.\n";
							continue;
						}

						if (place <= 0 || place >= session.rows[row].seats.size() - 1) {
							++cnt_error_messeg;
							wcout << L"Номер места вне диапазона. Введите корректный номер места.\n";
							continue;
						}

						if (session.rows[row].seats[place].status == L"x" || session.rows[row].seats[place].status == L"0") {
							++cnt_error_messeg;
							wcout << L"Место занято, выберите другое.\n";
							continue;
						}
						ClearScreen();
						// Если место свободно, бронируем его
						session.rows[row].seats[place].status = L"x";
						session.rows[row].seats[place].color = L"violet";
						DrawSession(session, session.rows.size(), session.rows[0].seats.size());
						wcout << L"Место успешно забронировано.\n";
						session.rows[row].seats[place].color = L"red";
						totalCost += session.rows[row].seats[place].cost;
						bookedRows.push_back(row);
						bookedPlaces.push_back(place);
						break;
					}
				}
				printTicketDetails(bookedRows, bookedPlaces, cnt_places, totalCost, session.film_name, session.time_film, session.genre, session.duration);
				chooseAdditionalItems(totalCost);
				choosePaymentMethod(totalCost);
				break;
			}
			break;// завершение программы.
		}
	}
}

#endif // HEADER_H