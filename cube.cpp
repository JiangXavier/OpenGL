#include "windows.h"
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#define PI 3.1415926
bool smoothEnabled = true;
bool drawLines = false;
bool redrawContinue = true; // 是否重绘
float mynear = 3.0; 
float myfar = 7.0;  
float viewxform_z = -5.0;
int winWidth, winHeight;
float angle = 0.0, axis[3]; // 用于对x、y、z轴的变换
int operatorType = 0;    // 操作类型，旋转 = 0、平移 = 1、缩放 = 2，默认为旋转
bool rotateMove = false; //旋转
bool translateMove = false; //平移
bool scaleMove = false;     //缩放
//正方体所有顶点坐标和顶点颜色
GLfloat vertices[][3] = { // 8个顶点的坐标
	{-1.0, -1.0, -1.0}, {1.0, -1.0, -1.0}, {1.0, 1.0, -1.0}, {-1.0, 1.0, -1.0},
	{-1.0, -1.0, 1.0},  {1.0, -1.0, 1.0},  {1.0, 1.0, 1.0},  {-1.0, 1.0, 1.0} };
GLfloat colors[][3] = { // 8个顶点的颜色 -> 根据 userSettings 函数中的 GL_SMOOTH
						// / GL_FLAT 填充面的颜色
	{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}, {0.0, 1.0, 0.0},
	{0.0, 0.0, 1.0}, {1.0, 0.0, 1.0}, {1.0, 1.0, 1.0}, {0.0, 1.0, 1.0} };
GLfloat objectXform[4][4] = { {1.0, 0.0, 0.0, 0.0},
							 {0.0, 1.0, 0.0, 0.0},
							 {0.0, 0.0, 1.0, 0.0},
							 {0.0, 0.0, 0.0, 1.0} };
void setMenuEntries();
void initSettings(void) {
	glLineWidth(3.0);
	glEnable(GL_DEPTH_TEST);
	setMenuEntries(); // 添加菜单
}
// 根据4个点画1个面
//建面函数围成正方体
void polygon(int a, int b, int c, int d, int face) 
{
	if (drawLines) {
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINE_LOOP);
		glVertex3fv(vertices[a]);
		glVertex3fv(vertices[b]);
		glVertex3fv(vertices[c]);
		glVertex3fv(vertices[d]);
		glEnd();
	}
	else {
		glBegin(GL_POLYGON);
		glColor3fv(colors[a]);
		glVertex3fv(vertices[a]);
		glColor3fv(colors[b]);
		glVertex3fv(vertices[b]);
		glColor3fv(colors[c]);
		glVertex3fv(vertices[c]);
		glColor3fv(colors[d]);
		glVertex3fv(vertices[d]);
		glEnd();
	}
}
//colorcube函数初始化正方体对象
void colorcube(void) {
	polygon(1, 0, 3, 2, 0);
	polygon(3, 7, 6, 2, 1);
	polygon(7, 3, 0, 4, 2);
	polygon(2, 6, 5, 1, 3);
	polygon(4, 5, 6, 7, 4);
	polygon(5, 4, 0, 1, 5);
}
float lastPos[3] = { 0.0F, 0.0F, 0.0F };
int curx, cury;
int lastx, lasty;
int startX, startY;
// 将坐标投影到中心在原点的半球体
void trackball_ptov(int x, int y, int width, int height, float v[3]) {
	float d, a;
	v[0] = (2.0F * x - width) / width;
	v[1] = (height - 2.0F * y) / height;
	d = (float)sqrt(v[0] * v[0] + v[1] * v[1]);
	v[2] = (float)cos((PI / 2.0F) * ((d < 1.0F) ? d : 1.0F));
	a = 1.0F / (float)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] *= a;
	v[1] *= a;
	v[2] *= a;
}
// 鼠标移动动作由glutMotionFunc的绑定函数mouseMotion实现
void mouseMotion(int x, int y) {
	if (!redrawContinue)
		return;
	float abc[3], dx, dy, dz;
	if (rotateMove) { // 旋转
		trackball_ptov(x, y, winWidth, winHeight, abc);
		//计算三个方向的位移
		dx = abc[0] - lastPos[0];
		dy = abc[1] - lastPos[1];
		dz = abc[2] - lastPos[2];
		if (dx || dy || dz) {
			angle = 90.0F * sqrt(dx * dx + dy * dy + dz * dz);
			// YoZ平面旋转的有向面积（由于更新频率，每次都是一个菱形），因为半径为1，其值为sin(alpha)
			axis[0] = lastPos[1] * abc[2] - lastPos[2] * abc[1];
			// 根据右手定则
			axis[1] = lastPos[2] * abc[0] - lastPos[0] * abc[2];
			axis[2] = lastPos[0] * abc[1] - lastPos[1] * abc[0];
			// 在3个轴上的旋转比例 alpha : beta : gama = sin(alpha) : sin(beta)
			// : sin(gama) 只需控制旋转比例即可，angle控制旋转快慢
			lastPos[0] = abc[0];
			lastPos[1] = abc[1];
			lastPos[2] = abc[2];
		}
	}
	else if (translateMove) {
		curx = x;
		cury = y;
		dx = curx - lastx;
		dy = cury - lasty;
		if (dx || dy) {
			axis[0] = dx * (10.0f / winWidth);
			axis[1] = -dy * (10.0f / winHeight);
			axis[2] = 0;
			lastx = curx;
			lasty = cury;
		}
	}
	else if (scaleMove) {
		float k_scale = 2; // 缩放系数，控制快慢
		curx = x;
		cury = y;
		dx = 2.0F * (curx - lastx) / winWidth * k_scale;
		dy = 2.0F * (lasty - cury) / winHeight * k_scale;
		if (dx || dy) {
			axis[0] = dx;
			axis[1] = dy;
			axis[2] = 0;
			lastx = curx;
			lasty = cury;
		}
	}
	else
		return;
	glutPostRedisplay();
}
//按下鼠标
void startMotion(long time, int button, int x, int y, int type) {
	rotateMove = false;
	translateMove = false;
	scaleMove = false;
	startX = x;
	startY = y;
	curx = x;
	cury = y;
	lastx = startX, lasty = startY;
	switch (type) { // 根据operatorType切换操作方式
	case 0:         // 旋转
		trackball_ptov(x, y, winWidth, winHeight, lastPos);
		rotateMove = true;
		break;
	case 1: // 平移
		translateMove = true;
		break;
	case 2: // 放缩
		scaleMove = true;
		break;
	default:
		return;
		break;
	}
}
//弹起鼠标
void stopMotion(long time, int button, int x, int y, int type) {
	angle = 0.0F;
	rotateMove = false;
	translateMove = false;
	scaleMove = false;
}
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, viewxform_z);
	if (redrawContinue) { // 如果重绘
		glPushMatrix();
		// 将当前状态压栈
		glLoadIdentity();
		// 矩阵重置
		if (rotateMove) {
			// 调用旋转API glRotatef();
			glRotatef(angle, axis[0], axis[1], axis[2]); // 旋转
		}
		else if (translateMove) {
			// 调用平移API glTranslate ()
			glTranslatef(axis[0], axis[1], axis[2]); // 平移
		}
		else if (scaleMove) {
			// 调用缩放API glScalef()
			glScalef(1 + axis[0], 1 + axis[1], 1 + axis[2]); // 缩放
		}
		// 物体初始状态（可不为单位矩阵）
		glMultMatrixf((GLfloat*)objectXform);
		// 将上述矩阵乘积存储到物体变换矩阵 objectXform
		glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)objectXform);
		// 将之前保存的状态出栈
		glPopMatrix();
	}
	glPushMatrix();
	glMultMatrixf((GLfloat*)objectXform); // 将用户坐标系根据objectXform变换
	colorcube();                           // 在此处画正方体
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
}
//mouseButton函数管理鼠标按键的响应事件
void mouseButton(int button, int state, int x, int y) {
	switch (button) {
	case GLUT_LEFT_BUTTON:
		break;
	case GLUT_MIDDLE_BUTTON:
		break;
	case GLUT_RIGHT_BUTTON:
		break;
	}//该分支判断鼠标左键，中键，右键
	switch (state) {
	case GLUT_DOWN:
		startMotion(0, 1, x, y, operatorType);
		break;
	case GLUT_UP:
		stopMotion(0, 1, x, y, operatorType);
		break;
	}//该分支判断鼠标按下或弹起
}
//保持纵横比
void myReshape(int w, int h) {
	glViewport(0, 0, w, h); // 设置视口大小和窗口一致
	winWidth = w;           // 记录当前窗口大小
	winHeight = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho(-5.0, 5.0, -5.0 * (GLfloat)h / (GLfloat)w,
			5.0 * (GLfloat)h / (GLfloat)w, mynear, myfar);
	else
		glOrtho(-5.0 * (GLfloat)w / (GLfloat)h, 5.0 * (GLfloat)w / (GLfloat)h,
			-5.0, 5.0, mynear, myfar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void userEventAction(int key) {
	rotateMove = false;
	translateMove = false;
	scaleMove = false;
	switch (key) {
	case '0': 
		angle = 0.0f;
		memset(axis, 0, sizeof(axis));               // axis清零
		memset(objectXform, 0, sizeof(objectXform)); // 变换矩阵重置
		// 重新构造单位矩阵
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (i == j) {
					objectXform[i][j] = 1.0f;}}}
		break;
	case 'r': // 旋转
		operatorType = 0;
		break;
	case 't': // 平移
		operatorType = 1;
		break;
	case 's': // 缩放
		operatorType = 2;
		break;
	case '1': // wire frame/polygon   // 线框图/多边形
		drawLines = !drawLines;
		break;
	case '2':
		smoothEnabled = !smoothEnabled; // GL_SMOOTH / GL_FLAT
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}
	glutPostRedisplay(); // 重绘
}
//键盘绑定函数将按键信息转发到userEventAction函数
void keyboard(unsigned char key, int x, int y) {
	userEventAction(key); // 调用通用动作解释函数
}
// 箭头按键
void arrow(int key, int x, int y) {
	startMotion(0, 1, x, y, operatorType); // 设置操作的布尔值
	angle = 10.0F;
	switch (key) {
	case GLUT_KEY_LEFT:
		if (rotateMove) { // 根据 y 轴旋转
			axis[0] = 0;
			axis[1] = -0.1;
			axis[2] = 0;}
		else if (translateMove) {
			axis[0] = -0.1;
			axis[1] = 0;
			axis[2] = 0;}
		else if (scaleMove) {
			axis[0] = -0.1;
			axis[1] = 0;
			axis[2] = 0;}
		break;
	case GLUT_KEY_UP:
		if (rotateMove) { // 根据 x 轴旋转
			axis[0] = -0.1;
			axis[1] = 0;
			axis[2] = 0;
		}
		else if (translateMove) {
			axis[0] = 0;
			axis[1] = 0.1;
			axis[2] = 0;
		}
		else if (scaleMove) {
			axis[0] = 0;
			axis[1] = 0.1;
			axis[2] = 0;
		}
		break;
	case GLUT_KEY_RIGHT:
		if (rotateMove) {
			axis[0] = 0;
			axis[1] = 0.1;
			axis[2] = 0;
		}
		else if (translateMove) {
			axis[0] = 0.1;
			axis[1] = 0;
			axis[2] = 0;
		}
		else if (scaleMove) {
			axis[0] = 0.1;
			axis[1] = 0;
			axis[2] = 0;
		}
		break;
	case GLUT_KEY_DOWN:
		if (rotateMove) {
			axis[0] = 0.1;
			axis[1] = 0;
			axis[2] = 0;
		}
		else if (translateMove) {
			axis[0] = 0;
			axis[1] = -0.1;
			axis[2] = 0;
		}
		else if (scaleMove) {
			axis[0] = 0;
			axis[1] = -0.1;
			axis[2] = 0;
		}
		break;
	default:
		break;
	}
	glutPostRedisplay();
}
void arrowKeyup(int key, int x, int y) {
	angle = 0.0F;
	rotateMove = false;
	translateMove = false;
	scaleMove = false;
}
//创建数组结构，创建初始化菜单
typedef struct menuEntryStruct { // 键-值对 数据结构
	const char* label;
	char key;
} menuEntryStruct;
static menuEntryStruct mainMenu[] =
{
	"Reset",          '0', 
	"Rotate",         'r',
	"Translate",      't',
	"Scale",          's',
	"lines/polygons", '1',
	"flat/smooth",    '2',
	"quit",            27,
};
int mainMenuEntries = sizeof(mainMenu) / sizeof(menuEntryStruct);
void selectMain(int choice) { // 序号->key 映射
	userEventAction(mainMenu[choice].key); // 调用通用动作解释函数
}
void setMenuEntries() {
	glutCreateMenu(selectMain); // 使用selectMain作为菜单调用函数
	for (int i = 0; i < mainMenuEntries; i++) {
		glutAddMenuEntry(mainMenu[i].label, i);
	}
	glutAttachMenu(GLUT_RIGHT_BUTTON); // 菜单绑定右键
}
int main(int argc, char** argv) {
	glutInit(&argc, argv);//初始化GLUT库
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(600, 200);
	glutInitWindowSize(500, 500);//窗口位置和大小
	glutCreateWindow("OpenGL图形几何变换");
	glutReshapeFunc(myReshape);  // 窗口变化保持原有纵横比
	glutDisplayFunc(display);    // 显示
	glutMouseFunc(mouseButton);  // 注册鼠标按键事件
	glutMotionFunc(mouseMotion); // 注册鼠标移动函数
	glutKeyboardFunc(keyboard);  // 键盘函数
	glutSpecialFunc(arrow);      // 特殊函数
	initSettings(); // 初始化
	glutMainLoop(); 
	return 0;
}