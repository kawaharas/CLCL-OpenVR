/*
 *--------------------------------------------------------
 * Sample code : sword.c
 *
 * by Nobuaki Ohno, 2005
 *
 *--------------------------------------------------------
*/

#include <cave_ogl.h>
#include <GL/GL.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#define LENGTH 3.0

/* The data that will be shared between processes */
struct _sworddata
{
	float wand[3];
	float tip[3];
	int on;
};

void init_gl(void), draw(struct _sworddata *);
struct _sworddata *init_shmem(void);
void compute(struct _sworddata *);

int
main(int argc, char **argv)
{
	struct _sworddata *sword;
	CAVEConfigure(&argc, argv, NULL);

	sword = init_shmem();

	CAVEInit();
	CAVEInitApplication(init_gl, 0);
	CAVEDisplay((CAVECALLBACK)draw, 1, sword);

	while (!CAVEgetbutton(CAVE_ESCKEY))
	{
		compute(sword);
#ifndef _WIN32
		sginap(1);
#else
		CAVEUSleep(10);
#endif
	}

	CAVEExit();

	return 0;
}

struct _sworddata *
	init_shmem(void)
{
	int i;
	struct _sworddata *sword;

	sword = (_sworddata *)CAVEMalloc(sizeof(struct _sworddata));

	for (i = 0; i < 3; i++)
	{
		sword->wand[i] = 0.0;
		sword->tip[i] = 0.0;
	}

	sword->on = 0;

	return sword;
}

void
compute(struct _sworddata *sword)
{
	int i;
	float w[3], d[3], t[3];

	if (CAVEBUTTON1)
	{
		CAVEGetPosition(CAVE_WAND, w);
		CAVEGetVector(CAVE_WAND_FRONT, d);

		for (i = 0; i < 3; i++)
		{
			t[i] = LENGTH * d[i] + w[i];
			sword->wand[i] = w[i];
			sword->tip[i] = t[i];
		}
		sword->on = 1;
	}
	else
	{
		sword->on = 0;
	}
}

void
init_gl(void)
{
	glDisable(GL_LIGHTING);
	glClearColor(0., 0., 0., 0.);
}

void
draw(struct _sworddata *sword)
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	if (sword->on)
	{
		glLineWidth(12.0);
		glBegin(GL_LINES);
		glColor3f(1.0, 1.0, 1.0);
		glVertex3fv(sword->wand);
		glColor3f(0.7, 1.0, 0.5);
		glVertex3fv(sword->tip);
		glEnd();
	}
}
