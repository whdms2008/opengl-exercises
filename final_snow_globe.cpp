// SKKU Computer Graphics (COM3013) - Final Project : Snow Globe
// 2015312229 Yoseob Kim

#define _CRT_SECURE_NO_WARNINGS // fscanf, fgets 사용을 위해 경고 무시하기 (Visual Studio 환경이 아닌 경우 코드를 삭제할 것)

#include<windows.h>
#include<MMSystem.h> // 소리 재생을 위한 헤더

// C headers (C++ type)
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <GL/glut.h>

// C++ headers
#include <iostream>
#include <iomanip> // 콘솔 출력 (디버깅 용도)에서의 부동 소수점 정밀도를 위한 헤더 파일
#include <ctime>

// tinkercad로 그린 눈송이 -> .obj 데이터 파싱 후 정점 배열로 변환

GLuint vlist[60000][3]; // GLubyte의 최대 범위인 255개를 넘어가기 때문에 GLuint로 설정
GLint vlist_size, vertex_info_size;
GLfloat vertex_info[30000][3];
GLfloat color_info[30000][3];

int snowflakeID; // 정보가 많아 display list로 처리, 해당 리스트의 ID
int treeupID, treedownID;

int id_array[10];

void CreateList(int param) { // .obj 파일에서 파싱해 온 정점 배열을 이용해 디스플레이 리스트 생성
	id_array[param-1] = glGenLists(param);
	glNewList(id_array[param - 1], GL_COMPILE);

	for (int i = 0; i < vlist_size; ++i) {
		// 기본 아이디어: 무수히 많은 삼각형 면을 이용하여 하나의 완성된 3D object를 그린다.
		glBegin(GL_TRIANGLES); // 이를 이용하기 위한 GL_TRIANGLES

		if (param == 1) {
			glColor3f(1.0, 1.0, 1.0); // 색상은 하얀 색으로 초기화 (특별한 의미 없음)
		}
		else if (param == 2) {
			glColor3f(0.0, 0.5, 0.0);
		}
		else if (param == 3) {
			glColor3f(0.5, 0.0, 0.0);
		}
		
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

// flatshading과 wireframe을 토글링하기 위한 변수
int FlatShaded = 0;
int WireFramed = 0;

// 마우스 움직임에 따라 시점을 바꾸기 위한 변수
// 카메라 위치를 이동 (X, Z 좌표계에서 카메라 원형으로 회전)
GLfloat ViewX = 0, ViewZ = 1;
GLfloat ViewY = 0;
// 초기에 설정한 값은, 디버깅을 위해 콘솔에 출력하던 ViewX~Z를 참고하여 가장 보기 좋은 각도를 기준으로 설정
// : 이렇게 하면 실행 시 가장 보기 좋은 각도로 모든 물체들이 나타난다.
GLfloat prevViewX, prevViewY, prevViewZ;

// 윈도우 사이즈
GLint windowWidth = 800, windowHeight = 600;

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

	// 카메라의 위치는 원점에 두고, 물체를 x^2+y^2+z^2=1 (반지름이 1인 구)를 기준으로 회전시킴
	// ViewX~Z를 구하는 것은 MyMouseMove() 함수에서 수행
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

	// 나무 그리기 - 큰 나무
	glPushMatrix(); // 4
	glScalef(0.04, 0.04, 0.04);
	glTranslatef(8, 3, 0);
	if (ViewY > 0) {
		glCallList(id_array[1]);
	}
	else {
		glCallList(id_array[2]);
	}

	// 계층 구조 모델링
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


	
	// 눈 송이 max_snow개 호출
	for (int i = 0; i < max_snow; ++i) {
		glPushMatrix(); // 4
		glScalef(0.004, 0.004, 0.004);
		glTranslatef(snowpos[i][0], snowpos[i][1], snowpos[i][2]);
		glCallList(id_array[0]);
		glPopMatrix(); // 4 e
	}


	// 나무 그리기 - 작은 나무
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

	// 계층 구조 모델링
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
	//delta += 0.03; // 100배속

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
	sndPlaySound(TEXT("C:\\셀바이뮤직-바흐_골드베르크연주곡주제"), SND_ASYNC | SND_NOSTOP);
	/*	음악의 크레딧 - 아래의 주소에서 다운로드 받음
		
		바흐 : 골드베르크 연주곡 주제 by 셀바이뮤직

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
	glClearColor(0.7, 0.7, 0.7, 0.0); // Background Color를 회색으로 설정

	//glInit();
	//InitLight(); // 빛 정보 초기화


	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);

	glEnable(GL_COLOR_MATERIAL);


	loadObject("snow.obj"); // 외부 모델링 데이터(.obj file) 처리를 위해 작성한 함수
	CreateList(1); // 위에서 불러온 데이터를 활용해 Display list 생성 및 컴파일 단계에서 그려지도록 설정

	loadObject("treeup.obj");
	CreateList(2);

	loadObject("treedown.obj");
	CreateList(3);
		

	// 아래에는 콜백 함수 등록
	glutDisplayFunc(MyDisplay);
	glutKeyboardFunc(MyKeyboard);
	glutMouseFunc(MyMouseClick); // 클릭하였을 때
	glutMotionFunc(MyMouseMove); // 마우스를 움직일 때, ViewX~Z 값을 조정
	glutReshapeFunc(MyReshape);
	glutIdleFunc(fallingSnow); // 눈송이가 떨어지는 장면 묘사

	glutMainLoop();
	return 0;
}