#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

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
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
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
		//функция настройки камеры
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
		glDisable(GL_LIGHTING);

		
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
void get_normal(double x, double y, double z, double x2, double y2, double z2, double& i, double& j, double& k) {
	i = y * z2 - z * y2;
	j = -1 * (x * z2 - z * 3);
	k = x * y2 - y * x2;
	double l = sqrt(pow(i, 2) + pow(j, 2) + pow(k, 2));
	i /= l;
	j /= l;
	k /= l;
	//return l;

}
/*void draw_quad(double A[], double B[], double B2[], double A2[], bool face_side) {
	double i, j, k;
	get_normal(A[0] - B[0], A[1] - B[1], A[2] - B[2], B2[0] - B[0], B2[1] - B[1], B2[2] - B[2], i, j, k);
	
	glBegin(GL_QUADS);

	glColor3d(0, 1, 0);
	glNormal3d(i, j, k);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(B2);
	glVertex3dv(A2);
	if (!face_side){
		i *= -1;
		j *= -1;
		k *= -1;
	}
	
	glEnd();
	glBegin(GL_LINES);

	
	glVertex3d(B[0],B[1],B[2]);
	glVertex3d(B[0]+i, B[1]+j, B[2]+k);

	glEnd();
}
*/

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	

	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE *texarray;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//генерируем ИД для текстуры
	glGenTextures(1, &texId);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//отчистка памяти
	free(texCharArray);
	free(texarray);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL *ogl)
{


	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//альфаналожение
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	textureMode = false;


	//настройка материала
	GLfloat amb[] = { 0.2, 0.6, 0.1, 1. };
	GLfloat dif[] = { 0.3, 0.3, 0.3, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//размер блика
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  
	//призма
	double A[] = { 0,0,0 };
	double B[] = { 5, 2, 0 };
	double C[] = { 0, 7, 0 };
	double D[] = { 2, 11, 0 };
	double E[] = { -2, 8, 0 };
	double F[] = { -6, 10, 0 };
	double G[] = { -15, 6, 0 };
	double H[] = { -2, 6, 0 };

	double A2[] = { 0,0,3 };
	double B2[] = { 5,2, 3 };
	double C2[] = { 0,7, 3 };
	double D2[] = { 2, 11, 3 };
	double E2[] = { -2, 8, 3 };
	double F2[] = { -6, 10, 3 };
	double G2[] = { -15, 6, 3 };
	double H2[] = { -2, 6, 3 };

	double i;
	double j;
	double k;

	glBegin(GL_TRIANGLES);
	glColor3d(1, 0, 0);

	//get_normal(B[0] - A[0], B[1] - A[1], B[2] - A[2], C[0] - A[0], C[1] - A[1], C[2] - A[2],i,j,k);
	glNormal3d(0, 0, -1);

	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);

	glVertex3dv(A);
	glVertex3dv(C);
	glVertex3dv(H);

	glVertex3dv(C);
	glVertex3dv(G);
	glVertex3dv(H);

	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(E);

	glVertex3dv(G);//// было G  D
	glVertex3dv(E);
	glVertex3dv(H);
	//glColor3d(1, 0, 0);

	glVertex3dv(H);
	glVertex3dv(E);
	glVertex3dv(C);

	glVertex3dv(G);
	glVertex3dv(E);
	glVertex3dv(F);

	glNormal3d(0, 0, 1);


	glVertex3dv(A2);
	glVertex3dv(B2);
	glVertex3dv(C2);

	glVertex3dv(A2);
	glVertex3dv(C2);
	glVertex3dv(H2);

	glVertex3dv(C2);
	glVertex3dv(G2);
	glVertex3dv(H2);

	glVertex3dv(C2);
	glVertex3dv(D2);
	glVertex3dv(E2);

	glVertex3dv(G2);//
	glVertex3dv(E2);
	glVertex3dv(H2);

	glVertex3dv(H2);
	glVertex3dv(E2);
	glVertex3dv(C2);

	glVertex3dv(G2);
	glVertex3dv(E2);
	glVertex3dv(F2);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);

	glBegin(GL_QUADS);

	glColor3d(0, 1, 0);

	get_normal(A[0] - B[0], A[1] - B[1], A[2] - B[2], B2[0] - B[0], B2[1] - B[1], B2[2] - B[2], i, j, k);
	glNormal3d(i, j, k);



	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(B2);
	glVertex3dv(A2);


	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(C2);
	glVertex3dv(B2);
	get_normal(D[0] - C[0], D[1] - C[1], D[2] - C[2], C2[0] - C[0], C2[1] - C[1], C2[2] - C[2], i, j, k);
	glNormal3d(-i, -j, -k);

	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(D2);
	glVertex3dv(C2);

	get_normal(D[0] - D2[0], D[1] - D2[1], D[2] - D2[2], E[0] - D2[0], E[1] - D2[1], E[2] - D2[2], i, j, k);
	glNormal3d(-i, -j, -k);

	glVertex3dv(D);
	glVertex3dv(E);
	glVertex3dv(E2);
	glVertex3dv(D2);

	get_normal(F[0] - E[0], F[1] - E[1], F[2] - E[2], E2[0] - E[0], E2[1] - E[1], E2[2] - E[2], i, j, k);
	glNormal3d(-i, -j, -k);

	glVertex3dv(E);
	glVertex3dv(F);
	glVertex3dv(F2);
	glVertex3dv(E2);

	get_normal(G[0] - F[0], G[1] - F[1], G[2] - F[2], G2[0] - F[0], G2[1] - F[1], G2[2] - F[2], i, j, k);
	glNormal3d(-i, -j, -k);

	glVertex3dv(F);
	glVertex3dv(G);
	glVertex3dv(G2);
	glVertex3dv(F2);

	get_normal(H[0] - G[0], H[1] - G[1], H[2] - G[2], H2[0] - G[0], H2[1] - G[1], H2[2] - G[2], i, j, k);
	glNormal3d(-i, -j, -k);

	glVertex3dv(G);
	glVertex3dv(H);
	glVertex3dv(H2);
	glVertex3dv(G2);

	get_normal(A[0] - H[0], A[1] - H[1], A[2] - H[2], A2[0] - H[0], A2[1] - H[1], A2[2] - H[2], i, j, k);
	glNormal3d(-i, -j, -k);

	glVertex3dv(H);
	glVertex3dv(A);
	glVertex3dv(A2);
	glVertex3dv(H2);

	glEnd();

	/*glBegin(GL_QUAD_STRIP);
	
	glColor3d(0, 0, 1);
	double Et = 0.02;
	double d = 2;
	double Xc = B[0];
	double Xc2 = C[0];
	double Yc2 = C[1];
	double Yc = B[1];
	double z = B[2];
	double z2 = C2[2];
	d = sqrt(pow((Xc2 - Xc), 2) + pow((Yc2 - Yc), 2)) / 2;
	int vector1[2];
	int vector2[2];
	vector1[0] = Xc2 - Xc;
	vector1[1] = Yc2 - Yc;
	double Xc_b = Xc;
	double Yc_b = Yc;
	Xc = (Xc2 + Xc) / 2;
	Yc = (Yc2 + Yc) / 2;
	vector2[0] = Xc - Xc_b;
	vector2[1] = Yc - Yc_b;
	double cos_fi = (vector1[0] * vector2[0] + vector2[0] * vector2[1]) / (sqrt(pow(vector1[0], 2) + pow(vector1[1], 2)) * sqrt(pow(vector2[0], 2) + pow(vector2[1], 2)));
	double fi = acos(cos_fi);
	//double c = PI * d;
	//double l = fi / 360 * PI;
	for (double t = 0.0 + fi; t <= PI + fi;)
	{
		double x2 = d * cos(t) + Xc;
		double y2 = d * sin(t) + Yc;
		t += Et;
		double x = d * cos(t) + Xc;
		double y = d * sin(t) + Yc;

		get_normal(x2-x, y2-y, z2-z, 0, 0,z2-z, i, j, k);
		if(t>((PI+fi)/2))glNormal3d(i, j, -k);
		else glNormal3d(i, j, k);

		glVertex3d(x, y, z);
		glVertex3d(x, y, z2);
		glVertex3d(x2, y2, z2);
		glVertex3d(x2, y2, z);

	}
	glEnd();
	Et = 0.01;
	glNormal3d(0, 0, -1);
	glBegin(GL_TRIANGLE_FAN);
	double angle = 0.0 + fi;
	double x;
	double y;
	glColor3d(0.5, 1, 0.5);
	while (angle <= PI + fi) {
		x = d * cos(angle) + Xc;
		y = d * sin(angle) + Yc;
		glVertex3d(x, y, z);
		angle = angle + Et;
	}
	x = d * cos(angle) + Xc;
	y = d * sin(angle) + Yc;
	glVertex3d(x, y, z);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texId);
	glColor4d(0.5, 1, 0.5, 0.8);

	glNormal3d(0, 0, 1);
	glBegin(GL_TRIANGLE_FAN);
	angle = 0.0 + fi;
	while (angle <= PI + fi) {
		x = d * cos(angle) + Xc;
		y = d * sin(angle) + Yc;
		glVertex3d(x, y, z2);
		angle = angle + Et;
	}
	x = d * cos(angle) + Xc;
	y = d * sin(angle) + Yc;
	glVertex3d(x, y, z2);
	glEnd();*/
	//призма
	
   //Сообщение вверху экрана

	
	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
	                                //(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции

	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - вкл/выкл текстур" << std::endl;
	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "F - Свет из камеры" << std::endl;
	ss << "G - двигать свет по горизонтали" << std::endl;
	ss << "G+ЛКМ двигать свет по вертикали" << std::endl;
	ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "Параметры камеры: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}
/*double A[] = { 1,5,1 };
	double B[] = { 3, 12, 1 };
	double C[] = { 11, 13, 1 };
	double D[] = { 11, 7, 1 };
	double E[] = { 16, 6, 1 };
	double F[] = { 15, 1, 1 };
	double G[] = { 9, 5, 1 };
	double H[] = { 5, 2, 1 };

	double A2[] = { 1,5,3 };
	double B2[] = { 3, 12, 3 };
	double C2[] = { 11, 13, 3 };
	double D2[] = { 11, 7, 3 };
	double E2[] = { 16, 6, 3 };
	double F2[] = { 15, 1, 3 };
	double G2[] = { 9, 5, 3 };
	double H2[] = { 5, 2, 3 };
	
	double i;
	double j;
	double k;

	glBegin(GL_TRIANGLES);
	glColor3d(1, 0, 0);

	//get_normal(B[0] - A[0], B[1] - A[1], B[2] - A[2], C[0] - A[0], C[1] - A[1], C[2] - A[2],i,j,k);
	glNormal3d(0,0,-1);

	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);

	glVertex3dv(A);
	glVertex3dv(C);
	glVertex3dv(H);

	glVertex3dv(C);
	glVertex3dv(G);
	glVertex3dv(H);

	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(G);

	//glColor3d(1, 0, 0);

	glVertex3dv(D);
	glVertex3dv(E);
	glVertex3dv(G);

	glVertex3dv(G);
	glVertex3dv(E);
	glVertex3dv(F);

	glNormal3d(0, 0, 1);


	glVertex3dv(A2);
	glVertex3dv(B2);
	glVertex3dv(C2);

	glVertex3dv(A2);
	glVertex3dv(C2);
	glVertex3dv(H2);

	glVertex3dv(C2);
	glVertex3dv(G2);
	glVertex3dv(H2);

	glVertex3dv(C2);
	glVertex3dv(D2);
	glVertex3dv(G2);

	glVertex3dv(D2);
	glVertex3dv(E2);
	glVertex3dv(G2);

	glVertex3dv(G2);
	glVertex3dv(E2);
	glVertex3dv(F2);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);

	glBegin(GL_QUADS);

	glColor3d(0, 1, 0);

	get_normal(A[0] - B[0], A[1] - B[1], A[2] - B[2], B2[0] - B[0], B2[1] - B[1], B2[2] - B[2], i, j, k);
	glNormal3d(i, j, k);


	
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(B2);
	glVertex3dv(A2);

	//draw_quad(A, B, B2, A2, true);

	//glDeleteTextures(GL_TEXTURE_2D, &texId);
	/*glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(C2);
	glVertex3dv(B2);*/
/*get_normal(D[0] - C[0], D[1] - C[1], D[2] - C[2], C2[0] - C[0], C2[1] - C[1], C2[2] - C[2], i, j, k);
glNormal3d(-i, -j, -k);

glVertex3dv(C);
glVertex3dv(D);
glVertex3dv(D2);
glVertex3dv(C2);

get_normal(D[0] - D2[0], D[1] - D2[1], D[2] - D2[2], E[0] - D2[0], E[1] - D2[1], E[2] - D2[2], i, j, k);
glNormal3d(-i, -j, -k);

glVertex3dv(D);
glVertex3dv(E);
glVertex3dv(E2);
glVertex3dv(D2);

get_normal(F[0] - E[0], F[1] - E[1], F[2] - E[2], E2[0] - E[0], E2[1] - E[1], E2[2] - E[2], i, j, k);
glNormal3d(-i, -j, -k);

glVertex3dv(E);
glVertex3dv(F);
glVertex3dv(F2);
glVertex3dv(E2);

get_normal(G[0] - F[0], G[1] - F[1], G[2] - F[2], G2[0] - F[0], G2[1] - F[1], G2[2] - F[2], i, j, k);
glNormal3d(-i, -j, -k);

glVertex3dv(F);
glVertex3dv(G);
glVertex3dv(G2);
glVertex3dv(F2);

get_normal(H[0] - G[0], H[1] - G[1], H[2] - G[2], H2[0] - G[0], H2[1] - G[1], H2[2] - G[2], i, j, k);
glNormal3d(-i, -j, -k);

glVertex3dv(G);
glVertex3dv(H);
glVertex3dv(H2);
glVertex3dv(G2);

get_normal(A[0] - H[0], A[1] - H[1], A[2] - H[2], A2[0] - H[0], A2[1] - H[1], A2[2] - H[2], i, j, k);
glNormal3d(-i, -j, -k);

glVertex3dv(H);
glVertex3dv(A);
glVertex3dv(A2);
glVertex3dv(H2);

glEnd(); */