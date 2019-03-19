#include<GL/glut.h>

void polygon() {
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POLYGON);
		glColor3f(1, 0, 0);
		glVertex2d(0.0, 0.5); // titik paling atas
		glColor3f(1, 1, 0);
		glVertex2d(0.35, 0.15); // atas kanan
		glColor3f(0, 1, 0);
		glVertex2d(0.25, -0.5); // bawah kanan
		glColor3f(0, 0, 1);
		glVertex2d(-0.25, -0.5); // bawah kiri
		glColor3f(1, 0, 1);
		glVertex2d(-0.35, 0.15); // atas kiri
	glEnd();
	glFlush();
}

int main(int argc, char**argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Polygon Onichan");
	glutDisplayFunc(polygon);
	glutMainLoop();

	return 0;
}