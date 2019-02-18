/*
 *--------------------------------------------------------
 * Sample code : solar_system.c
 *
 * by Nobuaki Ohno, 2005
 *
 *--------------------------------------------------------
*/

#include <cave_ogl.h>
#include <GL/GL.h>
#include <GL/glu.h>
#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#include <strings.h>
#else
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#endif
#include <math.h>

#define PI 3.14159
#define NUM_PLANET 10
#define YPOS 5.0
#define ORBIT_MAG 1.5
#define PLANET_MAG 0.15

/* The data that will be shared between processes */
struct _planetdata {
	float xpos;
	float ypos;
	float zpos;
};

void init_gl(void), draw(struct _planetdata *);
struct _planetdata *init_shmem(void);
void compute(struct _planetdata *);

GLUquadricObj *sphereOBJ;
GLUquadricObj *diskOBJ;

GLuint orbits;
GLuint planet_obj[NUM_PLANET];
/*
 0: Mercury, 1: Venus, 2: Earth, 3: Mars
 4: Jupitar, 5: Satrun, 6: Uranus, 7: Neptune, 8: Pluto
*/

GLfloat planet_color[NUM_PLANET][3] = {
	{ 1.0, 0.89, 0.76 },{ 1.0, 0.49, 0.31 },{ 0.0, 0.0, 1.0 },
	{ 0.82, 0.41, 0.12 },{ 0.96, 0.96, 0.86 },{ 1.0, 0.92, 0.76 },
	{ 0.69, 0.77, 0.05 },{ 0.0, 0.0, 1.0 },{ 0.5, 0.5, 0.05 }
};

GLfloat planet_rad[NUM_PLANET] =
	{ 0.38, 0.95, 1.0, 0.53, 11.2, 9.5, 4.0, 3.9, 0.18 };
float rev_rad[NUM_PLANET] =
	{ 0.39, 0.72, 1.0, 1.5, 5.2, 9.6, 19.1, 30.2, 39.5 };
float period[NUM_PLANET] =
	{ 0.24, 0.63, 1.0, 1.9, 11.9, 29.5, 84.0, 165.0, 247.0 };

int
main(int argc, char **argv)
{
	struct _planetdata *planet;
	CAVEConfigure(&argc, argv, NULL);

	planet = init_shmem();

	CAVEInit();
	CAVEInitApplication(init_gl, 0);
	CAVEDisplay((CAVECALLBACK)draw, 1, planet);

	while (!CAVEgetbutton(CAVE_ESCKEY))
	{
		compute(planet);
#ifndef _WIN32
		sginap(1);
#else
		CAVEUSleep(10);
#endif
	}
	CAVEExit();

	return 0;
}

struct _planetdata *init_shmem(void)
{
	int i;
	struct _planetdata *planet;

	planet = (_planetdata *)CAVEMalloc(NUM_PLANET * sizeof(struct _planetdata));
	bzero(planet, NUM_PLANET * sizeof(struct _planetdata));

	for (i = 0; i < NUM_PLANET; i++)
	{
		planet[i].xpos = ORBIT_MAG * rev_rad[i];
		planet[i].ypos = YPOS;
		planet[i].zpos = 0.0;
	}

	return planet;
}

void
compute(struct _planetdata *planet)
{
	int i;
	float angle;
	float t = CAVEGetTime();

	for (i = 0; i < NUM_PLANET; i++)
	{
		angle = (int)(t * 50.0 / period[i]) % 360;
		planet[i].xpos = ORBIT_MAG * rev_rad[i] * cos(2.0 * PI * angle / 360.0);
		planet[i].zpos = ORBIT_MAG * rev_rad[i] * sin(2.0 * PI * angle / 360.0);
	}
}

void
init_gl(void)
{
	int i, j;
	GLfloat diffuse[4];
	GLfloat ambient[4];
	GLfloat specular[4] = { 1.0, 1.0, 1.0, 1.0 };

	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 0.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat light_position[] = { 0.0, 1.0, 1.0, 0.0 };

	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glColor3f(1.0, 1.0, 1.0);
	glClearColor(0., 0., 0., 0.);

	sphereOBJ = gluNewQuadric();
	diskOBJ = gluNewQuadric();

	ambient[3] = 1.0;
	diffuse[3] = 1.0;

	for (i = 0; i < NUM_PLANET; i++)
	{
		ambient[0] = planet_color[i][0];
		ambient[1] = planet_color[i][1];
		ambient[2] = planet_color[i][2];

		diffuse[0] = 0.75 * planet_color[i][0];
		diffuse[1] = 0.75 * planet_color[i][1];
		diffuse[2] = 0.75 * planet_color[i][2];

		planet_obj[i] = glGenLists(1);
		glNewList(planet_obj[i], GL_COMPILE);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0);

		gluSphere(sphereOBJ, planet_rad[i] * PLANET_MAG, 32, 32);

		if (i == 5)
		{ /* For Saturnfs ring */
			glPushMatrix();
			glRotatef(75.0, 1.0, 0.0, 0.0);
			gluDisk(diskOBJ, 1.2 * PLANET_MAG * planet_rad[i],
				2.25 * PLANET_MAG * planet_rad[i], 32, 32);
			glPopMatrix();
		}
		glEndList();
	}

	orbits = glGenLists(1);
	glNewList(orbits, GL_COMPILE);
	for (i = 0; i < NUM_PLANET; i++)
	{
		glBegin(GL_LINE_STRIP);
		for (j = 0; j < 720; j++)
		{
			glVertex3f(
				ORBIT_MAG * rev_rad[i] * cos(2.0 * PI * j / 360.0),
				YPOS,
				ORBIT_MAG * rev_rad[i] * sin(2.0 * PI * j / 360.0));
		}
		glEnd();
	}
	glEndList();
}

void
draw(struct _planetdata *planet)
{
	int i;

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glCallList(orbits);

	glEnable(GL_LIGHTING);

	for (i = 0; i < NUM_PLANET; i++)
	{
		glPushMatrix();
		glTranslatef(planet[i].xpos, planet[i].ypos, planet[i].zpos);

		glCallList(planet_obj[i]);

		glPopMatrix();
	}

	glDisable(GL_LIGHTING);
}
