/**
 * @file textures.h
 * @author Alex "FaceFTW" Westerman
 * @brief Texture Related Stuff
 * @version 0.1
 * @date 2022-03-16
 *
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 *
 */

#include "../utils.h"
#include <GL/gl.h>

#ifndef __textures_h_
#define __textures_h_
extern MATERIAL TeaMaterial[], TexMaterial[], ss_BlackMat;
extern bool gbTextureObjects; // from texture.c

// material processing

extern void InitTeaMaterials();
extern void InitTexMaterials();
extern void InitMaterials();
extern void SetMaterial(MATERIAL* pMat);
extern void SetMaterialIndex(int index);
extern MATERIAL* RandomTeaMaterial(bool bSet);
extern int  RandomTeaMaterialIndex(bool bSet);
extern MATERIAL* RandomTexMaterial(bool bSet);
extern int  RandomTexMaterialIndex(bool bSet);
extern void CreateMaterialGradient(MATERIAL* matInc, MATERIAL* startMat,
						MATERIAL* endMat, int transCount);
extern void TransitionMaterial(MATERIAL* transMat, MATERIAL* transMatInc);

// texture file processing

extern int  ss_LoadBMPTextureFile( LPCTSTR pszBmpfile, TEXTURE *pTex );
extern int  ss_LoadTextureFile( TEXFILE *texFile, TEXTURE *pTex );
extern int  ss_LoadTextureResource( TEX_RES *pTexRes, TEXTURE *pTex );
extern bool ss_CopyTexture( TEXTURE *pTexDst, TEXTURE *pTexSrc );
extern bool ss_SetTextureTransparency( TEXTURE *pTex, float alpha, bool bSet );
extern void ss_DisableTextureErrorMsgs();
extern void ss_SetTexture( TEXTURE *pTex );
extern void ss_SetTexturePalette( TEXTURE *pTex, int index );
extern void ss_DeleteTexture( TEXTURE *pTex );
extern bool ss_LoadTextureResourceStrings();
extern bool ss_DIBImageLoad(PVOID pvFile, TEXTURE *ptex);
extern bool ss_RGBImageLoad(PVOID pvFile, TEXTURE *ptex);
extern bool ss_A8ImageLoad(PVOID pvFile, TEXTURE *ptex);
extern bool ss_VerifyTextureFile( TEXFILE *ptf );
extern bool ss_SelectTextureFile( HWND hDlg, TEXFILE *ptf );
extern void ss_GetDefaultBmpFile( LPTSTR pszBmpFile );
extern void ss_InitAutoTexture( TEX_POINT2D *pTexRep );

// texture objects

extern bool TextureObjectsEnabled( void );

// Paletted texture support
extern bool PalettedTextureEnabled(void);
extern bool QueryPalettedTextureEXT(void);

#endif //__textures_h_