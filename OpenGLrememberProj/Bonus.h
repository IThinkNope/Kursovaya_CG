#pragma once
#include <windows.h>
#include <iostream>

#include "ObjLoader.h"
#include <GL\gl.h>
#include <GL\glu.h>
#include "GL\glext.h"
#include <string>
#include <math.h>
#include "MyVector3d.h"

using namespace std;

class Bonus {
	char* obj_way;
	ObjFile obj;
	int dx, dy;
	float speed;
	float angle;
	Vector3 os_rotate;
	Vector3 pos;
	Vector3 scale;
	bool back;
	float W, H;
public:
	Bonus(char*);
	void Load();
	void Draw(bool);
	void Rotate(float, float, float, float);
	void Edit_pos(float, float, float);
	void Animation_play();
	bool Collide(Vector3, float, float);
};

Bonus::Bonus(char* way)
{
	obj_way = way;
	dx =(rand() % 3) - 2;
	if (dx == 0)
		dy = 1;
	else
		dy =(rand() % 3) - 2;
	W = 2;
	H = 2;
	pos.setCoords(50*dx + (rand()%21) - 10, 50*dy + (rand() % 21) - 10, 0);
	speed = 0.5;
	angle = 0;
	os_rotate = *new Vector3(0, 0, 1);
	scale.setCoords(0.5, 0.5, 0.5);
	back = false;
}

void Bonus::Load()
{
	loadModel(obj_way, &obj);
}

void Bonus::Draw(bool mod)
{
	Animation_play();
	glPushMatrix();
	if (mod == true)
	{
		pos.setCoords(pos.X() - dx * speed, pos.Y() - dy * speed, pos.Z());
		glTranslatef(pos.X(), pos.Y(), pos.Z());
		glRotatef(angle, os_rotate.X(), os_rotate.Y(), os_rotate.Z());
		glRotatef(90, 1, 0, 0);
		glRotatef(-90, 0, 1, 0);
		glScalef(scale.X(), scale.Y(), scale.Z());
		if (pos.X() > 60 || pos.Y() > 60 || pos.X() < -60 || pos.Y() < -60)
		{
			dx = (rand() % 3) - 2;
			if (dx == 0)
				dy = 1;
			else
				dy = (rand() % 3) - 2;
			pos.setCoords(50 * dx + (rand() % 21) - 10, 50 * dy + (rand() % 21) - 10, 0);
		}
	}
	else
	{
		angle += 4;
		glRotatef(angle, os_rotate.X(), os_rotate.Y(), os_rotate.Z());
		glTranslatef(2, 2, 0);
		glRotatef(90, 1, 0, 0);
		glRotatef(-90, 0, 1, 0);
		glScalef(scale.X(), scale.Y(), scale.Z());
	}
	obj.DrawObj();
	glPopMatrix();
}

void Bonus::Rotate(float ang, float x, float y, float z)
{
	angle = ang;
	os_rotate.setCoords(x, y, z);
}

void Bonus::Edit_pos(float x, float y, float z)
{
	pos.setCoords(x, y, z);
}

void Bonus::Animation_play() {
	angle += 5;
}

bool Bonus::Collide(Vector3 o_pos, float o_W, float o_H) {
	if ((o_pos.X() > pos.X() && o_pos.X() < pos.X() + W) && (o_pos.Y() > pos.Y() && o_pos.Y() < pos.Y() + H) ||
		(o_pos.X() + o_W > pos.X() && o_pos.X() + o_W < pos.X() + W) && (o_pos.Y() + o_H > pos.Y() && o_pos.Y() + o_H < pos.Y() + H))
	{
		dx = (rand() % 3) - 2;
		if (dx == 0)
			dy = 1;
		else
			dy = (rand() % 3) - 2;
		pos.setCoords(50 * dx + (rand() % 21) - 10, 50 * dy + (rand() % 21) - 10, 0);
		return true;
	}
	return false;
}