// SKKU Computer Graphics (COM3013) - Final Project : Snow Globe
// 2015312229 Yoseob Kim

#define _CRT_SECURE_NO_WARNINGS // fscanf, fgets ����� ���� ��� �����ϱ� (Visual Studio ȯ���� �ƴ� ��� �ڵ带 ������ ��)

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

void MyKeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q': case 'Q': case '\033': // esc key
		exit(0);
		break;

	case 's': case 'S': // s Ȥ�� S Ű�� ������ Flat <-> Shaded ���� Toggle
		if (FlatShaded) {
			FlatShaded = 0;
			glShadeModel(GL_SMOOTH);
		}
		else {
			FlatShaded = 1;
			glShadeModel(GL_FLAT);
		}
		glutPostRedisplay();
		break;

	case 'w': case 'W': // w Ȥ�� W Ű�� ������ Solid <-> WireFrame ���� Toggle
		if (WireFramed) {
			WireFramed = 0;
		}
		else {
			WireFramed = 1;
		}
		glutPostRedisplay();
		break;
	}
}


// ** Ź�ڿ� ���õ� ������

GLfloat tableSize = 0.6, legSize = 0.1;
GLfloat tableUpperPos = -0.1;
GLfloat tableLowerPos = -0.15;
GLfloat tableBottomPos = -0.5;
GLfloat top_of_table[8][3] = { {-tableSize, tableUpperPos, tableSize}, { tableSize, tableUpperPos, tableSize }, { tableSize, tableUpperPos, -tableSize }, { -tableSize, tableUpperPos, -tableSize },
								{-tableSize, tableLowerPos, tableSize}, { tableSize, tableLowerPos, tableSize }, { tableSize, tableLowerPos, -tableSize }, { -tableSize, tableLowerPos, -tableSize } };
GLfloat table_leg1[8][3] = { {-tableSize, tableLowerPos, tableSize}, {-tableSize + legSize, tableLowerPos, tableSize}, {-tableSize + legSize, tableLowerPos, tableSize - legSize}, {-tableSize, tableLowerPos, tableSize - legSize},
								{-tableSize, tableBottomPos, tableSize}, {-tableSize + legSize, tableBottomPos, tableSize}, {-tableSize + legSize, tableBottomPos, tableSize - legSize}, {-tableSize, tableBottomPos, tableSize - legSize} };
GLfloat table_leg2[8][3] = { {tableSize - legSize, tableLowerPos, tableSize}, {tableSize, tableLowerPos, tableSize}, {tableSize, tableLowerPos, tableSize - legSize}, {tableSize - legSize, tableLowerPos, tableSize - legSize},
								{tableSize - legSize, tableBottomPos, tableSize}, {tableSize, tableBottomPos, tableSize}, {tableSize, tableBottomPos, tableSize - legSize}, {tableSize - legSize, tableBottomPos, tableSize - legSize} };
GLfloat table_leg3[8][3] = { {tableSize - legSize, tableLowerPos, -tableSize + legSize}, {tableSize, tableLowerPos, -tableSize + legSize}, {tableSize, tableLowerPos, -tableSize}, {tableSize - legSize, tableLowerPos, -tableSize},
								{tableSize - legSize, tableBottomPos, -tableSize + legSize}, {tableSize, tableBottomPos, -tableSize + legSize}, {tableSize, tableBottomPos, -tableSize}, {tableSize - legSize, tableBottomPos, -tableSize} };
GLfloat table_leg4[8][3] = { {-tableSize, tableLowerPos, -tableSize + legSize}, {-tableSize + legSize, tableLowerPos, -tableSize + legSize}, {-tableSize + legSize, tableLowerPos, -tableSize}, {-tableSize, tableLowerPos, -tableSize},
								{-tableSize, tableBottomPos, -tableSize + legSize}, {-tableSize + legSize, tableBottomPos, -tableSize + legSize}, {-tableSize + legSize, tableBottomPos, -tableSize}, {-tableSize, tableBottomPos, -tableSize} };

// ���̺��� ���� (�� 4���� ������ ������ ����, �� 4���� ������ ������ �Ʒ���)
/*GLfloat table_color[8][3] = {	{0xe9 / 255.0, 0xec / 255.0, 0xef / 255.0}, {0xde / 255.0, 0xe2 / 255.0, 0xe6 / 255.0}, {0xce / 255.0, 0xd4 / 255.0, 0xda / 255.0}, {0xad / 255.0, 0xb5 / 255.0, 0xbd / 255.0},
								{0x86 / 255.0, 0x8e / 255.0, 0x96 / 255.0}, {0x49 / 255.0, 0x50 / 255.0, 0x57 / 255.0}, {0x34 / 255.0, 0x3a / 255.0, 0x40 / 255.0}, {0x21 / 255.0, 0x25 / 255.0, 0x29 / 255.0} };*/
GLubyte table_vlist[][4] = { {0, 4, 5, 1}, {1, 5, 6, 2}, {4, 7, 6, 5}, {0, 1, 2, 3}, {7, 3, 2, 6}, {4, 0, 3, 7} }; // ����� ���� �ٸ� (�� �׸��� ���� - �Ʒ��� ��)


GLint snowpos[30][3];
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


	// �� ���� 30�� ȣ��
	for (int i = 0; i < 30; ++i) {
		glPushMatrix();
		glScalef(0.004, 0.004, 0.004);
		glTranslatef(snowpos[i][0], snowpos[i][1], snowpos[i][2]);
		glCallList(id_array[0]);
		glPopMatrix();
	}

	// ���� �׸���
	glPushMatrix();
	glScalef(0.04, 0.04, 0.04);
	glTranslatef(8, 3, 0);
	glCallList(id_array[1]);

	// ���� ���� �𵨸�
	glPushMatrix();
	//glTranslatef()
	glCallList(id_array[2]);
	glPopMatrix();

	glPopMatrix();


	// ���� �׸���
	glPushMatrix();
	glScalef(0.02, 0.02, 0.02);
	glTranslatef(-15, -15, 0);
	glCallList(id_array[1]);

	// ���� ���� �𵨸�
	glPushMatrix();
	//glTranslatef()
	glCallList(id_array[2]);
	glPopMatrix();

	glPopMatrix();


	glFlush();
}

void MyReshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
}

int main(int argc, char** argv) {
	srand((unsigned int)time(0));
	for (int i = 0; i < 30; ++i) {
		snowpos[i][0] = (rand() % 2? 1: -1) * (rand() % 300);
		snowpos[i][1] = (rand() % 2 ? 1 : -1) * (rand() % 250);
		snowpos[i][2] = (rand() % 2 ? 1 : -1) * (rand() % 100);
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("Final Project 2015312229 ��伷");
	glClearColor(0.6, 0.6, 0.6, 0.0); // Background Color�� ȸ������ ����

	//glInit();
	//InitLight(); // �� ���� �ʱ�ȭ

	loadObject("tinker.obj"); // �ܺ� �𵨸� ������(.obj file) ó���� ���� �ۼ��� �Լ�
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

	glutMainLoop();
	return 0;
}