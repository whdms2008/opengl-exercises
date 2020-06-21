// SKKU Computer Graphics (COM3013) - Final Project : Snow Globe
// 2015312229 Yoseob Kim

#define _CRT_SECURE_NO_WARNINGS // fscanf, fgets ����� ���� ��� �����ϱ� (Visual Studio ȯ���� �ƴ� ��� �ڵ带 ������ ��)

#include<windows.h>
#include<MMSystem.h> // �Ҹ� ����� ���� ���

// C headers (C++ type)
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <GL/glut.h>

// C++ headers
#include <iostream>
#include <iomanip> // �ܼ� ��� (����� �뵵)������ �ε� �Ҽ��� ���е��� ���� ��� ����
#include <ctime>

// tinkercad�� �׸� ������ -> .obj ������ �Ľ� �� ���� �迭�� ��ȯ

GLuint vlist[60000][3]; // GLubyte�� �ִ� ������ 255���� �Ѿ�� ������ GLuint�� ����
GLint vlist_size, vertex_info_size;
GLfloat vertex_info[30000][3];
GLfloat color_info[30000][3];

int snowflakeID; // ������ ���� display list�� ó��, �ش� ����Ʈ�� ID
int treeupID, treedownID;

int id_array[10];

void CreateList(int param) { // .obj ���Ͽ��� �Ľ��� �� ���� �迭�� �̿��� ���÷��� ����Ʈ ����
	id_array[param-1] = glGenLists(param);
	glNewList(id_array[param - 1], GL_COMPILE);

	for (int i = 0; i < vlist_size; ++i) {
		// �⺻ ���̵��: ������ ���� �ﰢ�� ���� �̿��Ͽ� �ϳ��� �ϼ��� 3D object�� �׸���.
		glBegin(GL_TRIANGLES); // �̸� �̿��ϱ� ���� GL_TRIANGLES

		if (param == 1) {
			glColor3f(1.0, 1.0, 1.0); // ������ �Ͼ� ������ �ʱ�ȭ (Ư���� �ǹ� ����)
		}
		else if (param == 2) {
			glColor3f(0.0, 0.5, 0.0);
		}
		else if (param == 3) {
			glColor3f(0.5, 0.0, 0.0);
		}
		
		// ���� �迭�� index�� Ȱ���Ͽ� �ϳ��� �ﰢ���� �� ���� ���� (k=0~2 �� �� ��, vlist[i][k] �� �ϳ��� vertex ������ ǥ��)
		// u=������ ���̶� �� ��, vertex_info[u][0~2] �� �� ���� x, y, z ������ ����
		glVertex3f(vertex_info[vlist[i][0]][0], vertex_info[vlist[i][0]][1], vertex_info[vlist[i][0]][2]);
		glVertex3f(vertex_info[vlist[i][1]][0], vertex_info[vlist[i][1]][1], vertex_info[vlist[i][1]][2]);
		glVertex3f(vertex_info[vlist[i][2]][0], vertex_info[vlist[i][2]][1], vertex_info[vlist[i][2]][2]);
		/*
		// ����� ����Ʈ ������ �Ǵ��� ���� ���� ����� ��
		printf("%d��° ��: p1(%f, %f, %f)-p2(%f, %f, %f)-p3(%f, %f, %f)\n", i,
			vertex_info[vlist[i][0]][0], vertex_info[vlist[i][0]][1], vertex_info[vlist[i][0]][2],
			vertex_info[vlist[i][1]][0], vertex_info[vlist[i][1]][1], vertex_info[vlist[i][1]][2],
			vertex_info[vlist[i][2]][0], vertex_info[vlist[i][2]][1], vertex_info[vlist[i][2]][2]); */

		glEnd();
	}
	glEndList();
}


void loadObject(const char* path) {
	// �ܺ��� �𵨸��� �ҷ����� ���� �Լ�
	// https://github.com/opengl-tutorials/ogl/blob/master/common/objloader.cpp �� �����Ͽ�, 
	// ���� �׸��� ������ ������� �Լ��� ���� ���� (���� ������ bool loadOBJ() �Լ� Ȱ��)
	printf("Loading OBJ file %s...\n", path);

	FILE* file = fopen(path, "r");
	vlist_size = vertex_info_size = 0;

	while (true) {
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);

		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if (strcmp(lineHeader, "v") == 0) {
			fscanf(file, "%f %f %f\n", &vertex_info[vertex_info_size][0], &vertex_info[vertex_info_size][1], &vertex_info[vertex_info_size][2]);

			vertex_info_size++;

		}
		else if (strcmp(lineHeader, "f") == 0) {
			unsigned int vertexIndex[3];
			fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);

			vlist[vlist_size][0] = vertexIndex[0] - 1;
			vlist[vlist_size][1] = vertexIndex[1] - 1;
			vlist[vlist_size][2] = vertexIndex[2] - 1;

			vlist_size++;

		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}
	fclose(file);
}

// flatshading�� wireframe�� ��۸��ϱ� ���� ����
int FlatShaded = 0;
int WireFramed = 0;

// ���콺 �����ӿ� ���� ������ �ٲٱ� ���� ����
// ī�޶� ��ġ�� �̵� (X, Z ��ǥ�迡�� ī�޶� �������� ȸ��)
GLfloat ViewX = 0, ViewZ = 1;
GLfloat ViewY = 0;
// �ʱ⿡ ������ ����, ������� ���� �ֿܼ� ����ϴ� ViewX~Z�� �����Ͽ� ���� ���� ���� ������ �������� ����
// : �̷��� �ϸ� ���� �� ���� ���� ���� ������ ��� ��ü���� ��Ÿ����.
GLfloat prevViewX, prevViewY, prevViewZ;

// ������ ������
GLint windowWidth = 800, windowHeight = 600;

GLboolean isClicked = false; // ���콺 Ŭ�� ���θ� �Ǵ�

void MyMouseClick(GLint Button, GLint State, GLint X, GLint Y) { // ���콺 Ŭ���� �Ǿ��� ���� ���� ��ȯ�� ���� (gluLookAt)
	if ((Button == GLUT_LEFT_BUTTON || Button == GLUT_RIGHT_BUTTON) && State == GLUT_DOWN) { // ��, �� ��ư ��� ����, ���콺�� ������ �� Activate
		isClicked = true;
	}
	else { // ���콺 ��ư�� ���� �� Deactivate
		isClicked = false;
	}
}

void MyMouseMove(GLint X, GLint Y) {
	if (isClicked) {
		// ���� ��ȭ�� ���� �۾�

		if (X > 0 && X < windowWidth && Y > 0 && Y < windowHeight) {
			ViewX = (X - (windowWidth / 2.0)) / GLfloat(windowWidth); // ViewX�� ���콺 �������� ���� ����
			ViewY = (Y - (windowHeight / 2.0)) / GLfloat(windowHeight); // ViewY�� ���콺 �������� ���� ����
			if (1 - (ViewX * ViewX + ViewY * ViewY) > 0) { // �������� 1�� ���� �������� ��ü�� ȸ����Ų�ٰ� �����ϰ�, ViewZ�� ���ϱ�
				ViewZ = sqrt(1 - (ViewX * ViewX + ViewY * ViewY));
			}
			else {
				ViewZ = 0;
			}

			// ���� ViewX~Z ���� ���
			prevViewX = ViewX;
			prevViewY = ViewY;
			prevViewZ = ViewZ;
		}
		else { // ������ ���� �����, ����� ������ ���¸� ���
			ViewX = prevViewX;
			ViewY = prevViewY;
			ViewZ = prevViewZ;
		}

		glutPostRedisplay();
	}
}

void det_snowpos();

GLfloat delta;
GLboolean out_view = true;
void MyKeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q': case 'Q': case '\033': // esc key
		exit(0);
		break;

	case 's': case 'S': // Shake
		det_snowpos();
		delta = 0.0f;
		break;
	case 'v': case 'V': // Change View
		out_view = !out_view;
		break;
	}
}

const int max_snow = 100;
GLfloat snowpos[max_snow][3];

void MyDisplay() {
	glViewport(0, 0, windowWidth, windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// ī�޶��� ��ġ�� ������ �ΰ�, ��ü�� x^2+y^2+z^2=1 (�������� 1�� ��)�� �������� ȸ����Ŵ
	// ViewX~Z�� ���ϴ� ���� MyMouseMove() �Լ����� ����
	gluLookAt(0, 0, 0, ViewX, ViewY, ViewZ, 0, 1, 0);
	//std::cout << "ViewX: " << ViewX << " View Y: " << ViewY << " View Z: " << ViewZ << std::endl;

	glPushMatrix(); // 0

	GLfloat lightpos[] = { 1.06 - ViewX, 1.06 - ViewY, 1.06 - ViewZ, 1.2 };

	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	
	//glFrontFace(GL_CCW);

	if (!out_view) {
		glPushMatrix(); // 1
		glEnable(GL_BLEND);

		glPushMatrix(); // 2
		glScalef(0.75, 0.75, 0.75);

		glPushMatrix(); // 3
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	// ���� �׸��� - ū ����
	glPushMatrix(); // 4
	glScalef(0.04, 0.04, 0.04);
	glTranslatef(8, 3, 0);
	if (ViewY > 0) {
		glCallList(id_array[1]);
	}
	else {
		glCallList(id_array[2]);
	}

	// ���� ���� �𵨸�
	glPushMatrix(); // 5
	//glTranslatef()
	if (ViewY > 0) {
		glCallList(id_array[2]);
	}
	else {
		glCallList(id_array[1]);
	}
	glPopMatrix(); // 5 e

	glPopMatrix(); // 4 e


	
	// �� ���� max_snow�� ȣ��
	for (int i = 0; i < max_snow; ++i) {
		glPushMatrix(); // 4
		glScalef(0.004, 0.004, 0.004);
		glTranslatef(snowpos[i][0], snowpos[i][1], snowpos[i][2]);
		glCallList(id_array[0]);
		glPopMatrix(); // 4 e
	}


	// ���� �׸��� - ���� ����
	glPushMatrix(); // 4
	glScalef(0.02, 0.02, 0.02);
	glTranslatef(-15, -15, 0);
	//glCallList(id_array[1]);

	if (ViewY > 0) {
		glCallList(id_array[1]);
	}
	else {
		glCallList(id_array[2]);
	}

	// ���� ���� �𵨸�
	glPushMatrix(); // 5
	//glTranslatef()
	//glCallList(id_array[2]);


	if (ViewY > 0) {
		glCallList(id_array[2]);
	}
	else {
		glCallList(id_array[1]);
	}
	glPopMatrix(); // 5 e

	glPopMatrix(); // 4 e

	//glFlush();

	if (!out_view) {
		
		glPopMatrix(); // 3 e

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPopMatrix(); // 2 e

		glColor4f(1, 1, 1, 0.25);
		glutSolidSphere(0.8, 50, 15);

		glDisable(GL_BLEND);
		glPopMatrix(); // 1 e
	}

	/*if (!out_view) {
		glPopMatrix();
	}*/

	
	
	glPopMatrix(); // 0 e

	glutSwapBuffers();
}

void MyReshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
}

GLfloat snowpos_crit[][2] = {
	{70.0f, -240.0f},
	{90.0f, -225.0f},
	{120.0f, -210.0f},
	{140.0f, -195.0f},
	{160.0f, -180.0f},
	{180.0f, -165.0f},
	{200.0f, -140.0f},
};

void fallingSnow() {
	delta += 0.001;
	//delta += 0.03; // 100���

	for (int i = 0; i < max_snow; ++i) {
		if (abs(snowpos[i][0]) < snowpos_crit[0][0] && snowpos[i][1] > snowpos_crit[0][1] ||
			abs(snowpos[i][0]) < snowpos_crit[1][0] && snowpos[i][1] > snowpos_crit[1][1] ||
			abs(snowpos[i][0]) < snowpos_crit[2][0] && snowpos[i][1] > snowpos_crit[2][1] ||
			abs(snowpos[i][0]) < snowpos_crit[3][0] && snowpos[i][1] > snowpos_crit[3][1] ||
			abs(snowpos[i][0]) < snowpos_crit[4][0] && snowpos[i][1] > snowpos_crit[4][1] ||
			abs(snowpos[i][0]) < snowpos_crit[5][0] && snowpos[i][1] > snowpos_crit[5][1] ||
			abs(snowpos[i][0]) < snowpos_crit[6][0] && snowpos[i][1] > snowpos_crit[6][1]) {
			snowpos[i][1] -= delta;
		}
	}

	glutPostRedisplay();
}


GLboolean isFirst = true;
void det_snowpos() {
	if (isFirst) {
		for (int i = 0; i < max_snow; ++i) {
			snowpos[i][0] = (rand() % 2 ? 1 : -1) * (rand() % 200);
			if (abs(snowpos[i][0]) < snowpos_crit[0][0]) {
				snowpos[i][1] = (rand() % 2 ? 1 : -1) * (rand() % int(-snowpos_crit[0][1]));
			}
			else if (abs(snowpos[i][0]) < snowpos_crit[1][0]) {
				snowpos[i][1] = (rand() % 2 ? 1 : -1) * (rand() % int(-snowpos_crit[1][1]));
			}
			else if (abs(snowpos[i][0]) < snowpos_crit[2][0]) {
				snowpos[i][1] = (rand() % 2 ? 1 : -1) * (rand() % int(-snowpos_crit[2][1]));
			}
			else if (abs(snowpos[i][0]) < snowpos_crit[3][0]) {
				snowpos[i][1] = (rand() % 2 ? 1 : -1) * (rand() % int(-snowpos_crit[3][1]));
			}
			else if (abs(snowpos[i][0]) < snowpos_crit[4][0]) {
				snowpos[i][1] = (rand() % 2 ? 1 : -1) * (rand() % int(-snowpos_crit[4][1]));
			}
			else if (abs(snowpos[i][0]) < snowpos_crit[5][0]) {
				snowpos[i][1] = (rand() % 2 ? 1 : -1) * (rand() % int(-snowpos_crit[5][1]));
			}
			else /*if (abs(snowpos[i][0]) < 200.0f)*/ {
				snowpos[i][1] = (rand() % 2 ? 1 : -1) * (rand() % int(-snowpos_crit[6][1]));
			}
			snowpos[i][2] = (rand() % 2 ? 1 : -1) * (rand() % 15);
		}
		isFirst = false;
	}
	else {
		for (int i = 0; i < max_snow; ++i) {
			for (int j = 0; j < 7; ++j) {
				if(abs(snowpos[i][0]) < snowpos_crit[j][0]){
					if (snowpos[i][1] < -snowpos_crit[j][1] - 30) {
						snowpos[i][1] += (rand() % 50);
						break;
					}
				}
			}
		}
	}
}

void snow_globe_init() {
	sndPlaySound(TEXT("C:\\�����̹���-����_��庣��ũ���ְ�����"), SND_ASYNC | SND_NOSTOP);
	/*	������ ũ���� - �Ʒ��� �ּҿ��� �ٿ�ε� ����
		
		���� : ��庣��ũ ���ְ� ���� by �����̹���

		Download / Stream 
		- https://www.sellbuymusic.com/musicDetail/8311

	*/
}

int main(int argc, char** argv) {
	srand((unsigned int)time(0));
	det_snowpos();
	snow_globe_init();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("Final Project :: Snow Globe by Yoseob Kim, 2015312229");
	glClearColor(0.7, 0.7, 0.7, 0.0); // Background Color�� ȸ������ ����

	//glInit();
	//InitLight(); // �� ���� �ʱ�ȭ


	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);

	glEnable(GL_COLOR_MATERIAL);


	loadObject("snow.obj"); // �ܺ� �𵨸� ������(.obj file) ó���� ���� �ۼ��� �Լ�
	CreateList(1); // ������ �ҷ��� �����͸� Ȱ���� Display list ���� �� ������ �ܰ迡�� �׷������� ����

	loadObject("treeup.obj");
	CreateList(2);

	loadObject("treedown.obj");
	CreateList(3);
		

	// �Ʒ����� �ݹ� �Լ� ���
	glutDisplayFunc(MyDisplay);
	glutKeyboardFunc(MyKeyboard);
	glutMouseFunc(MyMouseClick); // Ŭ���Ͽ��� ��
	glutMotionFunc(MyMouseMove); // ���콺�� ������ ��, ViewX~Z ���� ����
	glutReshapeFunc(MyReshape);
	glutIdleFunc(fallingSnow); // �����̰� �������� ��� ����

	glutMainLoop();
	return 0;
}