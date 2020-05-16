// ��伷, 2015312229, ��ǻ�ͱ�����
#include <cstdlib>
#include <GL/glut.h>
// stdlib.h <-> glut.h �� exit(0) �Լ� ������ ���� �߻�
// �ذ������ stdlib.h �� ���� include ��
#include <string> // ������ Ÿ��Ʋ�� ó�����ֱ� ���� c++ string ��� �̿�
#include <utility>
#include <iostream>

// �⺻ â ������ ���õ� �׸��
GLint windowWidth, windowHeight; // �������� ������
const std::string myInfo = "Midterm 2015312229 Yoseob Kim - [";
std::string windowTitle;
GLboolean programInit = true;

// �޴��� �����ϴ� �׸��
GLint drawType, renderingType; // �׸� ������ ������ ������ Ÿ���� ����
GLfloat redRatio, greenRatio, blueRatio;
GLfloat redRatio_B, greenRatio_B, blueRatio_B;
GLboolean animationStart;

// �׸��� ���� �� �ִϸ��̼ǿ� ���õ� �׸��
std::pair<GLint, GLint> topLeft, bottomRight;
const std::string drawTypes[] = { "Cube", "Sphere", "Torus", "Cone", "Tetraheron", "Icosaheron", "Teapot" };
const std::string renderingTypes[] = { "Solid", "Wireframe" };
const std::string colorTypes[] = { "Red", "Green", "Blue" };
const std::string idleTypes[] = { "Move", "Stop" };
GLint colorType, idleType;
GLfloat delta = 0.0;

// �׸��� ������ ũ�� ����
GLfloat size, prev_size;

GLint prev_drawType;

void myDisplay() {
	//glViewport(0, 0, windowWidth, windowHeight);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(redRatio, greenRatio, blueRatio);

	if (!animationStart) {
		std::cout << topLeft.first << "," << topLeft.second << " "
			<< bottomRight.first << "," << bottomRight.second << "  ::  ";
	}

	std::cout.precision(1);
	GLfloat l, r, b, t;
	l = topLeft.first / GLfloat(windowWidth) - 1.0;
	r = bottomRight.first / GLfloat(windowWidth) + 1.0;
	b = (windowHeight - bottomRight.second) / GLfloat(windowHeight) - 1.0;
	t = (windowHeight - topLeft.second) / GLfloat(windowHeight) + 1.0;

	if (!animationStart) {
		std::cout << l << "," << r << "  #  " << b << "," << t << "  ::  ";
	}
	//glOrtho(l, r, b, t, 1.0, 10.0);

	size = (bottomRight.first - topLeft.first) * (bottomRight.second - topLeft.second) / GLfloat(windowWidth * windowHeight);

	if (!animationStart) {
		std::cout << "size = " << size << std::endl;
	}

	/*if (prev_drawType == 4 || prev_drawType == 5) {
		glScalef(1 / (1.2 * prev_size + 0.5), 1 / (1.2 * prev_size + 0.5), 1 / (1.2 * prev_size + 0.5));
	}*/

	glPushMatrix(); // ������ ���� ����

	if (drawType == 4 || drawType == 5) {
		glScalef(1.2 * size + 0.5, 1.2 * size + 0.5, 1.2 * size + 0.5);
	}
	else {
		glScalef(1.0, 1.0, 1.0);
	}

	switch (drawType) {
	case 0: // Cube
		if (!renderingType) { // Solid
			glutSolidCube(size * 4.5);
		}
		else { // Wire
			glutWireCube(size * 4.5);
		}
		break;
	case 1: // Sphere
		if (!renderingType) { // Solid
			glutSolidSphere(size * 3, 10, 10);
		}
		else { // Wire
			glutWireSphere(size * 3, 10, 10);
		}
		break;
	case 2: // Torus
		if (!renderingType) { // Solid
			glutSolidTorus(size, size * 2, 10, 10);
		}
		else { // Wire
			glutWireTorus(size, size * 2, 10, 10);
		}
		break;
	case 3: // Cone
		if (!renderingType) { // Solid
			glutSolidCone(size * 2.5, size, 5, 10);
		}
		else { // Wire
			glutWireCone(size * 2.5, size, 5, 10);
		}
		break;
	case 4: // Tetraheron
		if (size > 0) {
			if (!renderingType) { // Solid
				glutSolidTetrahedron();
			}
			else { // Wire
				glutWireTetrahedron();
			}
		}
		break;
	case 5: // Icosaheron
		if (size > 0) {
			if (!renderingType) { // Solid
				glutSolidIcosahedron();
			}
			else { // Wire
				glutWireIcosahedron();
			}
		}
		break;
	case 6: // Teapot
		if (size > 0) {
			if (!renderingType) { // Solid
				glutSolidTeapot(size * 2.5);
			}
			else { // Wire
				glutWireTeapot(size * 2.5);
			}
		}
		break;
	}

	/*if (!animationStart) {
		glFlush();
	}
	else {
		glutSwapBuffers();
	}*/
	glutSwapBuffers();

	// windowTitle
	windowTitle = myInfo + drawTypes[drawType] + ":" + renderingTypes[renderingType] + ":" + colorTypes[colorType] + ":" + idleTypes[idleType] + "]";
	glutSetWindowTitle(windowTitle.c_str());

	glPopMatrix(); // ������ ���� ����
}

void subShape(int entryID) {
	prev_drawType = drawType;
	drawType = entryID;
	// �������� drawType �� ���� �����ϰ� display callback �Լ����� ó��
	glutPostRedisplay();
}

void subRendering(int entryID) {
	renderingType = entryID;
	// �������� renderingType �� ���� �����ϰ� display callback �Լ����� ó��
	glutPostRedisplay();
}

void subColor(int entryID) {
	switch (entryID) {
	case 0:
		redRatio = 1.0;
		greenRatio = blueRatio = 0.0;
		break;
	case 1:
		greenRatio = 1.0;
		redRatio = blueRatio = 0.0;
		break;
	case 2:
		blueRatio = 10.0;
		redRatio = greenRatio = 0.0;
		break;
	}
	colorType = entryID;
	glutPostRedisplay();
}

void subAnimation(int entryID) {
	if (entryID == 0) {
		animationStart = true;
		//glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	}
	else {
		animationStart = false;
		//glutInitDisplayMode(GLUT_RGB);
	}
	idleType = entryID;
	glutPostRedisplay();
}

void mainMenu(int entryID) {
	// what to do ?
	std::cout << "I'm here ! " << std::endl;
}

GLboolean redundant_check = false;
void myAnimation() {
	if (animationStart) {

		delta += 0.5;

		if (delta > 5e+02) {
			delta = -5e+02;
		}
		glViewport(delta, 0, windowWidth + delta, windowHeight);
		//std::cout << "delta : " << delta << std::endl;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

		redundant_check = true;
		glutPostRedisplay();
	}
	else {
		if (redundant_check) {
			glViewport(0, 0, windowWidth, windowHeight);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

			redundant_check = false;
			glutPostRedisplay();
		}
	}
}

void myInitialize() {
	windowWidth = 800;
	windowHeight = 600;
	windowTitle = myInfo + "Cube:Solid:Red:Stop]";
	drawType = 0; // Shape - Cube (default)
	renderingType = 0; // Rendering - Solid (default)
	redRatio = 1.0; // Color - Red (default)
	greenRatio = blueRatio = 0.0;
	animationStart = false; // Animation  - Stop (default)
	colorType = 0;
	idleType = 1;
}

void mouseClick(GLint Button, GLint State, GLint X, GLint Y) {
	if (Button == GLUT_LEFT_BUTTON) {
		if (State == GLUT_DOWN) {
			topLeft.first = X;
			topLeft.second = Y;

			// ���� �� ���
			redRatio_B = redRatio;
			greenRatio_B = greenRatio;
			blueRatio_B = blueRatio;
		}
		else if (State == GLUT_UP) {
			// ��� �� ���� �� �ǵ�����
			redRatio = redRatio_B;
			greenRatio = greenRatio_B;
			blueRatio = blueRatio_B;
			glutPostRedisplay();
		}
		else {}
	}
}

void mouseMove(GLint X, GLint Y) {

	prev_size = size; // ������ ������ ������ size �� ����

	bottomRight.first = X;
	bottomRight.second = Y;

	// �巡���ϴ� ���ȿ��� ȸ������ �׸���
	redRatio = greenRatio = blueRatio = 0.5;
	glutPostRedisplay();
}

void myReshape(int newWidth, int newHeight) {
	glViewport(0, 0, newWidth, newHeight);
	GLfloat widthFactor = (GLfloat)newWidth / (GLfloat)windowWidth;
	GLfloat heightFactor = (GLfloat)newHeight / (GLfloat)windowHeight;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0 * widthFactor, 1.0 * widthFactor, -1.0 * heightFactor, 1.0 * heightFactor, -1.0, 1.0);
}

int main(int argc, char** argv) {
	// (default) �� ���� �� �⺻ â ���� ���� �׸� �ʱ�ȭ
	myInitialize();
	glutInit(&argc, argv);

	// double buffereing �� �̿��ϱ� ���� GLUT_DOUBLE
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	//glutInitDisplayMode(GLUT_RGB);

	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(windowTitle.c_str());

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	//glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	// Shape ���� �޴� ����
	GLint subShapeID = glutCreateMenu(subShape);
	for (int i = 0; i < 7; ++i) {
		glutAddMenuEntry(drawTypes[i].c_str(), i);
	}

	// Rendering ���� �޴� ����
	GLint subRenderingID = glutCreateMenu(subRendering);
	for (int i = 0; i < 2; ++i) {
		glutAddMenuEntry(renderingTypes[i].c_str(), i);
	}

	// Color ���� �޴� ����
	GLint subColorID = glutCreateMenu(subColor);
	for (int i = 0; i < 3; ++i) {
		glutAddMenuEntry(colorTypes[i].c_str(), i);
	}

	// Animation ���� �޴� ����
	GLint subAnimationID = glutCreateMenu(subAnimation);
	for (int i = 0; i < 2; ++i) {
		glutAddMenuEntry(idleTypes[i].c_str(), i);
	}

	// ���� �޴� ���� �� ���� �޴� ����
	GLint mainMenuID = glutCreateMenu(mainMenu);
	glutAddSubMenu("Shape", subShapeID);
	glutAddSubMenu("Rendering", subRenderingID);
	glutAddSubMenu("Color", subColorID);
	glutAddSubMenu("Animation", subAnimationID);
	glutAttachMenu(GLUT_RIGHT_BUTTON); // ���� ��ư�� �޴� ����

	// �ݹ� �Լ� ���
	glutDisplayFunc(myDisplay);
	glutIdleFunc(myAnimation);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove);
	glutReshapeFunc(myReshape);

	glPushMatrix(); // ���� ������ ���¸� ���� (�ʱ� ����)

	glutMainLoop();
	return 0;
}