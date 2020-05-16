// 2015312229 김요섭
#include <cstdlib>
#include <cmath>
#include <GL/glut.h>
#include <iostream>
#include <iomanip>

// flatshading과 wireframe을 토글링하기 위한 부울 변수
int FlatShaded = 0;
int WireFramed = 0;
// 마우스 움직임에 따라 시점을 바꾸기 위한 변수
// 카메라 위치를 이동 (X, Z 좌표계에서 카메라 원형으로 회전)
GLfloat ViewX = 0, ViewZ = 1;
GLfloat ViewY = 0;
GLfloat prevViewX, prevViewY, prevViewZ;
GLint windowWidth = 400, windowHeight = 400;

void InitLight() {
	GLfloat mat_diffuse[] = { 0.5, 0.4, 0.3, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_ambient[] = { 0.5, 0.4, 0.3, 1.0 };
	GLfloat mat_shininess[] = { 15.0 };

	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat light_position[] = { -3, 6, 3.0, 0.0 };

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

GLboolean isClicked = false;

void MyMouseClick(GLint Button, GLint State, GLint X, GLint Y) {
	if ((Button == GLUT_LEFT_BUTTON || Button == GLUT_RIGHT_BUTTON) && State == GLUT_DOWN) {
		isClicked = true;
	}
	else {
		isClicked = false;
	}
}

void MyMouseMove(GLint X, GLint Y) {
	if (isClicked) {
		// 시점 변화를 위한 작업

		if (X > 0 && X < windowWidth && Y > 0 && Y < windowHeight) {
			ViewX = (X - (windowWidth / 2.0)) / GLfloat(windowWidth);
			ViewY = (Y - (windowHeight / 2.0)) / GLfloat(windowHeight);
			if (1 - (ViewX * ViewX + ViewY * ViewY) > 0) {
				ViewZ = sqrt(1 - (ViewX * ViewX + ViewY * ViewY));
			}
			else {
				ViewZ = 0;
			}

			prevViewX = ViewX;
			prevViewY = ViewY;
			prevViewZ = ViewZ;
		}
		else { // 윈도우 범위 벗어나면, 벗어나기 직전의 상태를 출력
			ViewX = prevViewX;
			ViewY = prevViewY;
			ViewZ = prevViewZ;
		}

		glutPostRedisplay();
	}
}

void MyKeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':
	case 'Q':
	case '\033': // esc key
		exit(0);
		break;
	case 's':
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
		// 여기에 와이어 프레임 토글링을 위한 코드를 삽입할 것
		// 위의 전역 변수 WireFramed를 사용할 것
	case 'w':
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

// 테이블의 윗판 (앞 4개의 정점은 윗판의 윗면, 뒤 4개의 정점은 윗판의 아랫면)
/*GLfloat table_color[8][3] = {	{0xe9 / 255.0, 0xec / 255.0, 0xef / 255.0}, {0xde / 255.0, 0xe2 / 255.0, 0xe6 / 255.0}, {0xce / 255.0, 0xd4 / 255.0, 0xda / 255.0}, {0xad / 255.0, 0xb5 / 255.0, 0xbd / 255.0},
								{0x86 / 255.0, 0x8e / 255.0, 0x96 / 255.0}, {0x49 / 255.0, 0x50 / 255.0, 0x57 / 255.0}, {0x34 / 255.0, 0x3a / 255.0, 0x40 / 255.0}, {0x21 / 255.0, 0x25 / 255.0, 0x29 / 255.0} };*/
GLubyte table_vlist[][4] = { {0, 4, 5, 1}, {1, 5, 6, 2}, {4, 7, 6, 5}, {0, 1, 2, 3}, {7, 3, 2, 6}, {4, 0, 3, 7} }; // 교재와 조금 다름 (내 그림은 윗면 - 아랫면 순)

void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	//glColorPointer(3, GL_FLOAT, 0, table_color);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 0, ViewX, ViewY, ViewZ, 0, 1, 0);
	std::cout << std::fixed;
	std::cout << std::setprecision(4) << "ViewX(x-z plane): " << ViewX << "\tViewZ(x-z plane): " << ViewZ << "\tViewY(height): " << ViewY << std::endl;

	// 마우스의 움직임에 따라 시점을 변화시키려면
	// MyMouseMove 함수에서 입력되는 변화량을 참고하여
	// gluLookAt 명령어 파라미터를 변경해 볼 것(교재 참조)
	// 아래에 필요함 물체 드로잉 명령을 삽입할 것


	glFrontFace(GL_CW); // glut Object 그릴 때는 CW 로 해야 제대로 돌아감

	// glutSolidTeapot만을 이용해서 Wire, Solid 모두 표현할 수 있도록 하기 - 교재 P423, 424 참조함
	if (WireFramed) {
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_LINE);
		glCullFace(GL_FRONT);
	}
	else {
		glCullFace(GL_BACK);
	}

	glutSolidTeapot(0.15); // 예시

	glFrontFace(GL_CCW); // 내 정점 배열 그림은 CCW 로 설정
	glVertexPointer(3, GL_FLOAT, 0, top_of_table);
	for (GLint i = 0; i < 6; ++i) {
		glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_BYTE, table_vlist[i]);
	}
	glVertexPointer(3, GL_FLOAT, 0, table_leg1);
	for (GLint i = 0; i < 6; ++i) {
		glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_BYTE, table_vlist[i]);
	}
	glVertexPointer(3, GL_FLOAT, 0, table_leg2);
	for (GLint i = 0; i < 6; ++i) {
		glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_BYTE, table_vlist[i]);
	}
	glVertexPointer(3, GL_FLOAT, 0, table_leg3);
	for (GLint i = 0; i < 6; ++i) {
		glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_BYTE, table_vlist[i]);
	}
	glVertexPointer(3, GL_FLOAT, 0, table_leg4);
	for (GLint i = 0; i < 6; ++i) {
		glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_BYTE, table_vlist[i]);
	}

	glFlush();
}
void MyReshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("OpenGL PROJECT I 2015312229 김요섭");
	glClearColor(0.4, 0.4, 0.4, 0.0);

	InitLight();
	glutDisplayFunc(MyDisplay);
	glutKeyboardFunc(MyKeyboard);
	glutMouseFunc(MyMouseClick);
	glutMotionFunc(MyMouseMove);
	glutReshapeFunc(MyReshape);
	glutMainLoop();
}