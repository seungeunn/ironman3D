#include "ObjParser.h"
#include "bmpfuncs.h"
#include <Windows.h>
#include <GL/glext.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500
#define M_PI 3.1415926535897

using std::cos;
using std::sin;
using std::sqrt;
double radius = 11;
double theta = 80, phi = 1;
double cam[3];
double center[3] = { 0, 0, 0 };
double up[3] = { 0, cos(phi * M_PI / 180), 0 };

POINT ptMouse;

int assemble = 0;
double a = 0;
int individual = 0;
int color = 0;
int background = 0;
int houseparty = 0;
double h[5] = { 0,0,0,0,0 };
int smartgun = 0;
int repulsorbeam = 0;
double r = 0;
int score[3] = { 5,5,5 };

ObjParser* face = new ObjParser("obj/face.obj");
ObjParser* helmet = new ObjParser("obj/helmet.obj");
ObjParser* body = new ObjParser("obj/body.obj");
ObjParser* arc = new ObjParser("obj/arc.obj");
ObjParser* arm1gold = new ObjParser("obj/arm1gold.obj");
ObjParser* arm1goldleft = new ObjParser("obj/arm1goldleft.obj");
ObjParser* arm1goldright = new ObjParser("obj/arm1goldright.obj");
ObjParser* arm1red = new ObjParser("obj/arm1red.obj");
ObjParser* arm1redleft = new ObjParser("obj/arm1redleft.obj");
ObjParser* arm1redright = new ObjParser("obj/arm1redright.obj");
ObjParser* arm2 = new ObjParser("obj/arm2.obj");
ObjParser* arm2left = new ObjParser("obj/arm2left.obj");
ObjParser* arm2right = new ObjParser("obj/arm2right.obj");
ObjParser* gun = new ObjParser("obj/gun.obj");
ObjParser* hand = new ObjParser("obj/hand.obj");
ObjParser* handleft = new ObjParser("obj/handleft.obj");
ObjParser* handright = new ObjParser("obj/handright.obj");
ObjParser* handright2 = new ObjParser("obj/handright2.obj");
ObjParser* handarc = new ObjParser("obj/handarc.obj");
ObjParser* handarcleft = new ObjParser("obj/handarcleft.obj");
ObjParser* handarcright = new ObjParser("obj/handarcright.obj");
ObjParser* handarcright2 = new ObjParser("obj/handarcright2.obj");
ObjParser* leg1gold = new ObjParser("obj/leg1gold.obj");
ObjParser* leg1red = new ObjParser("obj/leg1red.obj");
ObjParser* leg2 = new ObjParser("obj/leg2.obj");
ObjParser* sil = new ObjParser("obj/silver.obj");
ObjParser* footarc = new ObjParser("obj/footarc.obj");

/* obj texture mapping */
GLuint redtexture;
GLuint goldtexture;
GLuint silvertexture;
GLuint pinktexture;
GLuint bluetexture;
GLuint whitetexture;
/* background texture mapping */
GLuint hallCubeTex;
GLuint universeCubeTex;
GLuint oceanCubeTex;
/* smart gun texture mapping */
GLuint cubeTex[6];
GLuint cylinderTex[3];
GLuint* sphereTex = new GLuint();
/* quadric object 객체 생성 */
GLUquadricObj* qobj = gluNewQuadric();

void draw_ironman();
void setEnvironmentMap();
void draw_skyBox(GLuint);
void housepartyProtocol();
void smartGun();
void cubeTextureMapping();
void cylinderTextureMapping();
void sphereTextureMapping();
void draw_textureCube();
void draw_Cylinder();
void draw_Sphere();
void setTextureMapping();
void draw_obj(ObjParser*, GLuint);
void init();
void light_default();
void idle();
void resize(int, int);
void draw();
void draw_axis();
void keyboard(unsigned char, int, int);
void specialkeyboard(int, int, int);
void mouseWheel(int, int, int, int);
void mouse(int, int, int, int);
void sub_menu1(int);
void sub_menu2(int);
void sub_menu3(int);
void main_menu(int);
void printInstruction();

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(300, 200);
	glutCreateWindow("12201933 이승은 Term Project");
	init(); // -> 사용자 초기화 함수

	// pop-up 메뉴 등록 함수
	int submenu1 = glutCreateMenu(sub_menu1);
	glutAddMenuEntry("Red", 0);
	glutAddMenuEntry("Pink", 10);
	glutAddMenuEntry("Blue", 11);

	int submenu2 = glutCreateMenu(sub_menu2);
	glutAddMenuEntry("Universe", 20);
	glutAddMenuEntry("Hall of armor", 21);
	glutAddMenuEntry("Ocean", 22);

	int submenu3 = glutCreateMenu(sub_menu3);
	glutAddMenuEntry("face", 30);
	glutAddMenuEntry("helmet", 31);
	glutAddMenuEntry("body", 32);
	glutAddMenuEntry("upper arm", 33);
	glutAddMenuEntry("under arm", 34);
	glutAddMenuEntry("hand", 35);
	glutAddMenuEntry("upper leg", 36);
	glutAddMenuEntry("under leg", 37);

	int mainmenu = glutCreateMenu(main_menu);
	glutAddMenuEntry("Init", 1);
	glutAddSubMenu("Color", submenu1);
	glutAddSubMenu("Background", submenu2);
	glutAddMenuEntry("Assemble", 2);
	glutAddSubMenu("Individual", submenu3);
	glutAddMenuEntry("Quit", 99);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// callback function
	glutMouseWheelFunc(&mouseWheel);
	glutMouseFunc(&mouse);
	glutKeyboardFunc(&keyboard);
	glutSpecialFunc(&specialkeyboard);
	glutDisplayFunc(&draw);
	glutReshapeFunc(&resize);
	glutIdleFunc(&idle);

	// looping  
	glutMainLoop();

	return 0;
}

void light_default() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// lignt0 setting
	GLfloat ambientLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat diffuseLight[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat specularLight[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	/************ Material  setting ************/
	GLfloat specularMaterial[] = { 0.8f, 0.8f, 0.8f, 1.5f };
	GLfloat diffuseMaterial[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat ambientMaterial[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, 32);
	
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glDisable(GL_COLOR_MATERIAL);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// depth test enable
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);	
}

void init() {
	// clear background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);

	// set blend
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	// set antialiasing
	glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
	glEnable(GL_LINE_SMOOTH);

	// set light
	light_default();

	// texture mapping set
	glEnable(GL_TEXTURE_2D);
	gluQuadricTexture(qobj, GL_TRUE);
	setEnvironmentMap();
	setTextureMapping();
	cubeTextureMapping();
	cylinderTextureMapping();
	sphereTextureMapping();

	// print instuction
	printInstruction();
}

void resize(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (double)width / (double)height, 1, 500);
	glMatrixMode(GL_MODELVIEW);
}

void idle() {
	if (assemble == 1) {
		a = a + 0.015;
		if (a >= 6) {
			a = 6;
		}
	}
	if (houseparty == 1) {
		h[0] = h[0] + 0.6;
		if (h[0] >= 20) {
			h[0] = 20;
		}
		h[1] = h[1] + 0.45;
		if (h[1] >= 20) {
			h[1] = 20;
		}	
		h[2] = h[2] + 0.3;
		if (h[2] >= 20) {
			h[2] = 20;
		}
		h[3] = h[3] + 0.5;
		if (h[3] >= 25) {
			h[3] = 25;
		}
		h[4] = h[4] + 0.45;
		if (h[4] >= 25) {
			h[4] = 25;
		}
	}
	if (repulsorbeam == 1) {
		r = r + 1;
		if (r >= 90) {
			r = 90;
		}
	}
	glutPostRedisplay();
}

void setTextureMapping() {
	int imgWidth, imgHeight, channels;
	uchar* img = readImageData("img/gold.bmp", &imgWidth, &imgHeight, &channels);
	glGenTextures(1, &goldtexture);
	glBindTexture(GL_TEXTURE_2D, goldtexture);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("img/silver.bmp", &imgWidth, &imgHeight, &channels);
	glGenTextures(1, &silvertexture);
	glBindTexture(GL_TEXTURE_2D, silvertexture);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("img/white.bmp", &imgWidth, &imgHeight, &channels);
	glGenTextures(1, &whitetexture);
	glBindTexture(GL_TEXTURE_2D, whitetexture);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("img/red.bmp", &imgWidth, &imgHeight, &channels);
	glGenTextures(1, &redtexture);
	glBindTexture(GL_TEXTURE_2D, redtexture);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("img/pink.bmp", &imgWidth, &imgHeight, &channels);
	glGenTextures(1, &pinktexture);
	glBindTexture(GL_TEXTURE_2D, pinktexture);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("img/blue.bmp", &imgWidth, &imgHeight, &channels);
	glGenTextures(1, &bluetexture);
	glBindTexture(GL_TEXTURE_2D, bluetexture);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void setEnvironmentMap() {
	int imgWidth, imgHeight, channels;
	uchar* img0 = readImageData("img/lefthall.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img1 = readImageData("img/righthall.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img2 = readImageData("img/black.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img3 = readImageData("img/black.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img4 = readImageData("img/black.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img5 = readImageData("img/casee1.bmp", &imgWidth, &imgHeight, &channels);

	glGenTextures(1, &hallCubeTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, hallCubeTex);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img1);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img2);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img3);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img4);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img5);

	glBindTexture(GL_TEXTURE_CUBE_MAP, hallCubeTex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	img0 = readImageData("img/1024px.bmp", &imgWidth, &imgHeight, &channels);
	img1 = readImageData("img/1024nx.bmp", &imgWidth, &imgHeight, &channels);
	img2 = readImageData("img/1024py.bmp", &imgWidth, &imgHeight, &channels);
	img3 = readImageData("img/1024ny.bmp", &imgWidth, &imgHeight, &channels);
	img4 = readImageData("img/1024pz.bmp", &imgWidth, &imgHeight, &channels);
	img5 = readImageData("img/1024nz.bmp", &imgWidth, &imgHeight, &channels);

	glGenTextures(1, &oceanCubeTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, oceanCubeTex);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img1);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img2);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img3);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img4);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img5);

	glBindTexture(GL_TEXTURE_CUBE_MAP, oceanCubeTex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	img0 = readImageData("img/universe.bmp", &imgWidth, &imgHeight, &channels);

	glGenTextures(1, &universeCubeTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, universeCubeTex);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, universeCubeTex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_CUBE_MAP);
}

void cubeTextureMapping() {
	glGenTextures(6, cubeTex);
	int imgWidth, imgHeight, channels;
	for (int i = 0; i < 6; i++) {
		glBindTexture(GL_TEXTURE_2D, cubeTex[i]);
		char buf[100];
		sprintf(buf, "img/TexImage%d.bmp", i);
		buf[strlen(buf)] = 0;
		unsigned char* img = readImageData(buf, &imgWidth, &imgHeight, &channels);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
}

void cylinderTextureMapping() {
	glGenTextures(3, cylinderTex);

	glBindTexture(GL_TEXTURE_2D, cylinderTex[0]);
	int width, height, channels;
	unsigned char* img = readImageData("img/CIDER_T.bmp", &width, &height, &channels);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, cylinderTex[1]);
	img = readImageData("img/coke.bmp", &width, &height, &channels);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, cylinderTex[2]);
	img = readImageData("img/CIDER_B.bmp", &width, &height, &channels);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void sphereTextureMapping() {
	glGenTextures(1, sphereTex);
	glBindTexture(GL_TEXTURE_2D, *sphereTex);
	int width, height, channels;
	unsigned char* img = readImageData("img/EARTH.bmp", &width, &height, &channels);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void draw_textureCube() {
	glColor3f(1.0, 1.0, 1.0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, cubeTex[0]);
	glBegin(GL_QUADS);
	glNormal3f(-1.0, 0, 0);   // -x axis
	glTexCoord2f(0, 0); glVertex3f(-1.0, 1.0, -1.0);
	glTexCoord2f(1, 0); glVertex3f(-1.0, -1.0, -1.0);
	glTexCoord2f(1, 1); glVertex3f(-1.0, -1.0, 1.0);
	glTexCoord2f(0, 1); glVertex3f(-1.0, 1.0, 1.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, cubeTex[1]);
	glBegin(GL_QUADS);
	glNormal3f(1.0, 0, 0);   // x axis
	glTexCoord2f(0, 0); glVertex3f(1.0, 1.0, 1.0);
	glTexCoord2f(1, 0); glVertex3f(1.0, -1.0, 1.0);
	glTexCoord2f(1, 1); glVertex3f(1.0, -1.0, -1.0);
	glTexCoord2f(0, 1); glVertex3f(1.0, 1.0, -1.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, cubeTex[2]);
	glBegin(GL_QUADS);
	glNormal3f(0, -1.0, 0);   // -y axis
	glTexCoord2f(0, 0); glVertex3f(-1.0, -1.0, -1.0);
	glTexCoord2f(1, 0); glVertex3f(1.0, -1.0, -1.0);
	glTexCoord2f(1, 1); glVertex3f(1.0, -1.0, 1.0);
	glTexCoord2f(0, 1); glVertex3f(-1.0, -1.0, 1.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, cubeTex[3]);
	glBegin(GL_QUADS);
	glNormal3f(0, 1.0, 0);   // y axis
	glTexCoord2f(0, 0); glVertex3f(-1.0, 1.0, 1.0);
	glTexCoord2f(1, 0); glVertex3f(1.0, 1.0, 1.0);
	glTexCoord2f(1, 1); glVertex3f(1.0, 1.0, -1.0);
	glTexCoord2f(0, 1); glVertex3f(-1.0, 1.0, -1.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, cubeTex[4]);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1.0);   // z axis
	glTexCoord2f(0, 0); glVertex3f(1.0, 1.0, 1.0);
	glTexCoord2f(1, 0); glVertex3f(-1.0, 1.0, 1.0);
	glTexCoord2f(1, 1); glVertex3f(-1.0, -1.0, 1.0);
	glTexCoord2f(0, 1); glVertex3f(1.0, -1.0, 1.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, cubeTex[5]);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, -1.0);   // -z axis
	glTexCoord2f(0, 0); glVertex3f(1.0, 1.0, -1.0);
	glTexCoord2f(1, 0); glVertex3f(-1.0, 1.0, -1.0);
	glTexCoord2f(1, 1); glVertex3f(-1.0, -1.0, -1.0);
	glTexCoord2f(0, 1); glVertex3f(1.0, -1.0, -1.0);
	glEnd();
}

void draw_Cylinder() {
	glColor3f(1.0, 1.0, 1.0);

	/* 윗면 - Disk */
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, cylinderTex[0]);
	glTranslatef(-2, 1, 0);
	glRotatef(-90, 1, 0, 0);
	gluDisk(qobj, 0, 1, 12, 1);
	glPopMatrix();

	/* 옆면 - Cylinder */
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, cylinderTex[1]);
	glTranslatef(-2, -2, 0);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(qobj, 1, 1, 3, 12, 1);
	glPopMatrix();

	/* 아랫면 - Disk */
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, cylinderTex[2]);
	glTranslatef(-2, -2, 0);
	glRotatef(90, 1, 0, 0);
	gluDisk(qobj, 0, 1, 12, 1);
	glPopMatrix();
}

void draw_Sphere() {
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, *sphereTex);
	glRotatef(-90, 1, 0, 0);
	gluSphere(qobj, 2, 24, 24);
	glRotatef(90, 1, 0, 0);
}

void draw_obj(ObjParser* objParser, GLuint texture) {
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_TRIANGLES);
	for (unsigned int n = 0; n < objParser->getFaceSize(); n += 3) {
		glNormal3f(objParser->normal[objParser->normalIdx[n] - 1].x,
			objParser->normal[objParser->normalIdx[n] - 1].y,
			objParser->normal[objParser->normalIdx[n] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n] - 1].x,
			objParser->vertices[objParser->vertexIdx[n] - 1].y,
			objParser->vertices[objParser->vertexIdx[n] - 1].z);

		glNormal3f(objParser->normal[objParser->normalIdx[n + 1] - 1].x,
			objParser->normal[objParser->normalIdx[n + 1] - 1].y,
			objParser->normal[objParser->normalIdx[n + 1] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 1] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].z);

		glNormal3f(objParser->normal[objParser->normalIdx[n + 2] - 1].x,
			objParser->normal[objParser->normalIdx[n + 2] - 1].y,
			objParser->normal[objParser->normalIdx[n + 2] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 2] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].z);
	}
	glEnd();
	glEnable(GL_BLEND);
}

void draw_ironman() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	if (individual == 0) {
		if (assemble == 1) {
			glPushMatrix();
			glTranslatef(0, 0, 6 - a);
		}
		draw_obj(face, goldtexture);
		if (assemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(-6 + a, 0, 0);
		}
		draw_obj(arm1goldleft, goldtexture);
		if (assemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(6 - a, 0, 0);
		} 
		if (repulsorbeam == 1) {
			glPushMatrix();
			glRotatef(-r, 1, 0, 0);
			glTranslatef(0, -0.6, 1.4);
		}
		draw_obj(arm1goldright, goldtexture);
		if (repulsorbeam == 1) {
			glPopMatrix();
		}
		if (assemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(-6 + a, -6 + a, 0);
		}
		draw_obj(arm2left, goldtexture);
		if (assemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(6 - a, -6 + a, 0);
		}
		if (assemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, 0, 6 - a);
		}
		if (repulsorbeam == 1) {
			glPushMatrix();
			glRotatef(-r, 1, 0, 0);
			glTranslatef(-0.1, -0.6, 1.4);
		}
		draw_obj(arm2right, goldtexture);
		if (repulsorbeam == 1) {
			glPopMatrix();
		}
		if (assemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, -6 + a, 0);
		}
		draw_obj(leg1gold, goldtexture);
		if (assemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, 6 - a, 0);
		}
		if (color == 0) draw_obj(helmet, redtexture);
		else if(color == 10) draw_obj(helmet, pinktexture);
		else if(color == 11) draw_obj(helmet, bluetexture);

		if (assemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, 0, -6 + a);
		}
		if (color == 0) draw_obj(body, redtexture);
		else if (color == 10) draw_obj(body, pinktexture);
		else if (color == 11) draw_obj(body, bluetexture);

		if (assemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, 0, 6 - a);
		} 
		if (color == 0) draw_obj(arm1redleft, redtexture);
		else if (color == 10) draw_obj(arm1redleft, pinktexture);
		else if (color == 11) draw_obj(arm1redleft, bluetexture);
		if (assemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, 0, 6 - a);
		}
		if (repulsorbeam == 1) {
			glPushMatrix();
			glRotatef(-r, 1, 0, 0);
			glTranslatef(0, -0.6, 1.4);
		}
		if (color == 0) draw_obj(arm1redright, redtexture);
		else if (color == 10) draw_obj(arm1redright, pinktexture);
		else if (color == 11) draw_obj(arm1redright, bluetexture);
		if (repulsorbeam == 1) {
			glPopMatrix();
		}
		if (assemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(-6 + a, 0, -6 + a);
		}
		if (color == 0) draw_obj(handleft, redtexture);
		else if (color == 10) draw_obj(handleft, pinktexture);
		else if (color == 11) draw_obj(handleft, bluetexture);
		if (assemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(6 - a, 0, -6 + a);
		}
		if (repulsorbeam == 1) {
			glPushMatrix();
			glRotatef(80-r, 1, 0, 0);
			glTranslatef(0, 0, -0.6);
			if (color == 0) draw_obj(handright2, redtexture);
			else if (color == 10) draw_obj(handright2, pinktexture);
			else if (color == 11) draw_obj(handright2, bluetexture);
			glPopMatrix();
			glPushMatrix();
		}
		else {
			if (color == 0) draw_obj(handright, redtexture);
			else if (color == 10) draw_obj(handright, pinktexture);
			else if (color == 11) draw_obj(handright, bluetexture);
		}

		if (assemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(6 - a, 0, 6 - a);
		}
		if (color == 0) draw_obj(leg1red, redtexture);
		else if (color == 10) draw_obj(leg1red, pinktexture);
		else if (color == 11) draw_obj(leg1red, bluetexture);

		if (assemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(-6 + a, 0, 6 - a);
		}
		if (color == 0) draw_obj(leg2, redtexture);
		else if (color == 10) draw_obj(leg2, pinktexture);
		else if (color == 11) draw_obj(leg2, bluetexture);

		if (assemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(6 - a, 6 - a, 6 - a);
		}
		draw_obj(arc, whitetexture);

		if (assemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(6 - a, 0, -6 + a);
		}
		draw_obj(handarcleft, silvertexture);
		if (repulsorbeam == 1) {
			glPushMatrix();
			glRotatef(80 - r, 1, 0, 0);
			glTranslatef(0, 0, -0.6);
			draw_obj(handarcright2, whitetexture);
			glPopMatrix();
		} 
		else {
			draw_obj(handarcright, silvertexture);
		}
	
		if (assemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, 0, 6 - a);
		}
		draw_obj(footarc, silvertexture);
		draw_obj(sil, silvertexture);

		if (assemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(-6 + a, 6 - a, 0);
		}
		draw_obj(gun, silvertexture);
		if (assemble == 1) {
			glPopMatrix();
		}
	}
	else if (individual == 30) {
		draw_obj(face, goldtexture);
	}
	else if (individual == 31) {
		if (color == 0) draw_obj(helmet, redtexture);
		else if (color == 10) draw_obj(helmet, pinktexture);
		else if (color == 11) draw_obj(helmet, bluetexture);
	}
	else if (individual == 32) {
		if (color == 0) draw_obj(body, redtexture);
		else if (color == 10) draw_obj(body, pinktexture);
		else if (color == 11) draw_obj(body, bluetexture);
		draw_obj(arc, silvertexture);
		draw_obj(gun, silvertexture);
	}
	else if (individual == 33) {
		draw_obj(arm1gold, goldtexture);

		if (color == 0) draw_obj(arm1red, redtexture);
		else if (color == 10) draw_obj(arm1red, pinktexture);
		else if (color == 11) draw_obj(arm1red, bluetexture);
	}
	else if (individual == 34) {
		draw_obj(arm2, goldtexture);
	}
	else if (individual == 35) {
		if (color == 0) draw_obj(hand, redtexture);
		else if (color == 10) draw_obj(hand, pinktexture);
		else if (color == 11) draw_obj(hand, bluetexture);
		draw_obj(handarc, silvertexture);
	}
	else if (individual == 36) {
		draw_obj(leg1gold, goldtexture);
		if (color == 0) draw_obj(leg1red, redtexture);
		else if (color == 10) draw_obj(leg1red, pinktexture);
		else if (color == 11) draw_obj(leg1red, bluetexture);
	}
	else if (individual == 37) {
		if (color == 0) draw_obj(leg2, redtexture);
		else if (color == 10) draw_obj(leg2, pinktexture);
		else if (color == 11) draw_obj(leg2, bluetexture);
		draw_obj(footarc, silvertexture);
		draw_obj(sil, silvertexture);
	}
}

void draw_skyBox(GLuint texture) {
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	float g_nSkysize = 200;
	glBegin(GL_QUADS);
	//px
	glTexCoord3d(1, -1, -1); glVertex3f(g_nSkysize, -g_nSkysize, -g_nSkysize);
	glTexCoord3d(1, -1, 1); glVertex3f(g_nSkysize, -g_nSkysize, g_nSkysize);
	glTexCoord3d(1, 1, 1); glVertex3f(g_nSkysize, g_nSkysize, g_nSkysize);
	glTexCoord3d(1, 1, -1); glVertex3f(g_nSkysize, g_nSkysize, -g_nSkysize);
	//nx
	glTexCoord3d(-1, -1, -1); glVertex3f(-g_nSkysize, -g_nSkysize, -g_nSkysize);
	glTexCoord3d(-1, -1, 1); glVertex3f(-g_nSkysize, -g_nSkysize, g_nSkysize);
	glTexCoord3d(-1, 1, 1); glVertex3f(-g_nSkysize, g_nSkysize, g_nSkysize);
	glTexCoord3d(-1, 1, -1); glVertex3f(-g_nSkysize, g_nSkysize, -g_nSkysize);
	//py
	glTexCoord3d(1, 1, 1); glVertex3f(g_nSkysize, g_nSkysize, g_nSkysize);
	glTexCoord3d(-1, 1, 1); glVertex3f(-g_nSkysize, g_nSkysize, g_nSkysize);
	glTexCoord3d(-1, 1, -1); glVertex3f(-g_nSkysize, g_nSkysize, -g_nSkysize);
	glTexCoord3d(1, 1, -1); glVertex3f(g_nSkysize, g_nSkysize, -g_nSkysize);
	//ny
	glTexCoord3d(1, -1, 1); glVertex3f(g_nSkysize, -g_nSkysize, g_nSkysize);
	glTexCoord3d(-1, -1, 1); glVertex3f(-g_nSkysize, -g_nSkysize, g_nSkysize);
	glTexCoord3d(-1, -1, -1); glVertex3f(-g_nSkysize, -g_nSkysize, -g_nSkysize);
	glTexCoord3d(1, -1, -1); glVertex3f(g_nSkysize, -g_nSkysize, -g_nSkysize);
	//pz
	glTexCoord3d(1, -1, 1); glVertex3f(g_nSkysize, -g_nSkysize, g_nSkysize);
	glTexCoord3d(-1, -1, 1); glVertex3f(-g_nSkysize, -g_nSkysize, g_nSkysize);
	glTexCoord3d(-1, 1, 1); glVertex3f(-g_nSkysize, g_nSkysize, g_nSkysize);
	glTexCoord3d(1, 1, 1); glVertex3f(g_nSkysize, g_nSkysize, g_nSkysize);
	//nz
	glTexCoord3d(1, -1, -1); glVertex3f(g_nSkysize, -g_nSkysize, -g_nSkysize);
	glTexCoord3d(-1, -1, -1); glVertex3f(-g_nSkysize, -g_nSkysize, -g_nSkysize);
	glTexCoord3d(-1, 1, -1); glVertex3f(-g_nSkysize, g_nSkysize, -g_nSkysize);
	glTexCoord3d(1, 1, -1); glVertex3f(g_nSkysize, g_nSkysize, -g_nSkysize);
	glEnd();
}

void draw_axis() {
	glLineWidth(2); // 좌표축의 두께
	glBegin(GL_LINES);

	glColor3f(1, 0, 0); // x축은 red
	glVertex3f(0, 0, 0);
	glVertex3f(4, 0, 0);

	glColor3f(0, 1, 0); // y축은 green
	glVertex3f(0, 0, 0);
	glVertex3f(0, 4, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 4); // z축은 blue

	glEnd();
	glColor3f(1, 1, 1);
	glLineWidth(1); // 두께 다시 환원
}

void housepartyProtocol() {
	// bottom
	glPushMatrix();
	draw_ironman();
	glPopMatrix();
	
	// bottom left
	glPushMatrix();
	glRotatef(15.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(-5, 0, -20 + h[0]);
	draw_ironman();
	glPopMatrix();

	glPushMatrix();
	glRotatef(15.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(-10, 0, -20 + h[1]);
	draw_ironman();
	glPopMatrix();

	glPushMatrix();
	glRotatef(15.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(-15, 0, -20 + h[2]);
	draw_ironman();
	glPopMatrix();

	// bottom right
	glPushMatrix();
	glRotatef(-15.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(5, 0, -20 + h[0]);
	draw_ironman();
	glPopMatrix();

	glPushMatrix();
	glRotatef(-15.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(10, 0, -20 + h[1]);
	draw_ironman();
	glPopMatrix();

	glPushMatrix();
	glRotatef(-15.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(15, 0, -20 + h[2]);
	draw_ironman();
	glPopMatrix();

	// top
	glTranslatef(1.0f, 5.0f, -5);
	glPushMatrix();
	glTranslatef(0, 25 - h[3], 0);
	draw_ironman();
	glPopMatrix();

	// top left
	glPushMatrix();
	glRotatef(10.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(-5, 25 - h[3], -25 + h[3]);
	draw_ironman();
	glPopMatrix();

	glPushMatrix();
	glRotatef(10.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(-35 + h[4], 25 - h[4], -25 + h[4]);
	draw_ironman();	
	glPopMatrix();

	glPushMatrix();
	glRotatef(10.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(-40 + h[4], 25 - h[4], -25 + h[4]);
	draw_ironman();
	glPopMatrix();

	glPushMatrix();
	glRotatef(10.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(-45 + h[4], 0.0f, -25 + h[4]);
	draw_ironman();
	glPopMatrix();

	// top right
	glPushMatrix();
	glRotatef(-10.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(5, 25 - h[3], -25 + h[3]);
	draw_ironman();
	glPopMatrix();

	glPushMatrix();
	glRotatef(-10.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(35 - h[4], 25 - h[4], -25 + h[4]);
	draw_ironman();
	glPopMatrix();

	glPushMatrix();
	glRotatef(-10.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(40 - h[4], 25 - h[4], -25 + h[4]);
	draw_ironman();
	glPopMatrix();

	glPushMatrix();
	glRotatef(-10.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(45 - h[4], 0, -25 + h[4]);
	draw_ironman();
	glPopMatrix();
}

void smartGun() {
	glEnable(GL_TEXTURE_2D);
	if (score[0] != 0) { // cube
		glPushMatrix();
		glTranslatef(5, 0, 11);
		glColor3f(1, 1, 1);
		draw_textureCube();
		glPopMatrix();

		glDisable(GL_TEXTURE_2D);		
		glPushMatrix();
		glTranslatef(6.2, 2.1, 11);
		for (int i = 0; i < score[0]; i++) {
			glColor3f(1, 0, 0);
			glutSolidSphere(0.2, 30, 30);
			glTranslatef(-0.6, 0, 0);
		}
		glPopMatrix();
	}
	glEnable(GL_TEXTURE_2D);
	if (score[1] != 0) { // sphere
		glPushMatrix();
		glTranslatef(0, 0, 21);
		glColor3f(1, 1, 1);
		draw_Sphere();
		glPopMatrix();

		glDisable(GL_TEXTURE_2D);
		glPushMatrix();
		glTranslatef(1.2, 3.8, 13);
		for (int i = 0; i < score[1]; i++) {
			glColor3f(1, 0, 0);
			glutSolidSphere(0.2, 30, 30);
			glTranslatef(-0.6, 0, 0);
		}
		glPopMatrix();
	}
	glEnable(GL_TEXTURE_2D);
	if (score[2] != 0) { // cylinder
		glPushMatrix();
		glTranslatef(-5, 0, 15);
		glColor3f(1, 1, 1);
		draw_Cylinder();
		glPopMatrix();

		glDisable(GL_TEXTURE_2D);
		glPushMatrix();
		glTranslatef(-5, 2.6, 13);
		for (int i = 0; i < score[2]; i++) {
			glColor3f(1, 0, 0);
			glutSolidSphere(0.2, 30, 30);
			glTranslatef(-0.6, 0, 0);
		}
		glPopMatrix();
	}
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
}

void draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	cam[0] = radius * sin(theta * M_PI / 180) * sin(phi * M_PI / 180);
	cam[1] = radius * cos(theta * M_PI / 180);
	cam[2] = radius * sin(theta * M_PI / 180) * cos(phi * M_PI / 180);
	gluLookAt(cam[0], cam[1], cam[2], center[0], center[1], center[2], up[0], up[1], up[2]);

	if (smartgun == 1) {
		smartGun();
	}

	if (background == 0) {
		draw_skyBox(universeCubeTex);
	}
	else if (background == 1) {
		draw_skyBox(hallCubeTex);
	} 
	else if (background == 2) {
		draw_skyBox(oceanCubeTex);
	}

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_CUBE_MAP);

	if (houseparty == 1 && smartgun == 0) {
		housepartyProtocol();
	}
	else {
		//draw_axis();
		draw_ironman();
	}

	if (smartgun == 1) {
		smartGun();
	}

	//GetCursorPos(&ptMouse);
	//printf("[윈도우 기준] X: %04d, Y: %04d", ptMouse.x, ptMouse.y);
	
	glFlush();
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
	if (key == 'R' || key == 'r') {
		printf("Repulsor Beam mode has been selected\n");
		if (repulsorbeam == 0) {
			smartgun = 0;
			repulsorbeam = 1;
			PlaySound("sound/repulsor.wav", 0, SND_FILENAME | SND_ASYNC);
		}
		else {
			repulsorbeam = 0;
			r = 0;
		}
	}
	else if (key == 'S' || key == 's') {
		printf("Smart Gun mode has been selected\n");
		if (smartgun == 0) {
			repulsorbeam = 0; houseparty = 0;
			smartgun = 1;
			radius = 8;
			theta = 50;
			phi = 180;
			center[1] = 4;
		}
		else {
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			smartgun = 0;
			radius = 11;
			theta = 80;
			phi = 1;
			center[1] = 0;
		}
	}
	else if (key == 'H' || key == 'h') {
		printf("House Party Protocol has been selected\n");
		if (houseparty == 0) {
			houseparty = 1;
		}
		else {
			houseparty = 0;
			h[0] = 0; h[1] = 0; h[2] = 0; h[3] = 0; h[4] = 0;
		}
	}

	glutPostRedisplay();
}

void specialkeyboard(int key, int x, int y) {
	if (smartgun == 0) {
		if (key == GLUT_KEY_LEFT) {
			phi -= 2.5;
			if (phi < 0) phi = 355;
		}
		else if (key == GLUT_KEY_RIGHT) {
			phi += 2.5;
			if (phi >= 360) phi = 0;
		}
		else if (key == GLUT_KEY_UP) {
			if (theta > 10) theta -= 2.5;
		}
		else if (key == GLUT_KEY_DOWN) {
			if (theta < 170) theta += 2.5;
		}
		glutPostRedisplay();
	}
}

void mouse(int button, int state, int x, int y) {
	if (smartgun == 1) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			printf("x: %d, y: %d\n", x, y);
			if (x > 98 && x < 156 && y > 270 && y < 328 && score[0] > 0) {
				score[0]--; // cube
			}
			if (x > 222 && x < 275 && y > 222 && y < 275 && score[1] > 0) {
				score[1]--; // sphere
			}
			if (x > 368 && x < 410 && y > 255 && y < 315 && score[2] > 0) {
				score[2]--; // cylinder
			}
			PlaySound("sound/gun.wav", 0, SND_FILENAME | SND_ASYNC);
			
			/*
			glPushMatrix();
			glColor3f(1, 0, 0);
			glTranslatef(x, y, 0);
			glutSolidSphere(15, 30, 30);
			glColor3f(1, 1, 1);
			glPopMatrix();
			*/
		}
		glutPostRedisplay();
	}
}

void mouseWheel(int button, int dir, int x, int y) {
	if (smartgun == 0) {
		if (dir > 0) {
			if (radius > 2) radius--;
		}
		else {
			if (radius < 100) radius++;
		}
		glutPostRedisplay();
	}
}

void main_menu(int option) {
	if (option == 99) exit(0);
	else if (option == 1) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		radius = 11; theta = 80; phi = 1; center[1] = 0;
		assemble = 0;
		a = 0;
		individual = 0;
		color = 0; 
		background = 0;
		houseparty = 0;
		h[0] = 0; h[1] = 0; h[2] = 0; h[3] = 0; h[4] = 0;
		smartgun = 0;
		repulsorbeam = 0; r = 0;
		score[0] = 5; score[1] = 5; score[2] = 5;
		printf("Init has been selected\n");
	}
	else if (option == 2) {
		if (assemble == 0) {
			assemble = 1;
			PlaySound("sound/move.wav", 0, SND_FILENAME | SND_ASYNC);
		}
		else {
			assemble = 0;
			a = 0;
		}
		printf("Assemble has been selected\n");
	}
	glutPostRedisplay();
}

void sub_menu1(int option) {
	if (option == 0) {
		color = 0;
		printf("Red has been selected\n");
	}
	if (option == 10) {
		color = 10;
		printf("Pink has been selected\n");
	}
	else if (option == 11) {
		color = 11;
		printf("Blue has been selected\n");
	}
	glutPostRedisplay();
}

void sub_menu2(int option) {
	if (option == 20) {
		background = 0;
		printf("Universe has been selected\n");
	}
	if (option == 21) {
		background = 1;
		printf("Hall of armor has been selected\n");
	}
	else if (option == 22) {
		background = 2;
		printf("Ocean has been selected\n");
	} 
	glutPostRedisplay();
}

void sub_menu3(int option) {
	if (option == 30) {
		individual = 30;
		printf("Face has been selected\n");
	}
	else if (option == 31) {
		individual = 31;
		printf("Helmet has been selected\n");
	}
	else if (option == 32) {
		individual = 32;
		printf("Body has been selected\n");
	}
	else if (option == 33) {
		individual = 33;
		printf("Upper Arm has been selected\n");
	}
	else if (option == 34) {
		individual = 34;
		printf("Under Arm has been selected\n");
	}
	else if (option == 35) {
		individual = 35;
		printf("Hand has been selected\n");
	}
	else if (option == 36) {
		individual = 36;
		printf("Upper Leg has been selected\n");
	}
	else if (option == 37) {
		individual = 37;
		printf("Under Leg has been selected\n");
	}
	glutPostRedisplay();
}

void printInstruction() {
	/* 조작법 console 출력 */
	printf("\n-----------Keyboard Navigation-----------\n");
	printf("R/r : Repulsor Beam mode\n");
	printf("S/s : Smart Gun mode\n");
	printf("H/h : House Party Protocol\n");	
	printf("방향키 : camera 위치\n");

	printf("\n-----------Mouse Navigation-----------\n");
	printf("Left Button : Shoot in Smart Gun mode\n");
	printf("Right Button : Menu\n");
	printf("Mouse Wheel : Zoom in&out\n");

	printf("\n-----------Menu Navigation-----------\n");
	printf("Init\n");
	printf("Color(red, pink, blue)\n");
	printf("Background(hall of armor, ocean, universe\n");
	printf("Assemble\n");
	printf("Individual\n");
	printf("Exit\n\n");
}