/**
 * @file structs.h
 * @author Alex "FaceFTW" Westerman
 * @brief A bunch of common structs
 * @version 0.1
 * @date 2022-03-16
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */
#include <GL/gl.h>


#ifndef __structs_h_
#define __structs_h_

// texture quality level
enum {
	TEXQUAL_DEFAULT = 0,
	TEXQUAL_HIGH
};

typedef struct _point2d {
	GLfloat x;
	GLfloat y;
} POINT2D;

typedef struct _ipoint2d {
	int x;
	int y;
} IPOINT2D;

typedef struct _point3d {
	GLfloat x;
	GLfloat y;
	GLfloat z;
} POINT3D;

typedef struct _ipoint3d {
	int x;
	int y;
	int z;
} IPOINT3D;

typedef struct _texpoint2d {
	GLfloat s;
	GLfloat t;
} TEX_POINT2D;

typedef struct _isize {
	int width;
	int height;
} ISIZE;

typedef struct _fsize {
	GLfloat width;
	GLfloat height;
} FSIZE;

typedef struct _glrect {
	int x, y;
	int width, height;
} GLRECT;

// texture data
typedef struct {
	int     width;
	int     height;
	GLenum  format;
	GLsizei components;
	float   origAspectRatio; // original width/height aspect ratio
	unsigned char* data;
	GLuint  texObj;          // texture object
	int     pal_size;
	int     iPalRot;         // current palette rotation (not used yet)
	RGBQUAD* pal;
} TEXTURE, * HTEXTURE;

// texture resource types
enum {
	TEX_UNKNOWN = 0,
	TEX_RGB,
	TEX_BMP,
	TEX_A8
};

typedef struct {
	int     type;
	int     name;
} TEX_RES;

typedef struct _MATRIX {
	GLfloat M[4][4];
} MATRIX;

typedef struct strRGBA {
	GLfloat r;
	GLfloat g;
	GLfloat b;
	GLfloat a;
} RGBA;

typedef struct {
	byte r;
	byte g;
	byte b;
} RGB8;

typedef struct {
	byte r;
	byte g;
	byte b;
	byte a;
} RGBA8;

// DlgDraw

enum {
	DLG_INTENSITY_LOW = 0,
	DLG_INTENSITY_MID,
	DLG_INTENSITY_HIGH,
};

#endif //__structs_h