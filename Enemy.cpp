#include "header.h"

Enemy::Enemy() {
	graphic[0] = " *** ";
	graphic[1] = "*****";
	graphic[2] = "*   *";
}

void SetEnemiesFirstPosition() {
	int enemyx, enemyy;
	enemyy = 27; // 27 because it needs to be 3 away from the ceiling
	for (int i = 0; i < 2; i++) {
		enemyx = 16; //16 because it needs 13 spaces from the wall, and its graphics are 3x5, so the middle x position will be 16
		for (int i2 = 0; i2 < 5; i2++) {
			enemies[i * 5 + i2].x = enemyx;
			enemies[i * 5 + i2].y = enemyy;
			enemyx = enemyx + 12 + 5; // 12 spaces between enemies in x axis, +5 because of graphic of the enemy
		}
		enemyy = enemyy - 3 - 3; // 3 spaces between enemies in y axis, -3 because of graphic of the enemy
	}
}

void FirstDisplayOfEnemies()
 {
	cout << RED;
	COORD temp;
	for (int i = 0; i < 2; i++) {	//loop for 1 row
		
		for (int i2 = 0; i2 < 5; i2++) { // loop for 5 columns
			Sleep(100); //visualisation effect
			SetConsoleWindowInfo(console_out, TRUE, &RectWindow); // disable scroll
			//left up corner of enemy
			temp.Y = (screen_height - enemies[i * 5 + i2].y) - 1; // extreme up y
			temp.X = (enemies[i * 5 + i2].x - 2) + 1; // extreme left x
			for (int i3 = 0; i3 < 3; i3++) {
				SetConsoleCursorPosition(console_out, temp);
				cout << enemies[i * 5 + i2].graphic[i3];
				temp.Y++;
			}
		}
	}
	cout << WHITE;
}

void Enemy::DieAnimation() {
	switch (dieanimationFrame) {
	case 0:
		graphic[0] = "- - -";
		graphic[1] = " - - ";
		graphic[2] = "- - -";
		break;
	case 7:
		graphic[0] = " - - ";
		graphic[1] = "-   -";
		graphic[2] = " - - ";
		break;
	case 15:
		graphic[0] = "     ";
		graphic[1] = "     ";
		graphic[2] = "     ";
		dieanimationFrame=-1;
		isdead = true;
		dying = false;
		break;
	}
	cout << GREY;
	Update_object_pos_on_screen(x, y, x, y, graphic, height, width);
	cout << WHITE;
	dieanimationFrame++;

}

string predirection;
string SetEnemiesDirection(string direction) {
	if (direction == "right") {
		for (int i = 4; i >= 0; i--) {
			if (!enemies[i+5].isdead || !enemies[i].isdead) {
				if (enemies[i].x + 1 + 2 >= screen_width) {
					predirection = "right";
					return "down";
				}
			}
		}
	}
	if (direction == "left") {
		for (int i = 0; i <= 4; i++) {
			if (!enemies[i].isdead || !enemies[i + 5].isdead) {
				if (enemies[i].x -1 -2 <= -1) {
					predirection = "left";
					return "down";
				}
			}
		}
	}
	if (direction == "down") {
		if (predirection == "right") return "left";
		if (predirection == "left") return "right";
	}
	return direction;
}

bool MoveEnemies(string direction) {
	int deltax=0;
	int deltay=0;

	if (direction == "right") deltax = 1;
	if (direction == "left") deltax = -1;
	if (direction == "down") deltay = -3;
	for (int i = 0; i < 10; i++) {
		cout << RED;
		if (!enemies[i].isdead) {
			if (enemies[i].dying) cout << GREY;
			Update_object_pos_on_screen(enemies[i].x, enemies[i].y, enemies[i].x + deltax, enemies[i].y + deltay, enemies[i].graphic, enemies[i].height, enemies[i].width);
		}
		enemies[i].x += deltax;
		enemies[i].y += deltay;
	}
	cout << WHITE;
	for (int i = 1; i >= 0; i--) {
		for (int i2 = 0; i2 < 5; i2++) {
			if (!enemies[i * 5 + i2].isdead && enemies[i * 5 + i2].y - enemies[i * 5 + i2].height / 2 <= barrier_height) {
				return false;
			}
		}
	}
	return true;
}

void RandomEnemyShoot() {
	//randomizing which enemy will shoot
	int random_enemy_to_shoot=0;
	int random_limit = 10;
	bool found = false;
	while (true) {
		if (random_limit == 0) {
			break;
		}
		random_enemy_to_shoot = rand() % 5; //randomising column
		if (!enemies[random_enemy_to_shoot + 5].isdead && !enemies[random_enemy_to_shoot + 5].dying) {   // checking if enemy in second row in randomized column is alive
			random_enemy_to_shoot += 5;
			found = true;
			break;
		}
		else {
			if (!enemies[random_enemy_to_shoot].isdead && !enemies[random_enemy_to_shoot].dying && !enemies[random_enemy_to_shoot + 5].dying) {  // checking if enemy in first row in randomized column is alive and enemy in second row is not dying
				found = true;
				break;
			}
		}
		random_limit--;

	}
	if (random_limit == 0) {
		for (int i = 0; i < 5; i++) {
			if (!enemies[i + 5].isdead && !enemies[i + 5].dying) {
				random_enemy_to_shoot = i + 5;
				found = true;
				break;
			}
			else {
				if (!enemies[i].isdead && !enemies[i].dying && !enemies[i+5].dying) {
					random_enemy_to_shoot = i;
					found = true;
					break;
				}
			}
		}
	}
	if (found) {
		//shooting the projectile
		projectiles.push_back(make_unique<Projectile>());
		enemies[random_enemy_to_shoot].ShootProjectile(*projectiles.back());
	}
}

void Enemy::ShootProjectile(Projectile& projectile) {
	projectile.x = x;
	projectile.y = y - (height/2 + 1);
	projectile.direction = "down";
}
