#include "header.h"


Player::Player()
{
	graphic[0] = "  *  ";	
	graphic[1] = "*****";
	graphic[2] = "*****";
}

void Player::FirstDisplay() {
	COORD temp;
	//left up corner of player
	temp.X = (x - 2) + 1;
	temp.Y = (screen_height - y) - 1; 
	cout << GREEN;
	for (int i = 0; i < 3; i++) {
		SetConsoleCursorPosition(console_out, temp);
		cout << graphic[i];
		temp.Y++;
	}
	cout << WHITE;
}

void Player::Move(string direction) {
	cout << GREEN;
	int x_sign = 0;
	if (direction == "left") {
		x_sign = -1;
	}
	if (direction == "right") {
		x_sign = 1;
	}
	Update_object_pos_on_screen(x, y, x + x_sign, y, graphic,3,5);
	x = x + x_sign;
	cout << WHITE;
}

void Player::ShootProjectile(Projectile &projectile) {
	playPlayerProjectileShootSound();
	projectile.x = x;
	projectile.y = y + (height/2 + 1);
	projectile.direction = "up";
}

void Player::DieAnimation() {
	switch (dieanimationFrame) {
	case 0:
		playPlayerDeathSound();
		graphic[0] = "* * *";
		graphic[1] = " * * ";
		graphic[2] = "* * *";
		break;
	case 5:
		graphic[0] = " * * ";
		graphic[1] = "* * *";
		graphic[2] = " * * ";
		break;
	case 10:
		graphic[0] = " *** ";
		graphic[1] = "*** *";
		graphic[2] = " * **";
		break;
	case 15:
		graphic[0] = "* * *";
		graphic[1] = " * * ";
		graphic[2] = "* * *";
		break;
	case 20:
		graphic[0] = " * * ";
		graphic[1] = "* * *";
		graphic[2] = " * * ";
		break;
	case 25:
		graphic[0] = " *** ";
		graphic[1] = "*** *";
		graphic[2] = " * **";
		break;
	case 30:
		graphic[0] = "     ";
		graphic[1] = "     ";
		graphic[2] = "     ";
		HP--;
		ChangeHPOnScreen();
		break;
	case 60:
		graphic[0] = "  *  ";
		graphic[1] = "*****";
		graphic[2] = "*****";
		dieanimationFrame = -1;
		dying = false;
		x = 50;
		y = 3;
	}
	cout << GREEN;
	Update_object_pos_on_screen(x, y, x, y, graphic, height, width);
	cout << WHITE;
	dieanimationFrame++;

}
