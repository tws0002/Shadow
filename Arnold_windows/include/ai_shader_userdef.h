/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2016 Solid Angle SL
 */

/**
 * \file 
 * User data functions for shaders
 */

#pragma once
#include "ai_shaderglobals.h"
#include "ai_array.h"
#include "ai_color.h"
#include "ai_matrix.h"
#include "ai_vector.h"
#include "ai_types.h"
#include "ai_nodes.h"
#include "ai_string.h"

/** \defgroup ai_shader_userdef User-Data API
 *
 * \details
 * This API is for accessing user-defined data on geometry. User-defined data
 * (sometimes called "user-data") are new parameters that have been added to
 * existing node-instances via AiNodeDeclare().
 *
 * There are four kinds of user-defined data: 
 *  - \c constant -- constant parameters are data that are defined on a per-object
 *                   basis and do not vary across the surface of that object.
 *  - \c uniform  -- uniform parameters are data that are defined on a "per-face"
 *                   basis.  During subdivision (if appropriate) values are
 *                   not interpolated.  Instead, the newly subdivided faces will
 *                   contain the value of their "parent" face.
 *  - \c varying  -- varying parameters are data that are defined on a per-vertex
 *                   basis.  During subdivision (if appropriate), the values 
 *                   at the new vertices are interpolated from the values at the
 *                   old vertices.  The user should only create parameters of
 *                   "interpolatable" variable types (such as floats, colors, etc.) 
 *  - \c indexed  -- indexed parameters are data that are defined on a per-face-vertex
 *                   basis.  During subdivision (if appropriate), the values
 *                   at the new vertices are interpolated from the values at the
 *                   old vertices, preserving edges where values were not shared.
 *                   The user should only create parameters of "interpolatable"
 *                   variable types (such as floats, colors, etc.)
 *
 * \{
 */


/** \name User-Data parameter
 *
 * \details
 * This macro is used to retrieve the user-data parameter from within the shaders.
 *
 * \param _name  a string with the name of the user-data value to query
 * \return       the parameter for the user data, or NULL if not found
 *
 * \{
 */
#define AiUDataGetParameter(_name) AiUserGetParameterFunc(_name, sg)
/*\}*/

/** \name User-Data Readers
 *
 * \details
 * These macros are used to retrieve user-data values from within
 * shaders.  If a varying-type user-data is requested, then the
 * user-data will be correctly interpolated across the face of the primitive.
 *
 * In the case of reading constant-type user-data, the usual node-parameter
 * reading functions, such as AiNodeGetInt(), may also be used.
 *
 * \param _name  a string with the name of the user-data value to read
 * \param _val   a pointer to the value in which to store the contents of the
 *               read operation
 * \return a boolean indicating whether the operation was successful
 *
 * Please refer to ai_shader_userdef.h for a description of the userdef-data
 * reading functions called by these macros.
 *
 * \{
 */
#define AiUDataGetBool(_name,_val)    AiUserGetBoolFunc  (_name, sg, _val)
#define AiUDataGetByte(_name,_val)    AiUserGetByteFunc  (_name, sg, _val)
#define AiUDataGetInt(_name,_val)     AiUserGetIntFunc   (_name, sg, _val)
#define AiUDataGetUInt(_name,_val)    AiUserGetUIntFunc  (_name, sg, _val)
#define AiUDataGetFlt(_name,_val)     AiUserGetFltFunc   (_name, sg, _val)
#define AiUDataGetRGB(_name,_val)     AiUserGetRGBFunc   (_name, sg, _val)
#define AiUDataGetRGBA(_name,_val)    AiUserGetRGBAFunc  (_name, sg, _val)
#define AiUDataGetVec(_name,_val)     AiUserGetVecFunc   (_name, sg, _val)
#define AiUDataGetPnt(_name,_val)     AiUserGetPntFunc   (_name, sg, _val)
#define AiUDataGetPnt2(_name,_val)    AiUserGetPnt2Func  (_name, sg, _val)
#define AiUDataGetStr(_name,_val)     AiUserGetStrFunc   (_name, sg, _val)
#define AiUDataGetPtr(_name,_val)     AiUserGetPtrFunc   (_name, sg, _val)
#define AiUDataGetNode(_name,_val)    AiUserGetNodeFunc  (_name, sg, _val)
#define AiUDataGetArray(_name,_val)   AiUserGetArrayFunc (_name, sg, _val)
#define AiUDataGetMatrix(_name,_val)  AiUserGetMatrixFunc(_name, sg, _val)
/*\}*/


/** \name User-Data Derivatives with respect to screen X and Y
 *
 * \details
 * These macros are used to retrieve user-data derivatives w.r.t. X and Y from
 * within shaders.  If a varying-type user-data is requested, then the
 * user-data derivatives will be correctly interpolated across the face of the
 * primitive.  These derivatives can be useful in determining filter sizes to
 * match pixel footprints, such as when texture mapping.
 *
 * In the case of reading constant-type user-data, the derivatives are zero.
 *
 * \param _name     a string with the name of the user-data value to read
 * \param _dx_val   a pointer to the value in which to store the contents of the
 *                  computed derivatives with respect to the X pixel direction
 * \param _dy_val   a pointer to the value in which to store the contents of the
 *                  computed derivatives with respect to the Y pixel direction
 * \return a boolean indicating whether the operation was successful
 *
 * Please refer to ai_shader_userdef.h for a description of the userdef-data
 * reading functions called by these macros.
 *
 * \{
 */
#define AiUDataGetDxyDerivativesFlt(_name,_dx_val,_dy_val)     AiUserGetDxyDerivativesFltFunc   (_name, sg, _dx_val, _dy_val)
#define AiUDataGetDxyDerivativesRGB(_name,_dx_val,_dy_val)     AiUserGetDxyDerivativesRGBFunc   (_name, sg, _dx_val, _dy_val)
#define AiUDataGetDxyDerivativesRGBA(_name,_dx_val,_dy_val)    AiUserGetDxyDerivativesRGBAFunc  (_name, sg, _dx_val, _dy_val)
#define AiUDataGetDxyDerivativesVec(_name,_dx_val,_dy_val)     AiUserGetDxyDerivativesVecFunc   (_name, sg, _dx_val, _dy_val)
#define AiUDataGetDxyDerivativesPnt(_name,_dx_val,_dy_val)     AiUserGetDxyDerivativesPntFunc   (_name, sg, _dx_val, _dy_val)
#define AiUDataGetDxyDerivativesPnt2(_name,_dx_val,_dy_val)    AiUserGetDxyDerivativesPnt2Func  (_name, sg, _dx_val, _dy_val)
#define AiUDataGetDxyDerivativesArray(_name,_dx_val,_dy_val)   AiUserGetDxyDerivativesArrayFunc (_name, sg, _dx_val, _dy_val)
#define AiUDataGetDxyDerivativesMatrix(_name,_dx_val,_dy_val)  AiUserGetDxyDerivativesMatrixFunc(_name, sg, _dx_val, _dy_val)
/*\}*/


/*\}*/

AI_API AI_DEPRECATED const AtUserParamEntry* AiUserGetParameterFunc        (const char*,    const AtShaderGlobals*);
AI_API               const AtUserParamEntry* AiUserGetParameterFuncAtString(const AtString, const AtShaderGlobals*);
inline               const AtUserParamEntry* AiUserGetParameterFunc(const AtString name, const AtShaderGlobals* sg) { return AiUserGetParameterFuncAtString(name, sg); }

#define AiCreateFuncs(_name, _type)                                                                               \
AI_API AI_DEPRECATED bool AiUserGet##_name##Func         (const char*,        const AtShaderGlobals*,    _type*); \
AI_API               bool AiUserGet##_name##FuncAtString (const AtString,     const AtShaderGlobals*,    _type*); \
inline               bool AiUserGet##_name##Func         (const AtString str, const AtShaderGlobals* sg, _type* x)\
{                                                                                                                 \
   return AiUserGet##_name##FuncAtString(str, sg, x);                                                             \
}


AiCreateFuncs(Bool, bool)
AiCreateFuncs(Byte, AtByte)
AiCreateFuncs(Int, int)
AiCreateFuncs(UInt, unsigned int)
AiCreateFuncs(Flt, float)
AiCreateFuncs(RGB, AtRGB)
AiCreateFuncs(RGBA, AtRGBA)
AiCreateFuncs(Vec, AtVector)
AiCreateFuncs(Pnt, AtPoint)
AiCreateFuncs(Pnt2, AtPoint2)
AiCreateFuncs(Str, const char*)
AiCreateFuncs(Ptr, void*)
AiCreateFuncs(Node, AtNode*)
AiCreateFuncs(Array, AtArray*)
AiCreateFuncs(Matrix, AtMatrix)
#undef AiCreateFuncs

#define AiCreateFuncs(_name, _type)                                                                                                         \
AI_API AI_DEPRECATED bool AiUserGetDxyDerivatives##_name##Func         (const char*,        const AtShaderGlobals*,    _type*,    _type*);  \
AI_API               bool AiUserGetDxyDerivatives##_name##FuncAtString (const AtString,     const AtShaderGlobals*,    _type*,    _type*);  \
inline               bool AiUserGetDxyDerivatives##_name##Func         (const AtString str, const AtShaderGlobals* sg, _type* dx, _type* dy)\
{                                                                                                                                           \
   return AiUserGetDxyDerivatives##_name##FuncAtString(str, sg, dx, dy);                                                                    \
}
AiCreateFuncs(Flt, float)
AiCreateFuncs(RGB, AtRGB)
AiCreateFuncs(RGBA, AtRGBA)
AiCreateFuncs(Vec, AtVector)
AiCreateFuncs(Pnt, AtPoint)
AiCreateFuncs(Pnt2, AtPoint2)
AiCreateFuncs(Array, AtArray*)
AiCreateFuncs(Matrix, AtMatrix)
#undef AiCreateFuncs
