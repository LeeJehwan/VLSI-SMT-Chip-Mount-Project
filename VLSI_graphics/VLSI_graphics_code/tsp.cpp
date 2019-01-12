#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <cstdio>
#include <gl/glut.h>
#include <sstream>
using namespace std;

void display();
void init();
void readText();
void reshape(int width, int height);
void readPATH();

GLfloat myVertices[13000][2];
GLint cnt = 0;
GLfloat current_width = 1000.0;
GLfloat current_height = 500.0;

GLfloat max_x = 0;
GLfloat max_y = 0;
GLint path[13000];
GLint speed = 10;

GLboolean PATH_BOOLEAN = FALSE;
GLboolean FILE_BOOLEAN = FALSE;
GLboolean DRAW_PATH = FALSE;
GLboolean SHOW = FALSE;
GLboolean CONT = FALSE;
GLboolean UP = FALSE;

void drawString(const char *str, int x, int y, float color[4], void *font) {
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); 
	glDisable(GL_LIGHTING);    
	glDisable(GL_TEXTURE_2D);
	glDepthFunc(GL_ALWAYS);

	glColor4fv(color);         
	glRasterPos2i(x, y);        

								
	while (*str) {
		glutBitmapCharacter(font, *str);
		++str;
	}

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glDepthFunc(GL_LEQUAL);
	glPopAttrib();
}

void showInfo() {
	glPushMatrix();                    
	glLoadIdentity();                   

	glMatrixMode(GL_PROJECTION);        
	glPushMatrix();                     
	glLoadIdentity();                  
	gluOrtho2D(0, current_width, 0, current_height);  

	float color[4] = { 0, 0, 0, 1 };

	std::stringstream ss;
	
	string str;
	if (UP)
		str = "X100";
	else {
		if (speed == 10)
			str = "X1";
		else
			str = "X5";
	}
	ss << "Speed : " << str;
	void *font = GLUT_BITMAP_8_BY_13;
	drawString(ss.str().c_str(), 2, current_height - 13, color, font);
	ss.str("");

	glPopMatrix();                  
									
	glMatrixMode(GL_MODELVIEW);      
	glPopMatrix();                   
}

void keyboard(unsigned char key, GLint x, GLint y) {

	switch (key) {
	case 'Z':
	case 'z':
		UP = false;
		speed = 2;
		break;

	case 'X':
	case 'x':
		speed = 10;
		UP = false;

		break;
	case 'c':
	case 'C':
		UP = true;
		break;

	}
	glutPostRedisplay();
}


enum menu_enum {
	PATH = 2, PATH_SHOW = 1, TXTFILE = 3, CLEAR = 4, EXIT = 5
};

void init() {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	readText();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	showInfo();


	glPointSize(2.0);

	glBegin(GL_POINTS);
	{
		for (GLint i = 0; i < cnt; i++) {
			glColor3f(1.0, 0.0, 0);
			glVertex2f(myVertices[i][0], myVertices[i][1]);
		}
	}
	glEnd();


	if (PATH_BOOLEAN)
		readPATH();

	if (DRAW_PATH) {
		for (GLint i = 0; i < cnt; i++) {
			glBegin(GL_LINES);
			glColor3f(0.0, 1.0, 0.0);
			glVertex2f(myVertices[path[i] - 1][0], myVertices[path[i] - 1][1]);
			glVertex2f(myVertices[path[i + 1] - 1][0], myVertices[path[i + 1] - 1][1]);
			glEnd();
			if (SHOW) {
				glFlush();
				if(!UP)
					Sleep(speed);
				else
					for (int j = 0; j < 1000000000; j++) {}
			}
		}
	}

	if (SHOW)
		SHOW = !SHOW;

	glFlush();

}


void readPATH() {
	if (CONT)
		return;
	FILE * fp = fopen("result.txt", "r");
	for (int i = 0; i <= cnt; i++) {
		int x;
		fscanf(fp, "%d", &x);
		path[i] = x;
	}

	fclose(fp);
	glutPostRedisplay();
	PATH_BOOLEAN = FALSE;
	CONT = TRUE;
}


void readText() {
	FILE * fp = fopen("input.txt", "r");
	GLint num;
	int n, x, y;
	char str[100];
	for (int i = 0; i < 3; i++)
		fscanf(fp, "%s", str);
	for (int i = 0; i < 7; i++)
		fscanf(fp, "%s", str);
	fscanf(fp, "%d", &num);

	for (int i = 0; i < 7; i++)
		fgets(str, sizeof(str), fp);

	for (int i = 0; i < num; i++) {
		fscanf(fp, "%d %d %d", &n, &x, &y);
		GLfloat a = (float)x;
		GLfloat b = (float)y;
		myVertices[cnt][0] = a;
		myVertices[cnt][1] = b;
		cnt++;
	}
	fclose(fp);


	for (int i = 0; i < num; i++) {
		if (max_x < myVertices[i][0])
			max_x = myVertices[i][0];
		if (max_y < myVertices[i][1])
			max_y = myVertices[i][1];
	}


	glutPostRedisplay();
	FILE_BOOLEAN = FALSE;


}

void menu(GLint value) {
	switch (value) {
	case PATH:
		PATH_BOOLEAN = TRUE;
		DRAW_PATH = TRUE;
		SHOW = FALSE;
		break;
	case PATH_SHOW:
		PATH_BOOLEAN = TRUE;
		DRAW_PATH = TRUE;
		SHOW = TRUE;
		break;

	case CLEAR:
		DRAW_PATH = FALSE;
		SHOW = FALSE;
		break;

	case EXIT:
		exit(0);
		break;
	}
	glutPostRedisplay();
}


void reshape(int width, int height) {
	current_width = width;
	current_height = height;

	glViewport(0, 0, width, height);
	glLoadIdentity();
	glOrtho(-5.0, max_x + 5.0, -5.0, max_y + 5.0, -1.0, 1.0);
}



int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {
	glutInitWindowSize(current_width, current_height);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("TSP Graphic");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);


	glutCreateMenu(menu);
	glutAddMenuEntry("TSP path view", 2);
	glutAddMenuEntry("TSP path view - trace", 1);

	glutAddMenuEntry("clear path", 4);
	glutAddMenuEntry("exit", 5);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();

	glutMainLoop();
	system("pause");
	return 0;


}


