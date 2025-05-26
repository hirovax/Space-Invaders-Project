#include "header.h"


int screen_width = 101;
int screen_height = 32;
int statistics_height = 3;
int Score = 0;
int barrier_height = 7;
HANDLE console_out;
HANDLE console_in;
CONSOLE_CURSOR_INFO cursorInfo;
CONSOLE_SCREEN_BUFFER_INFO ConsoleBufferInfo;
SMALL_RECT RectWindow{0,0,short(screen_width + 1),short(screen_height + statistics_height + 1) }; // Left = 0, Top = 0, Right = screen_width + 1, Bottom =screen_height + statistics_height + 1 

void SetProperConsoleBufferVariables() {

	if (!SetConsoleWindowInfo(console_out, TRUE, &RectWindow)) {
		throw runtime_error("Unable to set console window size (SetConsoleWindowInfo).");
	}

	ShowScrollBar(GetConsoleWindow(), SB_BOTH, 0);

	//disabling cursor visibility
	if (!GetConsoleCursorInfo(console_out, &cursorInfo)) {
		throw runtime_error("Unable to get console cursor info (GetConsoleCursorInfo).");
	}
	cursorInfo.bVisible = false; // set the cursor visibility
	if (!SetConsoleCursorInfo(console_out, &cursorInfo)) { 
		throw runtime_error("Unable to set console cursor info (SetConsoleCursorInfo).");
	}
}

void Erase_object_from_screen(COORD coordinates, int rows, int columns) {
	for (int i = 0; i < rows; i++) {
		SetConsoleCursorPosition(console_out, coordinates);
		for (int i2 = 0; i2 < columns; i2++) {
			cout << ' ';
		}
		coordinates.Y++;
	}
}

void Update_object_pos_on_screen(int x1,int y1,int x2,int y2, string graphic[],int rows, int columns) {
	COORD coordinates;
	coordinates.X = (x1 + 1) - (columns/2);   // x1 + 1 because of the left wall,columns/2 + 1 because it needs to be int division rounded down, it represents x coordinate of extreme left corner of object
	coordinates.Y = screen_height - y1 - (rows/2); // y coordinate of extreme left corner of object
	//erasing the object
	Erase_object_from_screen(coordinates, rows, columns);
	//drawing the object
	coordinates.X = (x2 + 1) - (columns / 2);   
	coordinates.Y = screen_height - y2 - (rows / 2);
	for (int i = 0; i < rows; i++) {
		SetConsoleCursorPosition(console_out, coordinates);
		cout << graphic[i];
		coordinates.Y++;
	}
}

void Display_Borders() {
	cout << WHITE;
	//Upper Frame
	cout << LeftUpCorner;
	for (int i = 0; i < screen_width; i++) {
		cout << Floor;
	}
	cout << RightUpCorner << endl;

	//walls and arena
	for (int i = screen_height; i > 0; i--) {
		for (int i2 = -1; i2 <= screen_width; i2++) {
			if (i2 == -1 || i2 == screen_width) {
				cout << Wall;
			}
			else {
				//displaying arena
				cout << ' ';
			}
		}
		cout << endl;
	}

	//Bottom frame
	cout << LeftDownCorner;
	for (int i = 0; i < screen_width; i++) {
		cout << Floor;
	}
	cout << RightDownCorner;
}

void NewRound() {
	//erasing the player
	COORD temp;
	temp.X = (player.x + 1) - (player.width / 2);
	temp.Y = screen_height - player.y - (player.height / 2);
	Erase_object_from_screen(temp, player.height, player.width);
	//erasing existing projectiles
	for (int i = 0; i < projectiles.size(); i++) {
		COORD temp;
		temp.X = projectiles[i]->x + 1;
		temp.Y = screen_height - projectiles[i]->y;
		Erase_object_from_screen(temp, 1, 1);
		projectiles.erase(projectiles.begin() + i);
		i--;
	}
	//setting variables for next round 
	for (int i = 0; i < 10; i++) {
		enemies[i].graphic[0] = " *** ";
		enemies[i].graphic[1] = "*****";
		enemies[i].graphic[2] = "*   *";
		enemies[i].isdead = false;
	}
	SetEnemiesFirstPosition();
	FirstDisplayOfEnemies();
	player.x = 50;
	player.y = 3;
	player.FirstDisplay();
}

void ChangeScoreOnScreen() {
	COORD coordinates;
	coordinates.X = screen_width - 13;
	coordinates.Y = screen_height+2 + statistics_height/2;
	SetConsoleCursorPosition(console_out, coordinates);
	if (Score < 10000 && Score >= 0) cout << "Score: " << setfill('0') << setw(4) << Score;
	else cout << "Score: " << setfill('0') << setw(4) << 9999;
}

void ChangeHPOnScreen() {
	COORD coordinates;
	coordinates.X = 4;
	coordinates.Y = screen_height + 2 + statistics_height / 2;
	SetConsoleCursorPosition(console_out, coordinates);
	cout <<"HP: " << player.HP;
	coordinates.X = 11;
	cout << GREEN;
	for (int i = 1; i < 3; i++) {
		coordinates.Y = screen_height + 2 + statistics_height / 2 - 1;
		if (i <= player.HP-1) {
			SetConsoleCursorPosition(console_out, coordinates);
			cout << "  *  ";
			coordinates.Y++;
			SetConsoleCursorPosition(console_out, coordinates);
			cout << "*****";
			coordinates.Y++;
			SetConsoleCursorPosition(console_out, coordinates);
			cout << "*****";
			coordinates.Y++;
		}
		else {
			for (int i = 0; i < player.height; i++) {
				SetConsoleCursorPosition(console_out, coordinates);
				cout << "     ";
				coordinates.Y++;
			}
		}
		coordinates.X += player.width + 2;
	}
	cout << WHITE;

}

void displayBarrier() {
	COORD coordinates;
	coordinates.X = 1;
	coordinates.Y = screen_height + 1 - barrier_height;
	SetConsoleCursorPosition(console_out, coordinates);
	cout <<BRBLUE<< setfill('-') << setw(screen_width)<<"" << WHITE;
}

void DisplayHighscore(int highscore) {
	COORD coordinates;
	coordinates.X = 44;
	coordinates.Y = screen_height + 2 + statistics_height / 2;
	SetConsoleCursorPosition(console_out, coordinates);
	if (highscore < 10000 && highscore >= 0) cout << "Highscore: " << setfill('0') << setw(4) << highscore;
	else cout << "Highscore: " << 9999;
}

bool Menu(bool* HELLMODE) {
	//main menu
	console_in = GetStdHandle(STD_INPUT_HANDLE);
	FlushConsoleInputBuffer(console_in); // clear all events in console input buffer, to ensure that no keystrokes are waiting in the buffer
	SetConsoleCursorPosition(console_out, COORD{ 39, 5 });
	cout << YELLOW << "'Space Invaders' project" << WHITE;
	SetConsoleCursorPosition(console_out, COORD{ 25, 27 });
	cout << "To navigate in the menu, use " << GREEN << "Up" << WHITE << " and " << GREEN << "Down" << WHITE << " arrow keys.";
	SetConsoleCursorPosition(console_out, COORD{ 31, 28 });
	cout << "To confirm your selection, press " << GREEN << "Enter" << WHITE << ".";
	if (*HELLMODE) {
		SetConsoleCursorPosition(console_out, COORD{ 29, 30 });
		cout << RED<< "To quit HELLMODE, please restart your game.";
	}
	cout << GREEN;
	Display_Start_Button(HELLMODE);
	cout << WHITE;
	SetConsoleCursorPosition(console_out, COORD{ 43, 14 });
	cout << BRBLUE << Arrow << WHITE;
	Display_Exit_Button();
	Display_Instructions_Button();
	Display_Codes_Button();
	int MenuPointer = 0;
	int _input{};
	if (_kbhit()) {
		_input = _getch(); // resetting previous input
		_input = 0;
	}
	do {
		Sleep(16); // to limit resources consumption
		SetProperConsoleBufferVariables();
		if (_kbhit()) {
			_input = _getch();
			if (_input == 224 || _input == 0) {
				//whitening previous button and removing arrow
				switch (MenuPointer) {
				case 0:
					cout << WHITE;
					Display_Start_Button(HELLMODE);
					SetConsoleCursorPosition(console_out, COORD{ 43, 14 });
					cout << " ";
					break;
				case 1:
					cout << WHITE;
					Display_Instructions_Button();
					SetConsoleCursorPosition(console_out, COORD{ 43, 17 });
					cout << " ";
					break;
				case 2:
					cout << WHITE;
					Display_Codes_Button();
					SetConsoleCursorPosition(console_out, COORD{ 43, 20 });
					cout << " ";
					break;
				case 3:
					cout << WHITE;
					Display_Exit_Button();
					SetConsoleCursorPosition(console_out, COORD{ 43, 23 });
					cout << " ";
					break;
				}
				//checking if input is arrow up or down
				switch (_getch()) {
				case KEY_DOWN:
					if (MenuPointer < 3) MenuPointer++;
					break;
				case KEY_UP:
					if (MenuPointer > 0) MenuPointer--;
					break;
				}
				//setting correct color and displaying arrow
				switch (MenuPointer) {
				case 0:
					cout << GREEN;
					Display_Start_Button(HELLMODE);
					SetConsoleCursorPosition(console_out, COORD{ 43, 14 });
					cout << BRBLUE << Arrow << WHITE;
					break;
				case 1:
					cout << GREEN;
					Display_Instructions_Button();
					SetConsoleCursorPosition(console_out, COORD{ 43, 17 });
					cout << BRBLUE << Arrow << WHITE;
					break;
				case 2:
					cout << GREEN;
					Display_Codes_Button();
					SetConsoleCursorPosition(console_out, COORD{ 43, 20 });
					cout << BRBLUE << Arrow << WHITE;
					break;
				case 3:
					cout << GREEN;
					Display_Exit_Button();
					SetConsoleCursorPosition(console_out, COORD{ 43, 23 });
					cout << BRBLUE << Arrow << WHITE;
					break;
				}
			}
		}
	} while (_input!= '\r');
	cout << WHITE;
	Erase_object_from_screen(COORD{ 43,13 }, 12, 17); // erase buttons
	Erase_object_from_screen(COORD{ 25,27 }, 2, 52); // erase menu instructions
	if (*HELLMODE) Erase_object_from_screen(COORD{ 29,30 }, 1, 43); // erase HELLMODE instructions
	SetConsoleCursorPosition(console_out, COORD{ 39, 5 });
	cout << "                        "; // remove title text

	if (MenuPointer == 0) return true; // start game button
	if (MenuPointer == 1) { // instructions button
		SetConsoleCursorPosition(console_out, COORD{ 45, 5 });
		cout << "Instructions";
		SetConsoleCursorPosition(console_out, COORD{ 18, 10 });
		cout << "To move in the game, use " << GREEN << "Left" << WHITE << " and " << GREEN << "Right" << WHITE << " arrow keys on your keyboard.";
		SetConsoleCursorPosition(console_out, COORD{ 31, 11 });
		cout << "To shoot a " << YELLOW << "projectile" << WHITE << ", use the " << GREEN << "Space" << WHITE << " key.";
		SetConsoleCursorPosition(console_out, COORD{ 20, 12 });
		cout << "Your goal in this game is to destroy as many " << RED << "enemies" << WHITE << " as you can,";
		SetConsoleCursorPosition(console_out, COORD{ 35, 13 });
		cout << "while avoiding their projectiles.";
		SetConsoleCursorPosition(console_out, COORD{ 34, 15 });
		cout << "To go back to the menu, press " << GREEN << "Enter" << WHITE <<".";
		do {
			Sleep(16); // to limit resources consumption
			SetConsoleCursorPosition(console_out, COORD{ 0, 0 }); // disable scroll
			if (_kbhit()) {
				if (_getch() == 13) break; // enter character
			}
		} while (true);
		Erase_object_from_screen(COORD{ 18,10 }, 6, 68); // erase the instructions
	}
	if (MenuPointer == 2) { //codes button
		SetConsoleCursorPosition(console_out, COORD{ 48, 5 });
		cout << "CODES";
		SetConsoleCursorPosition(console_out, COORD{ 12, 7 });
		cout << "Here you can enter unique " << YELLOW << "codes" << WHITE << ", which will enable special features in the game.";
		SetConsoleCursorPosition(console_out, COORD{ 14, 8 });
		cout << "To get these codes, you need to earn a certain amount of points in the game.";
		SetConsoleCursorPosition(console_out, COORD{ 23, 9 });
		cout << "Press " << GREEN << "Enter" << WHITE << " to confirm your " << YELLOW << "code" << WHITE << ", and go back to the menu.";
		string code = "          ";
		SetConsoleCursorPosition(console_out, COORD{ 43, 17 });
		cout << "Enter your code: ";
		SetConsoleCursorPosition(console_out, COORD{ 60, 17 });
		GetConsoleCursorInfo(console_out, &cursorInfo);
		cursorInfo.bVisible = true; // set the cursor visibility to true
		SetConsoleCursorInfo(console_out, &cursorInfo);
		//entering code with max size of 8 characters
		char ch{};
		int i = 0;
		do {
			Sleep(16); // to limit resources consumption
			SetConsoleWindowInfo(console_out, TRUE, &RectWindow); // disables scrolling by different approach than cursor position (cursor position caused flickering of cursor)
			if (_kbhit()) {
				ch = _getch();

				if (ch == 13) break; // enter character
				if (ch == 8) { // backspace character
					if (i > 0) {
						code[i] = ' ';
						SetConsoleCursorPosition(console_out, COORD{ short(60 + i - 1), 17 });
						cout << ' ';
						SetConsoleCursorPosition(console_out, COORD{ short(60 + i - 1), 17 });
						i--;
					}
				}
				else {

					if (i != 8 && ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == ' ')) { // codes can only contain a-z letters with uppercase letters, numbers, and spaces
						code[i] = ch;
						cout << ch;
						i++;
					}
				}
			}
		} while (true);
		GetConsoleCursorInfo(console_out, &cursorInfo);
		cursorInfo.bVisible = false;
		SetConsoleCursorInfo(console_out, &cursorInfo);
		if (code == "HELLMODE  ") {
			*HELLMODE = true;
			SetConsoleCursorPosition(console_out, COORD{ 47, 18 });
			cout << RED << "HELLMODE ACTIVATED" << WHITE;
		}
		else if (code != "          ") {
			SetConsoleCursorPosition(console_out, COORD{ 47, 18 });
			cout << "Invalid code";
		}
		if (code != "          ") Sleep(1500);
		SetConsoleCursorPosition(console_out, COORD{ 47, 18 });
		cout << "                  "; // erasing "enter your code: XXXXXXXX"
		SetConsoleCursorPosition(console_out, COORD{ 43, 17 });
		cout << "                         "; // erasing "hellmode activated", or anything else
		Erase_object_from_screen(COORD{ 12,7 }, 3, 80); // erasing instructions 
		return false;
	}
	if (MenuPointer == 3) { // exit button
		exit(0);
	}
	return false;
}

void Display_Start_Button(bool* HELLMODE) {
	COORD coordinates;
	coordinates.X = 44;
	coordinates.Y = 13;
	SetConsoleCursorPosition(console_out, coordinates);
	cout << LeftUpCorner;
	for (int i = 0; i < 14; i++) cout << Floor;
	cout << RightUpCorner;
	coordinates.Y++;
	SetConsoleCursorPosition(console_out, coordinates);
	cout << Wall;
	if (*HELLMODE) cout << "   HELLMODE   ";
	else cout << "  Start Game  ";
	cout << Wall;
	coordinates.Y++;
	SetConsoleCursorPosition(console_out, coordinates);
	cout << LeftDownCorner;
	for (int i = 0; i < 14; i++) cout << Floor;
	cout << RightDownCorner;
}

void Display_Exit_Button() {
	COORD coordinates;
	coordinates.X = 44;
	coordinates.Y = 22;
	SetConsoleCursorPosition(console_out, coordinates);
	cout << LeftUpCorner;
	for (int i = 0; i < 14; i++) cout << Floor;
	cout << RightUpCorner;
	coordinates.Y++;
	SetConsoleCursorPosition(console_out, coordinates);
	cout << Wall << "     Exit     " << Wall;
	coordinates.Y++;
	SetConsoleCursorPosition(console_out, coordinates);
	cout << LeftDownCorner;
	for (int i = 0; i < 14; i++) cout << Floor;
	cout << RightDownCorner;
}

void Display_Codes_Button() {
	COORD coordinates;
	coordinates.X = 44;
	coordinates.Y = 19;
	SetConsoleCursorPosition(console_out, coordinates);
	cout << LeftUpCorner;
	for (int i = 0; i < 14; i++) cout << Floor;
	cout << RightUpCorner;
	coordinates.Y++;
	SetConsoleCursorPosition(console_out, coordinates);
	cout << Wall << "    CODES     " << Wall;
	coordinates.Y++;
	SetConsoleCursorPosition(console_out, coordinates);
	cout << LeftDownCorner;
	for (int i = 0; i < 14; i++) cout << Floor;
	cout << RightDownCorner;
}

void Display_Instructions_Button() {
	COORD coordinates;
	coordinates.X = 44;
	coordinates.Y = 16;
	SetConsoleCursorPosition(console_out, coordinates);
	cout << LeftUpCorner;
	for (int i = 0; i < 14; i++) cout << Floor;
	cout << RightUpCorner;
	coordinates.Y++;
	SetConsoleCursorPosition(console_out, coordinates);
	cout << Wall << " Instructions " << Wall;
	coordinates.Y++;
	SetConsoleCursorPosition(console_out, coordinates);
	cout << LeftDownCorner;
	for (int i = 0; i < 14; i++) cout << Floor;
	cout << RightDownCorner;
}

void GameOver() {
	COORD coordinates;
	coordinates.X = 47;
	coordinates.Y = 13;
	cout << RED;
	for (int i = 0; i < 10; i++) {
		SetConsoleCursorPosition(console_out, coordinates);
		cout << "GAME OVER";
		Sleep(100);
		SetConsoleCursorPosition(console_out, coordinates);
		cout << "         ";
		Sleep(100);
	}
	system("CLS");
	cout << WHITE;
}
