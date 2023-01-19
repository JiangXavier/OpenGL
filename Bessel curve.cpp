#include <Windows.h>
#include <GL/glut.h>
#include <math.h>
class Point
{
public:
	int x, y;
	void setxy(int _x, int _y) {
		x = _x;
		y = _y;
	}
};
static int POINTSNUM = -1;
static Point pointset[20];
//定义组合函数
double C(double n, double k)
{
	double m = 1, s = 1;
	int i;
	for (i = 1; i <= n; i++)
	{
		m *= i;
	}
	for (i = 1; i <= k; i++)
	{
		s *= i;
	}
	for (i = 1; i <= n - k; i++)
	{
		s *= i;
	}
	return (double)(m / s);
}
//设置点
void setPoint(Point p) {
	glBegin(GL_POINTS);
	glVertex2f(p.x, p.y);
	glEnd();
	glFlush();
}
//设置线
void setline(Point p1, Point p2) {
	glBegin(GL_LINES);
	glVertex2f(p1.x, p1.y);
	glVertex2f(p2.x, p2.y);
	glEnd();
	glFlush();
}
//贝塞尔曲线函数
double BEZ(double t, int n, int k)
{
	return (C(n, k) * pow(t, k) * pow(1 - t, n - k));
}
//曲线方程
Point setBezier(Point* ps, double t, int n) {
	Point p;
	int i;
	for (i = 0, p.x = 0, p.y = 0; i <= n; i++)
	{
		p.x += BEZ(t, n, i) * ps[i].x;
		p.y += BEZ(t, n, i) * ps[i].y;
	}
	return p;
}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}
void exitt(unsigned char key, int x, int y)
{
	if (key == 27)   exit(0);
}
void mymouseFunction(int button, int state, int x, int y) {
	if (state == GLUT_DOWN)
	{
		pointset[++POINTSNUM].setxy(x, 960 - y);
		setPoint(pointset[POINTSNUM]);
		glColor3f(1.0, 1.0, 1.0);
		if (POINTSNUM > 0) { glLineWidth(1.0); }
		if (POINTSNUM >= 19) { POINTSNUM = -1; }
		else if (POINTSNUM > 0) {
			glColor3f(1, 1, 1);
			Point p_current = pointset[0];
			for (double t = 0.0; t <= 1.0; t += 0.001)
			{
				Point P = setBezier(pointset, t, POINTSNUM);
				glLineWidth(2.0);
				setline(p_current, P);
				p_current = P;
			}
		}
	}
}
//初始化函数
void init(void)
{
	glClearColor(0, 0, 0, 0);
	glColor3f(1.0, 1.0, 1.0);
	glPointSize(8.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 1200.0, 0.0, 960.0);
}
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(1200,960);
	glutInitWindowPosition(100,100);
	glutCreateWindow("贝塞尔曲线");
	init();
	glutKeyboardFunc(exitt);
	glutMouseFunc(mymouseFunction);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}