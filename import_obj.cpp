#define _CRT_SECURE_NO_WARNINGS

#include <GL/glut.h>
#include <cstdio>
#include <cstring>

GLuint vlist[700][3];
GLint vlist_size, vertex_info_size;
GLfloat vertex_info[400][3];
GLfloat color_info[400][3];

void loadObject(const char* path) {
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

int listID;

void display(void){
	glClear(GL_COLOR_BUFFER_BIT);
	/*glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);*/

	printf("%d\n", vlist_size);

	glViewport(0, 0, 500, 500);
	glPushMatrix();
	glTranslatef(13, 0, 0);
	//glRotatef(60, 0, 0, 0);
	glCallList(listID);
	glPopMatrix();
	/*
	for (int i = 0; i < vertex_info_size; ++i) { // 색상 임의로 지정
		color_info[i][0] = 0.0;
		color_info[i][1] = 0.0;
		color_info[i][2] = 0.0;
	}
	
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glColorPointer(3, GL_FLOAT, 0, color_info);
	glVertexPointer(3, GL_FLOAT, 0, vertex_info);

	glLoadIdentity();

	
	glPushMatrix();

	glRotatef(-30, 0, 0, 0);
	glScalef(0.5, 0.5, 0.5);
	for (GLint i = 0; i < vlist_size; ++i) {
		glDrawElements(GL_POLYGON, 3, GL_UNSIGNED_INT, vlist[i]);
	}
	glPopMatrix();
	*/
	glFlush();
}

void CreateList() {
	listID = glGenLists(1);
	glNewList(listID, GL_COMPILE);
	for (int i = 0; i < vlist_size; ++i) {
		glBegin(GL_TRIANGLES);

		glColor3f(0.5, 0.5, 0.5);
		glVertex3f(vertex_info[vlist[i][0]][0], vertex_info[vlist[i][0]][1], vertex_info[vlist[i][0]][2]);
		glVertex3f(vertex_info[vlist[i][1]][0], vertex_info[vlist[i][1]][1], vertex_info[vlist[i][1]][2]);
		glVertex3f(vertex_info[vlist[i][2]][0], vertex_info[vlist[i][2]][1], vertex_info[vlist[i][2]][2]);
		printf("%d번째 면: p1(%f, %f, %f)-p2(%f, %f, %f)-p3(%f, %f, %f)\n", i,
			vertex_info[vlist[i][0]][0], vertex_info[vlist[i][0]][1], vertex_info[vlist[i][0]][2],
			vertex_info[vlist[i][1]][0], vertex_info[vlist[i][1]][1], vertex_info[vlist[i][1]][2],
			vertex_info[vlist[i][2]][0], vertex_info[vlist[i][2]][1], vertex_info[vlist[i][2]][2]);

		glEnd();
	}
	glEndList();
}

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("Object Load Test");
	
	glClearColor(0, 0, 0, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-20, 20, -20, 20, -20, 20);

	loadObject("tinker.obj");
	CreateList();
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}