/**
 * @file gl_materials.h
 * @author Alex "FaceFTW" Westerman
 * @brief Material related functions/Definitions
 * This was copied mostly from SSCOMMON.H and MATERIAL.C
 * @version 0.1
 * @date 2022-03-21
 * 
 * @copyright Copyright (c) 2022. Work is based on original work from Microsoft Corp (c) 1994
 * 
 */

#ifndef __gl_material_h_
#define __gl_material_h_

#include "../structs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize Material structure with data in TeaMaterialData
 */
void InitTeaMaterials();

// /**
//  * @brief Initialize Material structure with data in TexMaterialData (tex materials follow the tea materials in Material)
//  */
// void InitTexMaterials();

/**
 * @brief Initialize MATERIAL structures with data
 * - Set alpha to 0.5 for all materials
 */
static void InitMaterials(MATERIAL* pm, float* pd, int count);

/**
 * @brief Initialize all materials
 */
void InitMaterials();

/**
 * @brief Set specified material with MATERIAL structure
 * 
 * @param pMat 
 */
void SetMaterial(MATERIAL* pMat);

/**
 * @brief Set material using its index name
 * 
 * @param index 
 */
void SetMaterialIndex(int index);

/**
 * @brief Select a random TeaMaterial
 * @param bSet 
 * @return ptr to material
 */
MATERIAL* RandomTeaMaterial(bool bSet);

/**
 * @brief Select a random TeaMaterial
 * @param bSet 
 * @return material index
 */
int RandomTeaMaterialIndex(bool bSet);

/**
 * @brief Given 2 materials and a count, create an incremental material, such
 * that adding it 'count' times to the first material will generate the
 * 2nd material.
 * 
 * @param matInc 
 * @param startMat 
 * @param endMat 
 * @param transCount 
 */
void CreateMaterialGradient(MATERIAL* matInc, MATERIAL* startMat,
                            MATERIAL* endMat, int transCount);

/**
 * @brief Given a material, add to it the incremental material, and set the
 * result as the current material
 * 
 * @param transMat 
 * @param transMatInc 
 */
void TransitionMaterial(MATERIAL* transMat, MATERIAL* transMatInc);

#ifdef __cplusplus
}
#endif

#endif//__gl_material_h_