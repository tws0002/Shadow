/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2016 Solid Angle SL
 */

/**
 * \file 
 * AOV-writing functions for shaders
 */

#pragma once
#include "ai_api.h"
#include "ai_params.h"
#include "ai_color.h"
#include "ai_vector.h"
#include "ai_string.h"
#include "ai_types.h"

struct AtShaderGlobals;

/** \defgroup ai_shader_aovs AOV (Arbitrary Output Variables) API 
 * \{
 *
 * This API is for writing AOVs (Arbitrary Ouput Values) from inside a shader.
 * Arnold can route any number of AOVs into any number of drivers that can
 * write to any number of output files.
 */

/** \name AOV-writing Functions
 *
 * \details
 * These functions allow the user to write AOVs.  Shaders may write
 * AOVs indiscriminately without hesitation because the system
 * understands the "context" in which an AOV write is performed. This means
 * Arnold can store AOVs when it "makes sense" and ignore writes when
 * it doesn't.
 *
 * For example, Arnold will not store "deep" AOV values (AOVs which are written 
 * at different depths for a singe pixel-sample) unless a driver specifically 
 * requests "deep" AOV values.  An example of a driver requiring "deep" AOV
 * values would be one which writes deep-texture/volume-shadow files.
 *
 * Arnold will only perform AOV writes from within the main
 * trunk of a pixel-sample ray-tree.  AOV writes made from inside a side-branch 
 * of the primary ray's ray-tree (such as a reflection) will be ignored.  Also,
 * AOV writes are ignored during shadow evaluations.
 *
 * \param sg    the current shader globals structure (pixel-sample state)
 * \param name  the name of the AOV to write into
 * \param val   the value to write into the AOV
 * \returns     a boolean indicating whether the write was successfully stored or not. 
 *              A failure could indicate that the AOV is not enabled, or is
 *              of the wrong type,  or is being written in the wrong "state" (such
 *              as not from within the trunk of the primary ray-tree).
 * \{
 */
#define AiCreateFuncs(_name, _type)                                                                       \
AI_API AI_DEPRECATED bool AiAOVSet##_name          (AtShaderGlobals* sg, const char* name,    _type val); \
AI_API               bool AiAOVSet##_name##AtString(AtShaderGlobals* sg, const AtString name, _type val); \
inline               bool AiAOVSet##_name          (AtShaderGlobals* sg, const AtString name, _type val)  \
{                                                                                                         \
   return AiAOVSet##_name##AtString(sg, name, val);                                                       \
}
AiCreateFuncs(Bool, bool)
AiCreateFuncs(Int, int)
AiCreateFuncs(Flt, float)
AiCreateFuncs(RGB, AtRGB)
AiCreateFuncs(RGBA, AtRGBA)
AiCreateFuncs(Vec, AtVector)
AiCreateFuncs(Pnt, AtPoint)
AiCreateFuncs(Pnt2, AtPoint2)
AiCreateFuncs(Ptr, void*)
#undef AiCreateFuncs
/*\}*/

/** \name AOV-reading Functions
 *
 * \details
 * These functions allow the user to read AOVs.
 *
 * \param sg    the current shader globals structure (pixel-sample state)
 * \param name  the name of the AOV to read from
 * \param val   the variable to store the read value
 * \returns     a boolean indicating whether the read was successfully done or not. 
 *              A failure could indicate that the AOV is not enabled, or is
 *              of the wrong type.
 * \{
 */
#define AiCreateFuncs(_name, _type)                                                                              \
AI_API AI_DEPRECATED bool AiAOVGet##_name          (const AtShaderGlobals* sg, const char* name,    _type& val); \
AI_API               bool AiAOVGet##_name##AtString(const AtShaderGlobals* sg, const AtString name, _type& val); \
inline               bool AiAOVGet##_name          (const AtShaderGlobals* sg, const AtString name, _type& val)  \
{                                                                                                                \
   return AiAOVGet##_name##AtString(sg, name, val);                                                              \
}
AiCreateFuncs(Bool, bool)
AiCreateFuncs(Int, int)
AiCreateFuncs(Flt, float)
AiCreateFuncs(RGB, AtRGB)
AiCreateFuncs(RGBA, AtRGBA)
AiCreateFuncs(Vec, AtVector)
AiCreateFuncs(Pnt, AtPoint)
AiCreateFuncs(Pnt2, AtPoint2)
AiCreateFuncs(Ptr, void*)
#undef AiCreateFuncs
/*\}*/

#define AI_AOV_BLEND_NONE     0
#define AI_AOV_BLEND_OPACITY  1

AI_API AI_DEPRECATED bool AiAOVEnabled        (const char* name,    AtByte type);
AI_API               bool AiAOVEnabledAtString(const AtString name, AtByte type);
inline               bool AiAOVEnabled        (const AtString name, AtByte type) { return AiAOVEnabledAtString(name, type); }

AI_API bool AiAOVRegister(const char* name, AtByte type, int blend_mode = AI_AOV_BLEND_NONE);

/*\}*/
