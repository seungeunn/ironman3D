#include "ObjParser.h"
#include "bmpfuncs.h"
#include <GL/glext.h>

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500
#define M_PI 3.1415926535897
using std::cos;
using std::sin;
using std::sqrt;

int disassemble = 0;
int individual = 0;
int color = 0;
int houseparty = 0;
int smartgun = 0;
int repulsorbeam = 0;
int unibeam = 0;
int score[3] = { 5,5,5 };

double radius = 11;
double theta = 80, phi = 1;
double cam[3];
double center[3] = { 0, 0, 0 };
double up[3] = { 0, cos(phi * M_PI / 180), 0 };

GLuint g_nCubeTex;
GLUquadric* qobj = gluNewQuadric();

ObjParser* face = new ObjParser("obj/face.obj");
ObjParser* helmet = new ObjParser("obj/helmet.obj");
ObjParser* body = new ObjParser("obj/body.obj");
ObjParser* arc = new ObjParser("obj/arc.obj");
ObjParser* arm1gold = new ObjParser("obj/arm1gold.obj");
ObjParser* arm1red = new ObjParser("obj/arm1red.obj");
ObjParser* arm2 = new ObjParser("obj/arm2.obj");
ObjParser* gun = new ObjParser("obj/gun.obj");
ObjParser* hand = new ObjParser("obj/hand.obj");
ObjParser* handarc = new ObjParser("obj/handarc.obj");
ObjParser* leg1gold = new ObjParser("obj/leg1gold.obj");
ObjParser* leg1red = new ObjParser("obj/leg1red.obj");
ObjParser* leg2 = new ObjParser("obj/leg2.obj");
ObjParser* sil = new ObjParser("obj/silver.obj");
ObjParser* footarc = new ObjParser("obj/footarc.obj");

GLuint redtexture;
GLuint goldtexture;
GLuint silvertexture;
GLuint blacktexture;
GLuint pinktexture;
GLuint bluetexture;

void draw_ironman();
void setEnvironmentMap();
void draw_skyBox();
void housepartyProtocol();
void smartGun();
void Picking();

void setTextureMapping();
void draw_obj(ObjParser*, GLuint);
void init();
void light_default();
void resize(int, int);
void draw();
void draw_axis();
void keyboard(unsigned char, int, int);
void specialkeyboard(int, int, int);
void mouseWheel(int, int, int, int);
void mouse(int, int, int, int);
void sub_menu1(int);
void sub_menu2(int);
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
	glutAddMenuEntry("face", 20);
	glutAddMenuEntry("helmet", 21);
	glutAddMenuEntry("body", 22);
	glutAddMenuEntry("upper arm", 23);
	glutAddMenuEntry("under arm", 24);
	glutAddMenuEntry("hand", 25);
	glutAddMenuEntry("upper leg", 26);
	glutAddMenuEntry("under leg", 27);

	int mainmenu = glutCreateMenu(main_menu);
	glutAddMenuEntry("Init", 1);
	glutAddSubMenu("Color", submenu1);
	glutAddMenuEntry("Disassemble", 2);
	glutAddSubMenu("Individual", submenu2);
	glutAddMenuEntry("Quit", 99);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// callback function
	glutMouseWheelFunc(&mouseWheel);
	glutMouseFunc(&mouse);
	glutKeyboardFunc(&keyboard);
	glutSpecialFunc(&specialkeyboard);
	glutDisplayFunc(&draw);
	glutReshapeFunc(&resize);

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

	/************* spot position setting ************/
	GLfloat spot_direction[] = { -10.0, 0.0, 0.0, 1.0 };
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45.0);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 20.0);

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
	//glEnable(GL_LIGHT1);

	// depth test enable
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);	
}

void init() {
	// clear background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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

void setTextureMapping() {
	int imgWidth, imgHeight, channels;
	uchar* img = readImageData("img/gold.bmp", &imgWidth, &imgHeight, &channels);
	glGenTextures(1, &goldtexture);
	glBindTexture(GL_TEXTURE_2D, goldtexture);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); //GL_REPLACE, GL_MODULATE
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

	img = readImageData("img/black.bmp", &imgWidth, &imgHeight, &channels);
	glGenTextures(1, &blacktexture);
	glBindTexture(GL_TEXTURE_2D, blacktexture);
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
	
	uchar* img0 = readImageData("img/1024px.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img1 = readImageData("img/1024nx.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img2 = readImageData("img/1024py.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img3 = readImageData("img/1024ny.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img4 = readImageData("img/1024pz.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img5 = readImageData("img/1024nz.bmp", &imgWidth, &imgHeight, &channels);
	/*
	uchar* img0 = readImageData("img/black.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img1 = readImageData("img/black.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img2 = readImageData("img/black.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img3 = readImageData("img/black.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img4 = readImageData("img/black.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img5 = readImageData("img/hallofarmor2.bmp", &imgWidth, &imgHeight, &channels);
	
	uchar* img0 = readImageData("img/128px.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img1 = readImageData("img/128nx.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img2 = readImageData("img/128py.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img3 = readImageData("img/128ny.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img4 = readImageData("img/128pz.bmp", &imgWidth, &imgHeight, &channels);
	uchar* img5 = readImageData("img/128nz.bmp", &imgWidth, &imgHeight, &channels);
	*/

	glGenTextures(1, &g_nCubeTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img1);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img2);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img3);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img4);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img5);

	glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);
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

void draw_obj(ObjParser* objParser, GLuint texture) {
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
}

void draw_ironman() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	if (individual == 0) {
		if (disassemble == 1) {
			glPushMatrix();
			glTranslatef(0, 1, 1);
		}
		draw_obj(face, goldtexture);
		if (disassemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, 0, 1);
		}
		draw_obj(arm1gold, goldtexture);
		if (disassemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, 0, 1.5);
		}
		draw_obj(arm2, goldtexture);
		if (disassemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, -1, 1);
		}
		draw_obj(leg1gold, goldtexture);

		if (disassemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, 1, 0);
		}

		if (color == 0) draw_obj(helmet, redtexture);
		else if(color == 10) draw_obj(helmet, pinktexture);
		else if(color == 11) draw_obj(helmet, bluetexture);

		if (disassemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, 0, -1);
		}

		if (color == 0) draw_obj(body, redtexture);
		else if (color == 10) draw_obj(body, pinktexture);
		else if (color == 11) draw_obj(body, bluetexture);

		if (disassemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, 0.5, 0.5);
		}

		if (color == 0) draw_obj(arm1red, redtexture);
		else if (color == 10) draw_obj(arm1red, pinktexture);
		else if (color == 11) draw_obj(arm1red, bluetexture);

		if (disassemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, 0, 2);
		}

		if (color == 0) draw_obj(hand, redtexture);
		else if (color == 10) draw_obj(hand, pinktexture);
		else if (color == 11) draw_obj(hand, bluetexture);

		if (disassemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, 0, 0.5);
		}

		if (color == 0) draw_obj(leg1red, redtexture);
		else if (color == 10) draw_obj(leg1red, pinktexture);
		else if (color == 11) draw_obj(leg1red, bluetexture);

		if (disassemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, -1, 1.5);
		}

		if (color == 0) draw_obj(leg2, redtexture);
		else if (color == 10) draw_obj(leg2, pinktexture);
		else if (color == 11) draw_obj(leg2, bluetexture);

		if (disassemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, 0, 0.5);
		}
		draw_obj(arc, silvertexture);
		if (disassemble == 1) {
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, 0, 2);
		}
		draw_obj(handarc, silvertexture);
		if (disassemble == 1) {
			glPopMatrix();
		}
		draw_obj(footarc, silvertexture);
		draw_obj(sil, silvertexture);
		if (disassemble == 1) {
			glPushMatrix();
			glTranslatef(0, 0, 1);
		}
		draw_obj(gun, blacktexture);
		if (disassemble == 1) {
			glPopMatrix();
		}
	}
	else if (individual == 20) {
		draw_obj(face, goldtexture);
	}
	else if (individual == 21) {
		if (color == 0) draw_obj(helmet, redtexture);
		else if (color == 10) draw_obj(helmet, pinktexture);
		else if (color == 11) draw_obj(helmet, bluetexture);
	}
	else if (individual == 22) {
		if (color == 0) draw_obj(body, redtexture);
		else if (color == 10) draw_obj(body, pinktexture);
		else if (color == 11) draw_obj(body, bluetexture);
		draw_obj(arc, silvertexture);
		draw_obj(gun, blacktexture);
	}
	else if (individual == 23) {
		draw_obj(arm1gold, goldtexture);

		if (color == 0) draw_obj(arm1red, redtexture);
		else if (color == 10) draw_obj(arm1red, pinktexture);
		else if (color == 11) draw_obj(arm1red, bluetexture);
	}
	else if (individual == 24) {
		draw_obj(arm2, goldtexture);
	}
	else if (individual == 25) {
		if (color == 0) draw_obj(hand, redtexture);
		else if (color == 10) draw_obj(hand, pinktexture);
		else if (color == 11) draw_obj(hand, bluetexture);
		draw_obj(handarc, silvertexture);
	}
	else if (individual == 26) {
		draw_obj(leg1gold, goldtexture);
		if (color == 0) draw_obj(leg1red, redtexture);
		else if (color == 10) draw_obj(leg1red, pinktexture);
		else if (color == 11) draw_obj(leg1red, bluetexture);
	}
	else if (individual == 27) {
		if (color == 0) draw_obj(leg2, redtexture);
		else if (color == 10) draw_obj(leg2, pinktexture);
		else if (color == 11) draw_obj(leg2, bluetexture);
		draw_obj(footarc, silvertexture);
		draw_obj(sil, silvertexture);
	}
}

void draw_skyBox() {
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);
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
	glPushMatrix();
		draw_ironman();
		glRotatef(15.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(-5.0f, 0.0f, 0.0f);
		draw_ironman();
		glRotatef(15.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(-5.0f, 0.0f, 0.0f);
		draw_ironman();
		glRotatef(15.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(-5.0f, 0.0f, 0.0f);
		draw_ironman();
		glRotatef(15.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(-5.0f, 0.0f, 0.0f); 
		draw_ironman();
	glPopMatrix();
	glPushMatrix();
		glRotatef(-15.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(5.0f, 0.0f, 0.0f);
		draw_ironman();
		glRotatef(-15.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(5.0f, 0.0f, 0.0f);
		draw_ironman();
		glRotatef(-15.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(5.0f, 0.0f, 0.0f);
		draw_ironman();
		glRotatef(-15.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(5.0f, 0.0f, 0.0f);
		draw_ironman();
	glPopMatrix();
	glTranslatef(1.0f, 5.0f, -5.0f);
	glPushMatrix();
		draw_ironman();
		glRotatef(10.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(-5.0f, 0.0f, 0.0f);
		draw_ironman();
		glRotatef(10.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(-5.0f, 0.0f, 0.0f);
		draw_ironman();
		glRotatef(15.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(-5.0f, 0.0f, 0.0f);
		draw_ironman();
		glRotatef(10.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(-5.0f, 0.0f, 0.0f);
		draw_ironman();
		glRotatef(10.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(-5.0f, 0.0f, 0.0f);
		draw_ironman();
	glPopMatrix();
	glPushMatrix();
		glRotatef(-10.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(5.0f, 0.0f, 0.0f);
		draw_ironman();
		glRotatef(-10.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(5.0f, 0.0f, 0.0f);
		draw_ironman();
		glRotatef(-10.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(5.0f, 0.0f, 0.0f);
		draw_ironman();
		glRotatef(-10.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(5.0f, 0.0f, 0.0f);
		draw_ironman();
		glRotatef(-10.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(5.0f, 0.0f, 0.0f);
		draw_ironman();
	glPopMatrix();
}

void smartGun() {
	glDisable(GL_TEXTURE_2D);
	if (score[0] != 0) {
		glPushMatrix();
			glColor3f(1, 0, 0);
			glPushMatrix();
			glTranslatef(6.2, 2.5, 11);
			for (int i = 0; i < score[0]; i++) {
				glutSolidSphere(0.2, 30, 30);
				glTranslatef(-0.6, 0, 0);
			}
			glPopMatrix();
		glTranslatef(5, 0, 11);
		glColor3f(1, 1, 0); // yellow
		glutSolidDodecahedron();
		glPopMatrix();
	}
	if (score[1] != 0) {
		glPushMatrix();
			glColor3f(1, 0, 0);
			glPushMatrix();
			glTranslatef(-5, 2.6, 13);
			for (int i = 0; i < score[1]; i++) {
				glutSolidSphere(0.2, 30, 30);
				glTranslatef(-0.6, 0, 0);
			}
			glPopMatrix();
		glTranslatef(-7, 0, 15);
		glColor3f(0, 1, 0); // green
		glutSolidTeapot(2);
		glPopMatrix();
	}
	if (score[2] != 0) {
		glPushMatrix();
			glColor3f(1, 0, 0);
			glPushMatrix();
			glTranslatef(1.2, 3.6, 13);
			for (int i = 0; i < score[2]; i++) {
				glutSolidSphere(0.2, 30, 30);
				glTranslatef(-0.6, 0, 0);
			}
			glPopMatrix();
		glTranslatef(0, 0, 21);
		glColor3f(0, 0, 1); // blue
		glutSolidSphere(2, 30, 30);
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

	draw_skyBox();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_CUBE_MAP);

	if (smartgun == 1) {
		smartGun();
	}

	if (houseparty == 1 && smartgun == 0) {
		housepartyProtocol();
	}
	else {
		//draw_axis();
		draw_ironman();
	}

	glFlush();
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
	if (key == 'R' || key == 'r') {
		if (smartgun == 0) {

		}
		printf("Repulsor Beam mode has been selected\n");

	}
	else if (key == 'U' || key == 'u') {
		if (smartgun == 0) {
			printf("UniBeam Blast mode has been selected\n");
			if (unibeam == 0)
				unibeam = 1;
			else unibeam = 0;
		}
	}
	else if (key == 'S' || key == 's') {
		printf("Smart Gun mode has been selected\n");
		if (smartgun == 0) {
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
		if (smartgun == 0) {
			printf("House Party Protocol has been selected\n");
			if (houseparty == 0)
				houseparty = 1;
			else houseparty = 0;
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
			//printf("x: %d, y: %d\n", x, y);
			if (x > 85 && x < 166 && y > 263 && y < 333 && score[0] > 0) {
				score[0]--; // yellow
			}
			if (x > 330 && x < 441 && y > 248 && y < 305 && score[1] > 0) {
				score[1]--; // green
			}
			if (x > 220 && x < 277 && y > 223 && y < 277 && score[2] > 0) {
				score[2]--; // blue
			}
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
		radius = 11;
		theta = 80;
		phi = 1;
		center[1] = 0;
		disassemble = 0;
		individual = 0;
		color = 0;
		houseparty = 0;
		smartgun = 0;
		repulsorbeam = 0;
		unibeam = 0;
		score[0] = 5;
		score[1] = 5;
		score[2] = 5;
		printf("Init has been selected\n");
	}
	else if (option == 2) {
		if (disassemble == 0) {
			disassemble = 1;
			printf("disassemble has been selected\n");
		}
		else {
			disassemble = 0;
			printf("disassemble has been selected\n");
		}
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
		individual = 20;
		printf("Face has been selected\n");
	}
	else if (option == 21) {
		individual = 21;
		printf("Helmet has been selected\n");
	}
	else if (option == 22) {
		individual = 22;
		printf("Body has been selected\n");
	}
	else if (option == 23) {
		individual = 23;
		printf("Upper Arm has been selected\n");
	}
	else if (option == 24) {
		individual = 24;
		printf("Under Arm has been selected\n");
	}
	else if (option == 25) {
		individual = 25;
		printf("Hand has been selected\n");
	}
	else if (option == 26) {
		individual = 26;
		printf("Upper Leg has been selected\n");
	}
	else if (option == 27) {
		individual = 27;
		printf("Under Leg has been selected\n");
	}
	glutPostRedisplay();
}

void printInstruction() {
	/* 조작법 console 출력 */
	printf("\n-----------Keyboard Navigation-----------\n");
	printf("R/r : Repulsor Beam mode\n");
	printf("U/u : UniBeam Blast mode\n");
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
	printf("Disassenble\n");
	printf("Individual\n");
	printf("Exit\n\n");
}