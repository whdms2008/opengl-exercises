// SKKU Computer Graphics (COM3013) - Project Modeling 1
// 2015312229 Yoseob Kim

// ** 이번 프로젝트를 통해 익힌 기술들:
// Vertex lists, Display list, 모델 변환, 시점 변환, WireFrame, 외부 모델링 데이터(.obj file) import

#define _CRT_SECURE_NO_WARNINGS // fscanf, fgets 사용을 위해 경고 무시하기 (Visual Studio 환경이 아닌 경우 코드를 삭제할 것)

// C headers (C++ type)
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <GL/glut.h>

// C++ headers
#include <iostream>
#include <iomanip> // 콘솔 출력 (디버깅 용도)에서의 부동 소수점 정밀도를 위한 헤더 파일

// tinkercad로 그린 눈송이 -> .obj 데이터 파싱 후 정점 배열로 변환
GLuint vlist[700][3]; // GLubyte의 최대 범위인 255개를 넘어가기 때문에 GLuint로 설정
GLint vlist_size, vertex_info_size;
GLfloat vertex_info[400][3];
GLfloat color_info[400][3];

int snowflakeID; // 정보가 많아 display list로 처리, 해당 리스트의 ID

void CreateList() { // .obj 파일에서 파싱해 온 정점 배열을 이용해 디스플레이 리스트 생성
	snowflakeID = glGenLists(1);
	glNewList(snowflakeID, GL_COMPILE);
	for (int i = 0; i < vlist_size; ++i) {
		// 기본 아이디어: 무수히 많은 삼각형 면을 이용하여 하나의 완성된 3D object를 그린다.
		glBegin(GL_TRIANGLES); // 이를 이용하기 위한 GL_TRIANGLES

		glColor3f(0, 0, 0); // 색상은 하얀 색으로 초기화 (특별한 의미 없음)

		// 정점 배열의 index를 활용하여 하나의 삼각형의 세 점을 구성 (k=0~2 라 할 때, vlist[i][k] 가 하나의 vertex 정보를 표현)
		// u=현재의 점이라 할 때, vertex_info[u][0~2] 는 한 점의 x, y, z 값으로 구성
		glVertex3f(vertex_info[vlist[i][0]][0], vertex_info[vlist[i][0]][1], vertex_info[vlist[i][0]][2]);
		glVertex3f(vertex_info[vlist[i][1]][0], vertex_info[vlist[i][1]][1], vertex_info[vlist[i][1]][2]);
		glVertex3f(vertex_info[vlist[i][2]][0], vertex_info[vlist[i][2]][1], vertex_info[vlist[i][2]][2]);
		/*
		// 제대로 리스트 형성이 되는지 보기 위해 출력한 값
		printf("%d번째 면: p1(%f, %f, %f)-p2(%f, %f, %f)-p3(%f, %f, %f)\n", i,
			vertex_info[vlist[i][0]][0], vertex_info[vlist[i][0]][1], vertex_info[vlist[i][0]][2],
			vertex_info[vlist[i][1]][0], vertex_info[vlist[i][1]][1], vertex_info[vlist[i][1]][2],
			vertex_info[vlist[i][2]][0], vertex_info[vlist[i][2]][1], vertex_info[vlist[i][2]][2]); */

		glEnd();
	}
	glEndList();
}


void loadObject(const char* path) {
	// 외부의 모델링을 불러오기 위한 함수
	// https://github.com/opengl-tutorials/ogl/blob/master/common/objloader.cpp 를 참조하여, 
	// 나의 그림에 적합한 방식으로 함수를 전면 수정 (원본 파일의 bool loadOBJ() 함수 활용)
	printf("Loading OBJ file %s...\n", path);

	FILE* file = fopen(path, "r");

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

// flatshading과 wireframe을 토글링하기 위한 변수
int FlatShaded = 0;
int WireFramed = 0;

// 마우스 움직임에 따라 시점을 바꾸기 위한 변수
// 카메라 위치를 이동 (X, Z 좌표계에서 카메라 원형으로 회전)
GLfloat ViewX = -0.2700, ViewZ = 0.9320;
GLfloat ViewY = -0.2420;
// 초기에 설정한 값은, 디버깅을 위해 콘솔에 출력하던 ViewX~Z를 참고하여 가장 보기 좋은 각도를 기준으로 설정
// : 이렇게 하면 실행 시 가장 보기 좋은 각도로 모든 물체들이 나타난다.
GLfloat prevViewX, prevViewY, prevViewZ;

// 윈도우 사이즈
GLint windowWidth = 500, windowHeight = 500;

void InitLight() { // 광원에 대한 함수 (from base code)
	GLfloat mat_diffuse[] = { 0.5, 0.35, 0.3, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_ambient[] = { 0.5, 0.35, 0.3, 1.0 };
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

GLboolean isClicked = false; // 마우스 클릭 여부를 판단

void MyMouseClick(GLint Button, GLint State, GLint X, GLint Y) { // 마우스 클릭이 되었을 때만 시점 변환을 수행 (gluLookAt)
	if ((Button == GLUT_LEFT_BUTTON || Button == GLUT_RIGHT_BUTTON) && State == GLUT_DOWN) { // 좌, 우 버튼 모두 가능, 마우스가 눌렸을 때 Activate
		isClicked = true;
	}
	else { // 마우스 버튼을 뗐을 때 Deactivate
		isClicked = false;
	}
}

void MyMouseMove(GLint X, GLint Y) {
	if (isClicked) {
		// 시점 변화를 위한 작업

		if (X > 0 && X < windowWidth && Y > 0 && Y < windowHeight) {
			ViewX = (X - (windowWidth / 2.0)) / GLfloat(windowWidth); // ViewX는 마우스 움직임의 가로 방향
			ViewY = (Y - (windowHeight / 2.0)) / GLfloat(windowHeight); // ViewY는 마우스 움직임의 세로 방향
			if (1 - (ViewX * ViewX + ViewY * ViewY) > 0) { // 반지름이 1인 구를 기준으로 물체를 회전시킨다고 생각하고, ViewZ를 구하기
				ViewZ = sqrt(1 - (ViewX * ViewX + ViewY * ViewY));
			}
			else {
				ViewZ = 0;
			}

			// 현재 ViewX~Z 값을 백업
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
	case 'q': case 'Q': case '\033': // esc key
		exit(0);
		break;

	case 's': case 'S': // s 혹은 S 키를 누르면 Flat <-> Shaded 상태 Toggle
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

	case 'w': case 'W': // w 혹은 W 키를 누르면 Solid <-> WireFrame 상태 Toggle
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


// ** 탁자에 관련된 정보들

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
	glViewport(0, 0, windowWidth, windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// 카메라의 위치는 원점에 두고, 물체를 x^2+y^2+z^2=1 (반지름이 1인 구)를 기준으로 회전시킴
	// ViewX~Z를 구하는 것은 MyMouseMove() 함수에서 수행
	gluLookAt(0, 0, 0, ViewX, ViewY, ViewZ, 0, 1, 0);


	// 마우스 좌표 변환한 정보를 콘솔 창에 출력 (디버깅을 위한 용도)
	std::cout << std::fixed;
	std::cout << std::setprecision(4) << "ViewX(x-z plane): " << ViewX << "\tViewZ(x-z plane): " << ViewZ << "\tViewY(height): " << ViewY << std::endl;


	// glutSolidTeapot만을 이용해서 Wire, Solid 모두 표현할 수 있도록 하기 - 교재 P423, 424 참조함
	if (WireFramed) {
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_LINE);
		glCullFace(GL_FRONT);
	}
	else {
		glCullFace(GL_BACK);
	}
	
	// ** 그리기 동작 1: GLUT 기본 제공 물체 - 도넛, 주전자

	glFrontFace(GL_CW); 

	// 도넛
	glPushMatrix();
		glTranslatef(-0.3, -0.04, +0.25);
		glRotatef(90, 1, 0, 0);

		glutSolidTorus(0.06, 0.15, 10, 10);
	glPopMatrix();

	// 주전자
	glPushMatrix();
		glTranslatef(+0.3, 0, +0.25);

		glutSolidTeapot(0.15);
	glPopMatrix();


	// ** 그리기 동작 2: 정점 배열을 이용한 나만의 탁자

	// 탁자
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
	

	// ** 그리기 동작 3: Autodesk Tinkercad를 이용해 그린 물체(.obj)를 import 하여 그린 눈송이 (탁자 위 물체로 이용 및 뒷 벽면 장식)

	glPushMatrix();
		glScalef(0.025, 0.025, 0.025);
		glTranslatef(15, -1.5, -10);
		glRotatef(90, 1, 0, 0);

		glCallList(snowflakeID);
	glPopMatrix();

	// 뒷 벽면 장식 1
	glPushMatrix();
		glScalef(0.01, 0.01, 0.01);
		glTranslatef(3, 20, 30);
		glCallList(snowflakeID);
	glPopMatrix();

	// 뒷 벽면 장식 2
	glPushMatrix();
		glScalef(0.005, 0.005, 0.005);
		glTranslatef(25, 30, 60);
		glCallList(snowflakeID);
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
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("OpenGL PROJECT I 2015312229 김요섭");
	glClearColor(0.4, 0.4, 0.4, 0.0); // Background Color를 회색으로 설정

	InitLight(); // 빛 정보 초기화

	loadObject("tinker.obj"); // 외부 모델링 데이터(.obj file) 처리를 위해 작성한 함수
	CreateList(); // 위에서 불러온 데이터를 활용해 Display list 생성 및 컴파일 단계에서 그려지도록 설정


	// 아래에는 콜백 함수 등록
	glutDisplayFunc(MyDisplay);
	glutKeyboardFunc(MyKeyboard);
	glutMouseFunc(MyMouseClick); // 클릭하였을 때
	glutMotionFunc(MyMouseMove); // 마우스를 움직일 때, ViewX~Z 값을 조정
	glutReshapeFunc(MyReshape);

	glutMainLoop();
	return 0;
}