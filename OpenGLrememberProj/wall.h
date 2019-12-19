#pragma once
#include <windows.h>

#include "ObjLoader.h"
#include <GL\gl.h>
#include <GL\glu.h>
#include "GL\glext.h"
#include <string>
#include <math.h>
#include "MyVector3d.h"

using namespace std;

class Wall {
	char* obj_way;
	ObjFile obj;
	float angle;
	Vector3 os_rotate;
public:
	Vector3 pos;
	Wall(char*);
	void Load(char *);
	void Draw();
	void Rotate(float, float, float, float);
	void Edit_pos(float, float, float);
	Texture tex;
};

Wall::Wall(char* way)
{
	obj_way = way;
	pos.setCoords(0, 0, 0);
	angle = 0;
	os_rotate = *new Vector3(0, 0, 1);
}

void Wall::Load(char *texture)
{
	tex.loadTextureFromFile(texture);
	loadModel(obj_way, &obj);
}

void Wall::Draw()
{
	glPushMatrix();
	glTranslatef(pos.X(), pos.Y(), pos.Z());
	//glRotatef(angle, os_rotate.X(), os_rotate.Y(), os_rotate.Z());
	tex.bindTexture();
	obj.DrawObj();
	glPopMatrix();
}

void Wall::Rotate(float ang, float x, float y, float z)
{
	angle = ang;
	os_rotate.setCoords(x, y, z);
}

void Wall::Edit_pos(float x, float y, float z)
{
	pos.setCoords(x, y, z);
}