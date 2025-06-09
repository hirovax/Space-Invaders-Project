#pragma once
#include <iostream>
#include <Windows.h>
#include <chrono>
#include <vector>
#include <iomanip>
#include <fstream>
#include <conio.h>
#include <SFML/Audio.hpp>

//COLORS
#define RED "\033[31m"
#define WHITE "\033[37m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define YELLOW "\033[33m"
#define GREY "\033[90m"
#define BRBLUE "\033[94m"
//SYMBOLS
#define Wall (char)0xBA
#define Floor (char)0xCD
#define LeftUpCorner (char)0xC9
#define RightUpCorner (char)0xBB
#define RightDownCorner (char)0xBC
#define LeftDownCorner (char)0xC8	
#define Arrow (char)0xAF
#define VolumeBlock (char)0xDB
//KEYS
#define KEY_DOWN 80
#define KEY_UP 72
#define KEY_RIGHT 77
#define KEY_LEFT 75

using namespace std;
extern int enemies_alive;

extern int screen_width;
extern int screen_height;
extern int statistics_height;
extern int barrier_height;

extern int Score;
extern HANDLE console_out;
extern CONSOLE_CURSOR_INFO cursorInfo;
extern CONSOLE_SCREEN_BUFFER_INFO ConsoleBufferInfo;
extern SMALL_RECT RectWindow;

extern float Volume;

//Core
void Update_object_pos_on_screen(int x1, int y1, int x2, int y2, string graphic[], int rows, int columns);
void Display_Borders();
void SetProperConsoleBufferVariables();
void Erase_object_from_screen(COORD coordinates, int rows, int columns);
void NewRound();
void ChangeScoreOnScreen();
void ChangeHPOnScreen();
void displayBarrier();
void DisplayHighscore(int highscore);
void Display_Volume_Setting();
void Display_Start_Button(bool* HELLMODE);
void Display_Exit_Button();
void Display_Codes_Button();
void Display_Instructions_Button();
void GameOver();

void loadSoundFiles();
void playMusic(int* index);
void playEnemyDeathSound();
void playPlayerDeathSound();
void playPlayerProjectileShootSound();
void playEnemyProjectileShootSound();
void playGameOverSound();

bool Menu(bool* HELLMODE);


//projectile
class Projectile {
public:
	void Move();
	bool CheckCollision(int index);
	void DieAnimation();
	bool isDying() const { return dying; }
	int getX() const { return x; }
	int getY() const { return y; }
private:
	bool dying = false;
	int dieanimationFrame = 0;
	int x=0;
	int y=0;
	string direction;
	const int width = 1;
	const int height = 1;
	string graphic = "|";
	friend class Player;
	friend class Enemy;
};
extern vector<unique_ptr<Projectile>> projectiles;

//Player
class Player {
public:
	Player();
	void FirstDisplay();
	void Move(string direction);
	void ShootProjectile(Projectile &projectile);
	void DieAnimation();
	void setGraphic(const string line1, const string line2, const string line3) { graphic[0] = line1; graphic[1] = line2; graphic[2] = line3; }
	void setHp(int hp) { HP = hp; }
	int getHp() const { return HP; }
	bool isDying() const { return dying; }
	int getX() const { return x; }
	int getY() const { return y; }
	void setX(int X) { x = X; }
	void setY(int Y) { y = Y; }
	int getWidth() const { return width; }
	int getHeight() const { return height; }
private:
	int dieanimationFrame = 0;
	bool dying = false;
	int HP = 3;
	int x = 50; // starting pos x
	int y = 3; // starting pos y
	const int width = 5;
	const int height = 3;
	string graphic[3] = {};
	friend class Projectile;
};
extern Player player;


//Enemy 
class Enemy {
public:
	Enemy();
	void DieAnimation();
	void ShootProjectile(Projectile& projectile);
	bool isDying() const { return dying; }
	bool isDead() const { return isdead; }
	void setGraphic(const string line1, const string line2, const string line3) { graphic[0] = line1; graphic[1] = line2; graphic[2] = line3; }
	void setIsDead(bool ISDEAD) { isdead = ISDEAD; }
	int getX() const { return x; }
	int getY() const { return y; }
	string getGraphicLine(int index) const { return graphic[index]; }
private:
	int dieanimationFrame = 0;
	bool dying = false;
	bool isdead = false;
	int x = 0;
	int y = 0;
	const int width = 5;
	const int height = 3;
	string graphic[3] = {};


	friend class Projectile;
	friend void SetEnemiesFirstPosition();
	friend bool MoveEnemies(string direction);
	
};
void SetEnemiesFirstPosition();
void FirstDisplayOfEnemies();
bool MoveEnemies(string direction);
string SetEnemiesDirection(string direction);
void RandomEnemyShoot();
extern Enemy enemies[10];


