#include "Render.h"

#include <iostream>
#include <cstdlib>
#include <windows.h>

#include <GL\gl.h>
#include <GL\glu.h>
#include "GL\glext.h"

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "MyShaders.h"

#include "ObjLoader.h"
#include "GUItextRectangle.h"

#include "Texture.h"
#include "Player.h"
#include "wall.h"
#include "Bonus.h"
#include "Enemy.h"

#include "Render.h"
#include <sstream>
#include "MyOGL.h"

bool play_mod = false;
int map[100][100];
int score = 0;
char score_char[10];
string score_out;

GuiTextRectangle rec;

bool textureMode = true;
bool lightMode = true;


//небольшой дефайн для упрощения кода
#define POP glPopMatrix()
#define PUSH glPushMatrix()


ObjFile *model;

Texture texture1;
Texture sTex;
Texture rTex;
Texture tBox;
Texture SkyBoxSides;
Texture score_tex;

Shader s[10];  //массивчик для десяти шейдеров
Shader frac;
Shader cassini;
list<Enemy*>Enemies;



void IntChar(int num)
{
	int i = 0, j = 0;
	char rev_num_ch[10];
	while (num != 0)
	{
		rev_num_ch[i] = num % 10 + 48;
		num /= 10;
		i++;
	}
	i--;
	score_char[0] = 48;
	while (i >= 0)
	{
		score_char[j] = rev_num_ch[i];
		j++;
		i--;
	}
}


//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	virtual void SetUpCamera()
	{

		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		Shader::DontUseShaders();
		bool f1 = glIsEnabled(GL_LIGHTING);
		glDisable(GL_LIGHTING);
		bool f2 = glIsEnabled(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_2D);
		bool f3 = glIsEnabled(GL_DEPTH_TEST);
		
		glDisable(GL_DEPTH_TEST);
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//линия от источника света до окружности
				glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}
	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света



//старые координаты мыши
int mouseX = 0, mouseY = 0;




float offsetX = 0, offsetY = 0;
float zoom=1;
float Time = 0;
int tick_o = 0;
int tick_n = 0;

//обработчик движения мыши
void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON) && play_mod == false)
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}


	if (OpenGL::isKeyPressed(VK_LBUTTON) && play_mod == false)
	{
		offsetX -= 1.0*dx/ogl->getWidth()/zoom;
		offsetY += 1.0*dy/ogl->getHeight()/zoom;
	}
}

//обработчик вращения колеса  мыши
void mouseWheelEvent(OpenGL *ogl, int delta)
{


	float _tmpZ = delta*0.003;
	if (ogl->isKeyPressed('Z'))
		_tmpZ *= 10;
	zoom += 0.2*zoom*_tmpZ;


	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;
}

//обработчик нажатия кнопок клавиатуры
void keyDownEvent(OpenGL *ogl, int key)
{
	if (OpenGL::isKeyPressed('L') && play_mod == false)
	{
		lightMode = !lightMode;
	}

	if (OpenGL::isKeyPressed('T') && play_mod == false)
	{
		textureMode = !textureMode;
	}	   

	if (key == 'R'  && play_mod == false)
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F'  && play_mod == false)
	{
		light.pos = camera.pos;
	}

	if (key == 'S'  && play_mod == false)
	{
		frac.LoadShaderFromFile();
		frac.Compile();

		s[0].LoadShaderFromFile();
		s[0].Compile();

		cassini.LoadShaderFromFile();
		cassini.Compile();
	}

	if (OpenGL::isKeyPressed('O'))
	{
		play_mod = !play_mod;
		if (play_mod == true)
		{
			camera.fi1 = 0;
			camera.fi2 = 45;
			camera.camDist = 60;
			light.pos.setCoords(12 * 3, 12, 40);
			Enemies.push_back(new Enemy("textures\\Enemy.bmp"));
		}
		else
		{
			camera.fi1 = 0.8;
			camera.fi2 = 0.45;
			camera.camDist = 20;
			score = 0;
			Enemies.clear();
		}
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{

}

void DrawQuad()
{
	double A[] = { 0,0 };
	double B[] = { 2,0 };
	double C[] = { 2,2 };
	double D[] = { 0,2 };
	glBegin(GL_QUADS);
	glColor3d(.5, 0, 0);
	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 1);
	glVertex2dv(A);
	glTexCoord2d(0, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 0);
	glVertex2dv(C);
	glTexCoord2d(1, 1);
	glVertex2dv(D);
	glEnd();
}

void SkyBox() {
	float W = 60;
	glColor3b(1, 1, 1);
	glBegin(GL_POLYGON);
	glVertex3f(W, -W, -5);
	glVertex3f(W, W, -5);
	glVertex3f(-W, W, -5);
	glVertex3f(-W, -W, -5);
	glEnd();

	// Фиолетовая сторона — ПРАВАЯ
	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3f(-30, -W, -W);
	glTexCoord2d(1, 0);
	glVertex3f(-30, W, -W);
	glTexCoord2d(1, 1);
	glVertex3f(-30, W, W);
	glTexCoord2d(0, 1);
	glVertex3f(-30, -W, W);
	glEnd();

	// Зеленая сторона — ЛЕВАЯ
	glBegin(GL_POLYGON);
	glTexCoord2d(1, 0);
	glVertex3f(-W, -W, W);
	glTexCoord2d(1, 1);
	glVertex3f(-W, W, W);
	glTexCoord2d(0, 1);
	glVertex3f(-W, W, -W);
	glTexCoord2d(0, 0);
	glVertex3f(-W, -W, -W);
	glEnd();

	// Синяя сторона — ВЕРХНЯЯ
	glBegin(GL_POLYGON);
	glTexCoord2d(1, 1);
	glVertex3f(W, W, W);
	glTexCoord2d(1, 0);
	glVertex3f(W, W, -W);
	glTexCoord2d(0, 0);
	glVertex3f(-W, W, -W);
	glTexCoord2d(0, 1);
	glVertex3f(-W, W, W);
	glEnd();

	// Красная сторона — НИЖНЯЯ
	glBegin(GL_POLYGON);
	glTexCoord2d(1, 1);
	glVertex3f(W, -W, -W);
	glTexCoord2d(1, 0);
	glVertex3f(W, -W, W);
	glTexCoord2d(0, 0);
	glVertex3f(-W, -W, W);
	glTexCoord2d(0, 1);
	glVertex3f(-W, -W, -W);
	glEnd();
}


ObjFile objModel,monkey;
Texture monkeyTex;
Texture SnowMan;

Player ply("models\\Player.obj");
Wall wall("models\\Wall.obj");
Bonus snow("models\\Bonus.obj");

//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	camera.fi1 = 0.8;
	camera.fi2 = 0.45;
	camera.camDist = 20;
	light.pos.setCoords(12 * 3, 12, 40);
	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	
	


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	//ogl->mainCamera = &WASDcam;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);


	frac.VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	frac.FshaderFileName = "shaders\\frac.frag"; //имя файла фрагментного шейдера
	frac.LoadShaderFromFile(); //загружаем шейдеры из файла
	frac.Compile(); //компилируем

	cassini.VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	cassini.FshaderFileName = "shaders\\cassini.frag"; //имя файла фрагментного шейдера
	cassini.LoadShaderFromFile(); //загружаем шейдеры из файла
	cassini.Compile(); //компилируем
	

	s[0].VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	s[0].FshaderFileName = "shaders\\light.frag"; //имя файла фрагментного шейдера
	s[0].LoadShaderFromFile(); //загружаем шейдеры из файла
	s[0].Compile(); //компилируем

	s[1].VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	s[1].FshaderFileName = "shaders\\textureShader.frag"; //имя файла фрагментного шейдера
	s[1].LoadShaderFromFile(); //загружаем шейдеры из файла
	s[1].Compile(); //компилируем

	
	wall.Load("textures\\ice.bmp");
	snow.Load();
	score_tex.loadTextureFromFile("textures\\texture.bmp");
	SkyBoxSides.loadTextureFromFile("textures\\Box.bmp");


	glActiveTexture(GL_TEXTURE0);
	ply.Load("textures\\sTex.bmp");


	tick_n = GetTickCount();
	tick_o = tick_n;

	rec.setSize(300, 100);
	rec.setPosition(0, -50);
	rec.setText("Groovy Snowman\nPress \"O\" to start",0,0,0);

	for (int i = 0; i < 10; i++)
	{
		map[0][i] = 1;
		map[i][0] = 1;
		map[9][i] = 1;
		map[i][9] = 1;
	}
	ply.Edit_pos(4*3 - 5*3, 4*3 - 5 * 3, 0);
}

void Render(OpenGL *ogl)
{   
	tick_o = tick_n;
	tick_n = GetTickCount();
	Time += (tick_n - tick_o) / 1000.0;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);

	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;

	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
		{
			if (map[i][j] == 1)
			{
				wall.Edit_pos(j*3 - 5 * 3, i*3 - 5 * 3, wall.pos.Z());
				wall.Draw();
			}
		}

	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, new GLfloat[4]{ 0.2, 0.2, 0.1, 1. });
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, new GLfloat[4]{ 0.6, 0.7, 0.6, 1. });
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, new GLfloat[4]{ 0.9, 0.4, 0.64, 1. });
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, 10);
	snow.Draw(play_mod);
	if (snow.Collide(ply.pos, ply.W, ply.H))
	{
		score++;
		if (score % 3 == 0)
		{
			Enemies.push_back(new Enemy("textures\\Enemy.bmp"));
		}
	}
	for (list<Enemy*>::iterator it = Enemies.begin(); it != Enemies.end(); it++)
	{
		if ((*it)->Collide(ply.pos, ply.W, ply.H))
		{
			camera.fi1 = 0.8;
			camera.fi2 = 0.45;
			camera.camDist = 20;
			score = 0;
			Enemies.clear();
			play_mod = false;
			break;
		}
		(*it)->Draw(play_mod);
	}
	for (int i = 0, j = 0, k = 0; i < score; i++, k++)
	{
		score_tex.bindTexture();
		glPushMatrix();
		glTranslatef(20 + j*3, -20 + k*3, 0);
		DrawQuad();
		glPopMatrix();
		if (k > 11)
		{
			j++;
			k = -1;
		}
	}
	ply.Collis(map);
	ply.Draw(play_mod);
	SkyBoxSides.bindTexture();
	SkyBox();
}



//рисует интерфейс, вызывется после обычного рендера
void RenderGUI(OpenGL *ogl)
{
	
	Shader::DontUseShaders();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	
	glActiveTexture(GL_TEXTURE0);
	if (play_mod == false)
	{
		rec.Draw();
	}
		
	Shader::DontUseShaders(); 
}

void resizeEvent(OpenGL *ogl, int newW, int newH)
{
	rec.setPosition(10, newH - 100 - 10);
}

