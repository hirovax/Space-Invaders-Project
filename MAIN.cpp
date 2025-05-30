#include "header.h"


Enemy enemies[10];
int enemies_alive = 10;
int highscore;
vector<unique_ptr<Projectile>> projectiles;
Player player;

void playSound() {
	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile("killdeer.wav")) {
		std::cout << "Failed to load sound" << std::endl;
		return;
	}
	sf::Sound sound;
	sound.setBuffer(buffer);
	sound.play();
}

int main() {
	//setting window parameters
	try {
		console_out = GetStdHandle(STD_OUTPUT_HANDLE);
		if (console_out == INVALID_HANDLE_VALUE) {
			throw runtime_error("Unable to get handle to console (GetStdHandle).");
		}
		SetProperConsoleBufferVariables(); // scroll to the top (in case of user scrolling down), set window size, disable cursor, disable scrollbars

		// disabling resizing and maximizing
		HWND consoleWindow = GetConsoleWindow();
		if (!consoleWindow) {
			throw runtime_error("Unable to get handle to console window (GetConsoleWindow).");
		}
		LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
		if (style == 0) {
			throw runtime_error("Unable to get style of console window (GetWindowLong).");
		}
		style &= ~WS_SIZEBOX;
		style &= ~WS_MAXIMIZEBOX;
		style &= ~WS_MINIMIZEBOX;

		if (!SetWindowLong(consoleWindow, GWL_STYLE, style)) {	
			throw runtime_error("Unable to set style of console window (SetWindowLong).");
		}

		//enable colors
		DWORD consoleMode = 0;
		if (!GetConsoleMode(console_out, &consoleMode)) {
			throw runtime_error("Unable to get console mode (GetConsoleMode).");
		}
		if (!SetConsoleMode(console_out, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
			throw runtime_error("Unable to set console mode (SetConsoleMode).");
		};
	} catch (const exception& e) {
		cerr << "Error while initializing window: " << e.what() << endl;
		system("PAUSE");
		return 0;
	}


	//reading highscore from binary file
	ifstream saveOUT("statistics.dat", ios::binary | ios::in);
	if (saveOUT.is_open()) {
		saveOUT.read(reinterpret_cast<char*>(&highscore), sizeof(highscore));
		saveOUT.close();
	}
	else highscore = 0;

	try {
		loadSoundFiles();
	} catch (const exception& e) {
		cerr << "Error while loading sound files: " << e.what() << endl;
		system("PAUSE");
		return 0;
	}
	bool HELLMODE = false;
	do {
		SetConsoleCursorPosition(console_out, COORD{ 0, 0 }); // scroll to the top

		Display_Borders();
		DisplayHighscore(highscore);


		
		//main menu
		while (!Menu(&HELLMODE)) {}

		//setting variables
		srand(unsigned int(time(NULL)));
		Score = 0;
		enemies_alive = 10;
		if (HELLMODE) player.HP = 1;
		else player.HP = 3;
		player.graphic[0] = "  *  ";
		player.graphic[1] = "*****";
		player.graphic[2] = "*****";

		//displaying stats
		ChangeScoreOnScreen();
		ChangeHPOnScreen();

		NewRound();
		//declaractions 
		chrono::steady_clock::time_point start_time, end_time;
		double frame_duration;
		double reload_time = 0;
		double enemymove_time = 0.5;
		double enemyreload_time = 1.0;
		string enemiesDirection = "right";
		int soundIndex{};
		double MusicDelay{};
		displayBarrier();
		Sleep(1000);
		//Game
		do {
			if (MusicDelay <= 0) {
				playMusic(&soundIndex);
				MusicDelay = 1.0 / (11 - enemies_alive);
			}
			start_time = chrono::steady_clock::now();
			try {
				SetProperConsoleBufferVariables();
			}
			catch (const exception& e) {
				cerr << "Error while initializing window: " << e.what() << endl;
				system("PAUSE");
				return 0;
			}
			displayBarrier(); // displaying barrier every frame, because projectiles can penetrate it

			//enemies dying animations
			for (int i = 0; i < 10; i++) {
				if (enemies[i].dying) {
					enemies[i].DieAnimation();
					if (!enemies[i].dying) {
						enemies_alive--;
					}
				}
			}
			if (!player.dying) {
				if (enemies_alive <= 0) {
					//next round
					enemies_alive = 10;
					Sleep(500);
					NewRound();
					//setting local variables
					reload_time = 0;
					enemymove_time = 0.5;
					enemyreload_time = 1.0;
					enemiesDirection = "right";
					Sleep(1000);
					start_time = chrono::steady_clock::now();
				}
				//enemy shooting
				if (enemyreload_time < 0) {
					if (HELLMODE) enemyreload_time = 0.15;
					 else enemyreload_time = (rand() % 1200 + 300) / 1000.0; // random reload time from 0.3 seconds to 1.5 seconds
					RandomEnemyShoot();
				}

				//player shooting
				if (GetAsyncKeyState(VK_SPACE) && reload_time < 0) {
					reload_time = 0.5;
					try {
						projectiles.push_back(make_unique<Projectile>());
						player.ShootProjectile(*projectiles.back());
					} catch (const exception&) {}
				}



				//projectiles
				for (int i = 0; i < projectiles.size(); i++) {
					//check if collision occurs if not animated death
					if (!projectiles[i]->dying && projectiles[i]->CheckCollision(i)) {
						//delete if not animated death // double checking of (bool dying), because CheckCollision can change its state
						if (!projectiles[i]->dying) {
							COORD temp;
							temp.X = projectiles[i]->x + 1;
							temp.Y = screen_height - projectiles[i]->y;
							Erase_object_from_screen(temp, 1, 1);
							projectiles.erase(projectiles.begin() + i);
							i--;// why? because if it wasn't i--, then the next projectile would be skipped on checking the collisions.
						}
					}
				}
				//projectiles movement and animations
				for (int i = 0; i < projectiles.size(); i++) {
					if (!projectiles[i]->dying) {
						projectiles[i]->Move();
					}
					else {
						//projectile dying animation
						projectiles[i]->DieAnimation();
						//delete if animation ended
						if (!projectiles[i]->dying) {
							projectiles.erase(projectiles.begin() + i);
							i--; //the same as in the collisions
						}
					}

				}

				//enemy movement
				if (enemymove_time < 0) {
					enemiesDirection = SetEnemiesDirection(enemiesDirection);
					enemymove_time = 1.0 / ((11 - enemies_alive) * 2.0);
					//if enemies can move -> aren't too close to player
					if (!MoveEnemies(enemiesDirection)) {
						break;
					}
				}
				//player movement
				if (GetAsyncKeyState(VK_LEFT)) {
					if (player.x - 3 >= 0) {
						player.Move("left");
					}
				}
				if (GetAsyncKeyState(VK_RIGHT)) {
					if (player.x + 3 < screen_width) {
						player.Move("right");
					}
				}
			}
			//player dying animation
			else {
				player.DieAnimation();
				//resetting key states
				GetAsyncKeyState(VK_SPACE);
				GetAsyncKeyState(VK_RIGHT);
				GetAsyncKeyState(VK_LEFT);
			}


			//setting refresh rate to 60 frames per second, 16 miliseconds for 1 frame
			end_time = chrono::steady_clock::now();
			frame_duration = chrono::duration_cast<chrono::duration<double>>(end_time - start_time).count();
			if (frame_duration < 0.016) {
				Sleep(int(16.0 - frame_duration * 1000));
				if (!player.dying) {
					enemymove_time -= 0.020;
					reload_time -= 0.020;
					enemyreload_time -= 0.020;
					MusicDelay -= 0.020;
				}
			}
			else {
				if (!player.dying) {
					enemymove_time -= frame_duration;
					reload_time -= frame_duration;
					enemyreload_time -= frame_duration;
					MusicDelay -= frame_duration;
				}
			}

		} while (player.HP > 0);
		GameOver();
		//writing highscore to binary file
		if (Score > highscore) {
			ofstream saveIN("statistics.dat", ios::binary | ios::out);
			saveIN.write(reinterpret_cast<char*>(&Score), sizeof(Score));
			if (!saveIN) {
				SetConsoleCursorPosition(console_out, COORD{ 45,12 });
				cout << RED << "ERROR: Unable to write highscore to file!";
			}
			SetConsoleCursorPosition(console_out, COORD{ 45,12 });
			cout << RED << "ERROR: Unable to write highscore to file!";
			highscore = Score;
			SetConsoleCursorPosition(console_out, COORD{ 45,13 });
			cout << YELLOW << "New Highscore!";
			SetConsoleCursorPosition(console_out, COORD{ 50,14 });
			if (highscore<10000) cout << setfill('0') << setw(4) << highscore << WHITE;
			else cout << 9999 << WHITE;
			if (Score < 500) Sleep(1500);
		}
		if (Score >= 500) {
			SetConsoleCursorPosition(console_out, COORD{ 35,15 });
			cout << BRBLUE << "You've reached 500 points milestone!";
			SetConsoleCursorPosition(console_out, COORD{ 35,16 });
			cout << "Here's secret code for you: " << RED << "HELLMODE" << WHITE;
			Sleep(5000);
		}
		
	} while (true);
	exit(0);
}