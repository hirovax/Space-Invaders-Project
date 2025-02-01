#include "header.h"




bool Projectile::CheckCollision(int index) {
	if (direction == "up") {
		//hit roof
		if (y + 1 >= screen_height) {
			dying = true;
			return true;
		}
		//hit enemy
		for (int i = 0; i < 10; i++) {
			if (!enemies[i].isdead && !enemies[i].dying &&
				y  >= (enemies[i].y - enemies[i].height / 2) &&
				y  <= (enemies[i].y + enemies[i].height / 2) &&
				x >= (enemies[i].x - enemies[i].width / 2) &&
				x <= (enemies[i].x + enemies[i].width / 2)) {
				enemies[i].dying = true;
				Score += 10;
				ChangeScoreOnScreen();
				return true;
			}
		}
		//hit projectile
		for (int i = index + 1; i < projectiles.size(); i++) {
			if (projectiles[i]->x == x) {
				if (projectiles[i]->y == y + 2) {
					projectiles[i]->dying = true;
					dying = true;
					Move(); // move closer to the collided projectile
					return true;
				}
				if (projectiles[i]->y == y + 1) {
					projectiles[i]->dying = true;
					dying = true;
					return true;
				}
			}
		}
	}
	if (direction == "down") {
		//hit bottom
		if (y - 1 <= -1) {
			dying = true;
			return true;
		}
		//hit player
		if (y <= (player.y + player.height / 2) &&
			y >= (player.y - player.height / 2) &&
			x <= (player.x + player.width / 2) &&
			x >= (player.x - player.width / 2)) {
			player.dying = true;
			return true;
		}
		//hit projectile
		for (int i = index + 1; i < projectiles.size(); i++) {
			if (projectiles[i]->x == x) {
				if (projectiles[i]->y == y - 2) {
					projectiles[i]->dying = true;
					dying = true;
					Move(); // move closer to the collided projectile
					return true;
				}
				if (projectiles[i]->y == y - 1) {
					projectiles[i]->dying = true;
					dying = true;
					return true;
				}
			}
		}

	}
	return false;

}

void Projectile::DieAnimation() {
	switch (dieanimationFrame) {
	case 0:
		graphic = '*';
		break;
	case 5:
		graphic = ' ';
		dying = false;
		break;
	}
	cout << GREY;
	Update_object_pos_on_screen(x, y, x, y, &graphic, height, width);
	cout << WHITE;
	dieanimationFrame++;

}

void Projectile::Move() {
	cout << YELLOW;
	int deltay = 0;
	if (direction == "up") deltay = 1;
	if (direction == "down") deltay = -1;
	Update_object_pos_on_screen(x,y,x,y + deltay,&graphic,height,width);
	y += deltay;
	cout << WHITE;
}
