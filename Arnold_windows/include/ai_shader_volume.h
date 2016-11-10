/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2016 Solid Angle SL
 */

/**
 * \file 
 * Volume functions for shaders
 */
 
#pragma once
#include "ai_color.h"
#include "ai_vector.h"
#include "ai_api.h"
#include "ai_string.h"

// forward declaration
struct AtShaderGlobals;

/** \defgroup ai_shader_volume Volume Rendering API
 * \{
 */

/** \name Volume sampling
 *
 * \details
 * These macros are used to retrieve volume channel values from within
 * volume shaders.
 * 
 * \param      _channel  Name of data channel in the volume node
 * \param      _interp   Volume interpolation quality, one of \c AI_VOLUME_INTERP_*
 * \param[out] _val      Resulting sampled value at the shader globals position
 * \return               True if sampling was successful
 *
 * Please refer to ai_shader_volume.h for a description of the volume sampling
 * functions called by these macros.
 *
 * \{
 */
#define AiVolumeSampleFlt(_channel,_interp,_val)   AiVolumeSampleFltFunc (_channel, sg, _interp, _val)
#define AiVolumeSampleRGB(_channel,_interp,_val)   AiVolumeSampleRGBFunc (_channel, sg, _interp, _val)
#define AiVolumeSampleRGBA(_channel,_interp,_val)  AiVolumeSampleRGBAFunc(_channel, sg, _interp, _val)
#define AiVolumeSampleVec(_channel,_interp,_val)   AiVolumeSampleVecFunc (_channel, sg, _interp, _val)
#define AiVolumeSamplePnt(_channel,_interp,_val)   AiVolumeSamplePntFunc (_channel, sg, _interp, _val)
#define AiVolumeSamplePnt2(_channel,_interp,_val)  AiVolumeSamplePnt2Func(_channel, sg, _interp, _val)
/*\}*/

#define AiCreateFuncs(_name, _type)                                                                                                            \
AI_API AI_DEPRECATED bool AiVolumeSample##_name##Func        (const char* channel, const AtShaderGlobals *sg, int interp, _type* value);       \
AI_API               bool AiVolumeSample##_name##FuncAtString(const AtString channel,    const AtShaderGlobals *sg, int interp, _type* value); \
inline               bool AiVolumeSample##_name##Func        (const AtString channel,    const AtShaderGlobals *sg, int interp, _type* value)  \
{                                                                                                                                              \
   return AiVolumeSample##_name##FuncAtString(channel, sg, interp, value);                                                                     \
}

AiCreateFuncs(Flt, float)
AiCreateFuncs(RGB, AtRGB)
AiCreateFuncs(RGBA, AtRGBA)
AiCreateFuncs(Vec, AtVector)
AiCreateFuncs(Pnt, AtPoint)
AiCreateFuncs(Pnt2, AtPoint2)
#undef AiCreateFuncs

AI_API void AiShaderGlobalsSetVolumeAbsorption(const AtShaderGlobals *sg, const AtRGB &absorption);
AI_API void AiShaderGlobalsSetVolumeAttenuation(const AtShaderGlobals *sg, const AtRGB &attenuation);
AI_API void AiShaderGlobalsSetVolumeEmission(const AtShaderGlobals *sg, const AtRGB &emission);
AI_API void AiShaderGlobalsSetVolumeScattering(const AtShaderGlobals *sg, const AtRGB &scattering, float g = 0.f);

/*\}*/
