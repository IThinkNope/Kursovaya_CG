#pragma once
#include <windows.h>
#include <iostream>

#include "ObjLoader.h"
#include <GL\gl.h>
#include <GL\glu.h>
#include "GL\glext.h"
#include <string>
#include <math.h>

using namespace std;

class Player {
	char* obj_way;
	ObjFile obj;
	float dx, dy;
	float speed;
	float angle;
	Vector3 os_rotate;
	Vector3 scale;
	bool back;
	Texture tex;
	int xp;
public:
	Vector3 pos;
	float W, H;
	Player(char*);
	void Load(char *);
	void Draw(bool);
	void Keys();
	void Rotate(float, float, float, float);
	void Edit_pos(float, float, float);
	bool Collis(int map[100][100]);
	void Animation_play();
	void Animation_stop();
};

Player::Player(char* way)
{
	obj_way = way;
	dx = 0;
	dy = 0;
	W = 2;
	H = 2;
	xp = 3;
	pos.setCoords(0, 0, 0);
	speed = 0.4;
	angle = 0;
	os_rotate = *new Vector3(0,0,1);
	scale.setCoords(1, 1, 1);
	back = false;
}

void Player::Load(char *texture)
{
	tex.loadTextureFromFile(texture);
	loadModel(obj_way, &obj);
}

void Player::Draw(bool mod)
{
	glPushMatrix();
	if (mod == true)
	{
		Keys();
		Animation_play();
		if (dx == 0 && dy == 0)
			Animation_stop();
		pos.setCoords(pos.X() + dx * speed, pos.Y() + dy * speed, pos.Z());
		glTranslatef(pos.X(), pos.Y(), pos.Z());
		glRotatef(angle, os_rotate.X(), os_rotate.Y(), os_rotate.Z());
		glRotatef(90, 1, 0, 0);
		glRotatef(-90, 0, 1, 0);
		glScalef(scale.X(), scale.Y(), scale.Z());
		dx = 0;
		dy = 0;
	}
	else {
		angle += 4;
		glRotatef(angle, os_rotate.X(), os_rotate.Y(), os_rotate.Z());
		glRotatef(90, 1, 0, 0);
		glRotatef(90, 0, 1, 0);
		glScalef(scale.X(), scale.Y(), scale.Z());
	}

	tex.bindTexture();
	obj.DrawObj();
	glPopMatrix();
}

void Player::Keys() {
	if (OpenGL::isKeyPressed('W'))
	{
		dx = -cos(angle/180*PI);
		dy = -sin(angle/180*PI);
	}
	if (OpenGL::isKeyPressed('D'))
	{
		angle -= 4;
	}
	if (OpenGL::isKeyPressed('S'))
	{
		dx = cos(angle / 180 * PI);
		dy = sin(angle / 180 * PI);
	}
	if (OpenGL::isKeyPressed('A'))
	{
		angle += 4;
	}
	Animation_play();
}

void Player::Rotate(float ang, float x, float y, float z)
{
	angle = ang;
	os_rotate.setCoords(x, y, z);
}

void Player::Edit_pos(float x, float y, float z)
{
	pos.setCoords(x, y, z);
}

bool Player::Collis(int map[100][100]) {
	if (pos.Y() <= 1 * 3 - 5 * 3)
	{
		pos.setCoords(pos.X(), 3 - 5 * 3, pos.Z());
		dx = 0;
		dy = 0;
	}
	if (pos.Y() >= 8 * 3 - 5 * 3)
	{
		pos.setCoords(pos.X(), 8 * 3 - 5 * 3, pos.Z());
		dx = 0;
		dy = 0;
	}
	if (pos.X() <= 1 * 3 - 5 * 3)
	{
		pos.setCoords(3 - 5 * 3, pos.Y(), pos.Z());
		dx = 0;
		dy = 0;
	}
	if (pos.X() >= 8 * 3 - 5 * 3)
	{
		pos.setCoords(8 * 3 - 5 * 3, pos.Y(), pos.Z());
		dx = 0;
		dy = 0;
	}
	return false;
}

void Player::Animation_play() {
	if (scale.X() > 0.9 && back == false)
	{
		scale.setCoords(scale.X() - 0.005, scale.Y() - 0.005, scale.Z() - 0.005);
	}
	else
		back = true;
	if (scale.X() < 1 && back == true)
	{
		scale.setCoords(scale.X() + 0.005, scale.Y() + 0.005, scale.Z() + 0.005);
	}
	else
		back = false;
}

void Player::Animation_stop() {
	scale.setCoords(1, 1, 1);
}