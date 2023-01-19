#include <GL/glut.h>
#include <math.h>
#define PI 3.1415926
#define PI2 6.2831853

GLsizei width = 500, height = 500;
class Point
{
public:
    Point(double a, double b, double c) :x(a), y(b), z(c) {};
public:6
    double x;
    double y;
    double z;
};//宏定义，类
int up = 50, down = 50;
//实现类
Point getPoint(double u, double v)
{
    double x = sin(PI * v) * cos(PI2 * u);
    double y = sin(PI * v) * sin(PI2 * u);
    double z = cos(PI * v);
    return Point(x, y, z);
}//得到球体上点的坐标

void drawWire()
{
    double ustep = 1 / (double)up, vstep = 1 / (double)down;
    double u = 0, v = 0;
    //绘制下端三角形组
    for (int i = 0; i < up; i++)
    {
        glBegin(GL_LINE_LOOP);//绘制折线
        Point a = getPoint(0, 0);
        glVertex3d(a.x, a.y, a.z); //设置顶点坐标
        Point b = getPoint(u, vstep);
        glVertex3d(b.x, b.y, b.z);
        Point c = getPoint(u + ustep, vstep);
        glVertex3d(c.x, c.y, c.z);
        u += ustep;
        glEnd();
    }
    ////绘制中间四边形组
    u = 0, v = vstep;
    for (int i = 1; i < down - 1; i++)
    {
        for (int j = 0; j < up; j++)
        {
            glBegin(GL_LINE_LOOP);
            Point a = getPoint(u, v);
            Point b = getPoint(u + ustep, v);
            Point c = getPoint(u + ustep, v + vstep);
            Point d = getPoint(u, v + vstep);
            glVertex3d(a.x, a.y, a.z);
            glVertex3d(b.x, b.y, b.z);
            glVertex3d(c.x, c.y, c.z);
            glVertex3d(d.x, d.y, d.z);
            u += ustep;
            glEnd();
        }
        v += vstep;
    }
    //绘制下端三角形组
    u = 0;
    for (int i = 0; i < up; i++)
    {
        glBegin(GL_LINE_LOOP);
        Point a = getPoint(0, 1);
        Point b = getPoint(u, 1 - vstep);
        Point c = getPoint(u + ustep, 1 - vstep);
        glVertex3d(a.x, a.y, a.z);
        glVertex3d(b.x, b.y, b.z);
        glVertex3d(c.x, c.y, c.z);
        glEnd();
    }
}
//显示，绘制三维球体
void init()
{
    glClearColor(1,1, 1, 1);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清除缓冲区
    glClearColor(1, 1, 1, 1);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLfloat light_position[] = { 0.5f, 0.5f, 1.0f, 0.0f };
    GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 0.2f };
    GLfloat light_diffuse[] = { 0.5f, 0.5f, 0.5f, 0.2f };
    GLfloat light_specular[] = { 0.5f, 0.5f, 0.5f, 0.2f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}
//初始化背景

//保持纵横比，改变窗口大小时图形不变形
void winReshapeFcn(int newWidth, int newHeight)
{
    GLfloat ratio = (GLfloat)newWidth / (GLfloat)newHeight;
    glViewport(0, 0, newWidth, newHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (newWidth <= newHeight)
        gluOrtho2D(-1.5, 1.5, -1.5 / ratio, 1.5 / ratio);//指定屏幕区域对应的模型坐标范围
    else
        gluOrtho2D(-1.5 * ratio, 1.5 * ratio, -1.5, 1.5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void displayFunc()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();//恢复初始坐标系
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(0.0, 1.0, 1.0);//绘制曲线颜色
    glPointSize(1.0);
    glRotated(30, 1, 0, 0);
    glRotated(60, 0, 1, 0);
    glRotated(90, 0, 0, 1);
    drawWire();
    glutSwapBuffers();//交换两个缓冲区指针
}
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(600,200);
    glutInitWindowSize(0.8*width, 0.8*height);
    glutCreateWindow("三维球体");
    init();
    glutDisplayFunc(displayFunc);
    glutReshapeFunc(winReshapeFcn);
    glutMainLoop();
    return 0;
}