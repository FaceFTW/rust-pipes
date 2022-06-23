/**
 * @file utils.cpp
 * @author Alex "FaceFTW" Westerman
 * @brief
 * @version 0.1
 * @date 2022-03-17
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */
#include <GL/gl.h>
#include <math.h>

#include "include/utils.h"

int iRand(int max) {
	return rand() % (max + 1);
}

int iRand2(int min, int max) {
	if(min == max)
		return min;
	else if(max < min) {
		int temp = min;
		min = max;
		max = temp;
	}

	return min + (rand() % (max - min + 1));
}

float fRand(float min, float max) {
	float diff;

	diff = max - min;
	return min + (diff * (((float) rand()) / ((float) (RAND_MAX))));
}

void transformCircle(
        float angle,
        POINT3D* inPoint,
        POINT3D* outPoint,
        GLint num,
        POINT3D* anchor) {
	MATRIX matrix1, matrix2, matrix3;
	int i;

	// translate anchor point to origin
	matrixIdent(&matrix1);
	matrixTranslate(&matrix1, -anchor->x, -anchor->y, -anchor->z);

	// rotate by angle, cw around x-axis
	matrixIdent(&matrix2);
	matrixRotate(&matrix2, (double) -angle, 0.0, 0.0);

	// concat these 2
	matrixMult(&matrix3, &matrix2, &matrix1);

	// translate back
	matrixIdent(&matrix2);
	matrixTranslate(&matrix2, anchor->x, anchor->y, anchor->z);

	// concat these 2
	matrixMult(&matrix1, &matrix2, &matrix3);

	// transform all the points, + center
	for(i = 0; i < num; i++, outPoint++, inPoint++) {
		xformPoint(outPoint, inPoint, &matrix1);
	}
}
void calcNormals(POINT3D* p, POINT3D* n, POINT3D* center,
                 int num) {
	int i;

	for(i = 0; i < num; i++, n++, p++) {
		n->x = p->x - center->x;
		n->y = p->y - center->y;
		n->z = p->z - center->z;
		normalizeNorm(n);
	}
}

POINT3D ptZero = {0.0f, 0.0f, 0.0f};

void xformPoint(POINT3D* ptOut, POINT3D* ptIn, MATRIX* mat) {
	double x, y, z;

	x = (ptIn->x * mat->M[0][0]) + (ptIn->y * mat->M[0][1]) +
	        (ptIn->z * mat->M[0][2]) + mat->M[0][3];

	y = (ptIn->x * mat->M[1][0]) + (ptIn->y * mat->M[1][1]) +
	        (ptIn->z * mat->M[1][2]) + mat->M[1][3];

	z = (ptIn->x * mat->M[2][0]) + (ptIn->y * mat->M[2][1]) +
	        (ptIn->z * mat->M[2][2]) + mat->M[2][3];

	ptOut->x = (float) x;
	ptOut->y = (float) y;
	ptOut->z = (float) z;
}

void xformNorm(POINT3D* ptOut, POINT3D* ptIn, MATRIX* mat) {
	double x, y, z;
	double len;

	x = (ptIn->x * mat->M[0][0]) + (ptIn->y * mat->M[0][1]) +
	        (ptIn->z * mat->M[0][2]);

	y = (ptIn->x * mat->M[1][0]) + (ptIn->y * mat->M[1][1]) +
	        (ptIn->z * mat->M[1][2]);

	z = (ptIn->x * mat->M[2][0]) + (ptIn->y * mat->M[2][1]) +
	        (ptIn->z * mat->M[2][2]);

	len = (x * x) + (y * y) + (z * z);
	if(len >= ZERO_EPS)
		len = 1.0 / sqrt(len);
	else
		len = 1.0;

	ptOut->x = (float) (x * len);
	ptOut->y = (float) (y * len);
	ptOut->z = (float) (z * len);
	return;
}

void matrixIdent(MATRIX* mat) {
	mat->M[0][0] = 1.0f;
	mat->M[0][1] = 0.0f;
	mat->M[0][2] = 0.0f;
	mat->M[0][3] = 0.0f;

	mat->M[1][0] = 0.0f;
	mat->M[1][1] = 1.0f;
	mat->M[1][2] = 0.0f;
	mat->M[1][3] = 0.0f;

	mat->M[2][0] = 0.0f;
	mat->M[2][1] = 0.0f;
	mat->M[2][2] = 1.0f;
	mat->M[2][3] = 0.0f;

	mat->M[3][0] = 0.0f;
	mat->M[3][1] = 0.0f;
	mat->M[3][2] = 0.0f;
	mat->M[3][3] = 1.0f;
}

void matrixRotate(MATRIX* m, double xTheta, double yTheta, double zTheta) {
	float xScale, yScale, zScale;
	float sinX, cosX;
	float sinY, cosY;
	float sinZ, cosZ;

	xScale = m->M[0][0];
	yScale = m->M[1][1];
	zScale = m->M[2][2];
	sinX = (float) sin(xTheta);
	cosX = (float) cos(xTheta);
	sinY = (float) sin(yTheta);
	cosY = (float) cos(yTheta);
	sinZ = (float) sin(zTheta);
	cosZ = (float) cos(zTheta);

	m->M[0][0] = (float) ((cosZ * cosY) * xScale);
	m->M[0][1] = (float) ((cosZ * -sinY * -sinX + sinZ * cosX) * yScale);
	m->M[0][2] = (float) ((cosZ * -sinY * cosX + sinZ * sinX) * zScale);

	m->M[1][0] = (float) (-sinZ * cosY * xScale);
	m->M[1][1] = (float) ((-sinZ * -sinY * -sinX + cosZ * cosX) * yScale);
	m->M[1][2] = (float) ((-sinZ * -sinY * cosX + cosZ * sinX) * zScale);

	m->M[2][0] = (float) (sinY * xScale);
	m->M[2][1] = (float) (cosY * -sinX * yScale);
	m->M[2][2] = (float) (cosY * cosX * zScale);
}

void matrixTranslate(MATRIX* m, double xTrans, double yTrans,
                     double zTrans) {
	m->M[0][3] = (float) xTrans;
	m->M[1][3] = (float) yTrans;
	m->M[2][3] = (float) zTrans;
}

void matrixMult(MATRIX* m1, MATRIX* m2, MATRIX* m3) {
	int i, j;

	for(j = 0; j < 4; j++) {
		for(i = 0; i < 4; i++) {
			m1->M[j][i] = m2->M[j][0] * m3->M[0][i] +
			        m2->M[j][1] * m3->M[1][i] +
			        m2->M[j][2] * m3->M[2][i] +
			        m2->M[j][3] * m3->M[3][i];
		}
	}
}

void calcNorm(POINT3D* norm, POINT3D* p1, POINT3D* p2, POINT3D* p3) {
	float crossX, crossY, crossZ;
	float abX, abY, abZ;
	float acX, acY, acZ;
	float sqrLength;
	float invLength;

	abX = p2->x - p1->x;// calculate p2 - p1
	abY = p2->y - p1->y;
	abZ = p2->z - p1->z;

	acX = p3->x - p1->x;// calculate p3 - p1
	acY = p3->y - p1->y;
	acZ = p3->z - p1->z;

	crossX = (abY * acZ) - (abZ * acY);// get cross product
	crossY = (abZ * acX) - (abX * acZ);// (p2 - p1) X (p3 - p1)
	crossZ = (abX * acY) - (abY * acX);

	sqrLength = (crossX * crossX) + (crossY * crossY) +
	        (crossZ * crossZ);

	if(sqrLength > ZERO_EPS)
		invLength = (float) (1.0 / sqrt(sqrLength));
	else
		invLength = 1.0f;

	norm->x = crossX * invLength;
	norm->y = crossY * invLength;
	norm->z = crossZ * invLength;
}

void normalizeNorm(POINT3D* n) {
	float len;

	len = (n->x * n->x) + (n->y * n->y) + (n->z * n->z);
	if(len > ZERO_EPS)
		len = (float) (1.0 / sqrt(len));
	else
		len = 1.0f;

	n->x *= len;
	n->y *= len;
	n->z *= len;
}

void normalizeNorms(POINT3D* p, unsigned long cPts) {
	float len;
	unsigned long i;

	for(i = 0; i < cPts; i++, p++) {
		len = (p->x * p->x) + (p->y * p->y) + (p->z * p->z);
		if(len > ZERO_EPS)
			len = (float) (1.0 / sqrt(len));
		else
			len = 1.0f;

		p->x *= len;
		p->y *= len;
		p->z *= len;
	}
}

GLint notchTurn[NUM_DIRS][NUM_DIRS][NUM_DIRS] = {
        // oldDir = +x
        iXX, iXX, iXX, iXX, iXX, iXX,
        iXX, iXX, iXX, iXX, iXX, iXX,
        iXX, iXX, MINUS_X, PLUS_X, PLUS_Z, MINUS_Z,
        iXX, iXX, PLUS_X, MINUS_X, PLUS_Z, MINUS_Z,
        iXX, iXX, PLUS_Y, MINUS_Y, MINUS_X, PLUS_X,
        iXX, iXX, PLUS_Y, MINUS_Y, PLUS_X, MINUS_X,
        // oldDir = -x
        iXX, iXX, iXX, iXX, iXX, iXX,
        iXX, iXX, iXX, iXX, iXX, iXX,
        iXX, iXX, PLUS_X, MINUS_X, PLUS_Z, MINUS_Z,
        iXX, iXX, MINUS_X, PLUS_X, PLUS_Z, MINUS_Z,
        iXX, iXX, PLUS_Y, MINUS_Y, PLUS_X, MINUS_X,
        iXX, iXX, PLUS_Y, MINUS_Y, MINUS_X, PLUS_X,
        // oldDir = +y
        MINUS_Y, PLUS_Y, iXX, iXX, PLUS_Z, MINUS_Z,
        PLUS_Y, MINUS_Y, iXX, iXX, PLUS_Z, MINUS_Z,
        iXX, iXX, iXX, iXX, iXX, iXX,
        iXX, iXX, iXX, iXX, iXX, iXX,
        PLUS_X, MINUS_X, iXX, iXX, MINUS_Y, PLUS_Y,
        PLUS_X, MINUS_X, iXX, iXX, PLUS_Y, MINUS_Y,
        // oldDir = -y
        PLUS_Y, MINUS_Y, iXX, iXX, PLUS_Z, MINUS_Z,
        MINUS_Y, PLUS_Y, iXX, iXX, PLUS_Z, MINUS_Z,
        iXX, iXX, iXX, iXX, iXX, iXX,
        iXX, iXX, iXX, iXX, iXX, iXX,
        PLUS_X, MINUS_X, iXX, iXX, PLUS_Y, MINUS_Y,
        PLUS_X, MINUS_X, iXX, iXX, MINUS_Y, PLUS_Y,
        // oldDir = +z
        MINUS_Z, PLUS_Z, PLUS_Y, MINUS_Y, iXX, iXX,
        PLUS_Z, MINUS_Z, PLUS_Y, MINUS_Y, iXX, iXX,
        PLUS_X, MINUS_X, MINUS_Z, PLUS_Z, iXX, iXX,
        PLUS_X, MINUS_X, PLUS_Z, MINUS_Z, iXX, iXX,
        iXX, iXX, iXX, iXX, iXX, iXX,
        iXX, iXX, iXX, iXX, iXX, iXX,
        // oldDir = -z
        PLUS_Z, MINUS_Z, PLUS_Y, MINUS_Y, iXX, iXX,
        MINUS_Z, PLUS_Z, PLUS_Y, MINUS_Y, iXX, iXX,
        PLUS_X, MINUS_X, PLUS_Z, MINUS_Z, iXX, iXX,
        PLUS_X, MINUS_X, MINUS_Z, PLUS_Z, iXX, iXX,
        iXX, iXX, iXX, iXX, iXX, iXX,
        iXX, iXX, iXX, iXX, iXX, iXX};

GLint defCylNotch[NUM_DIRS] =
        {PLUS_Y, PLUS_Y, MINUS_Z, PLUS_Z, PLUS_Y, PLUS_Y};

float RotZ[NUM_DIRS][NUM_DIRS] = {
        0.0f, 0.0f, 90.0f, 90.0f, 90.0f, -90.0f,
        0.0f, 0.0f, -90.0f, -90.0f, -90.0f, 90.0f,
        180.0f, 180.0f, 0.0f, 0.0f, 180.0f, 180.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -90.0f, 90.0f, 0.0f, 180.0f, 0.0f, 0.0f,
        90.0f, -90.0f, 180.0f, 0.0f, 0.0f, 0.0f};

/*-----------------------------------------------------------------------
|                                                                       |
|    align_plusy( int lastDir, int newDir )                             |
|       - Assuming +z axis is already aligned with newDir, align        |
|         +y axis BACK along lastDir                                    |
|                                                                       |
-----------------------------------------------------------------------*/

// given a dir, determine how much to rotate cylinder around z to match notches
// format is [newDir][notchVec]

GLfloat alignNotchRot[NUM_DIRS][NUM_DIRS] = {
        fXX, fXX, 0.0f, 180.0f, 90.0f, -90.0f,
        fXX, fXX, 0.0f, 180.0f, -90.0f, 90.0f,
        -90.0f, 90.0f, fXX, fXX, 180.0f, 0.0f,
        -90.0f, 90.0f, fXX, fXX, 0.0f, 180.0f,
        -90.0f, 90.0f, 0.0f, 180.0f, fXX, fXX,
        90.0f, -90.0f, 0.0f, 180.0f, fXX, fXX};

void align_plusz(int newDir) {
	// align +z along new direction
	switch(newDir) {
		case PLUS_X:
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			break;
		case MINUS_X:
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
			break;
		case PLUS_Y:
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
			break;
		case MINUS_Y:
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			break;
		case PLUS_Z:
			glRotatef(0.0f, 0.0f, 1.0f, 0.0f);
			break;
		case MINUS_Z:
			glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
			break;
	}
}

void align_plusy(int oldDir, int newDir) {
	GLfloat rotz;

	rotz = RotZ[oldDir][newDir];
	glRotatef(rotz, 0.0f, 0.0f, 1.0f);
}

/**************************************************************************\
* Geometry functions
\**************************************************************************/

/*-----------------------------------------------------------------------
|                                                                       |
|    align_notch( int newDir )                                          |
|       - a cylinder is notched, and we have to line this up            |
|         with the previous primitive's notch which is maintained as    |
|         notchVec.                                                     |
|       - this adds a rotation around z to achieve this                 |
|                                                                       |
-----------------------------------------------------------------------*/

void align_notch(int newDir, int notch) {
	GLfloat rotz;
	GLint curNotch;

	// figure out where notch is presently after +z alignment
	curNotch = defCylNotch[newDir];
	// (don't need this now we have lut)

	// look up rotation value in table
	rotz = alignNotchRot[newDir][notch];
#if PIPES_DEBUG
	if(rotz == fXX) {
		printf("align_notch(): unexpected value\n");
		return;
	}
#endif

	if(rotz != 0.0f)
		glRotatef(rotz, 0.0f, 0.0f, 1.0f);
}
