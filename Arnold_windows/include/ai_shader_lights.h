/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2016 Solid Angle SL
 */

/**
 * \file 
 * Shader utilities for sampling direct lighting from light sources
 */

#pragma once
#include "ai_nodes.h"
#include "ai_shaderglobals.h"
#include "ai_color.h"
#include "ai_vector.h"
#include "ai_ray.h"

/** \defgroup ai_shader_lights  Light source sampling API
 * \{
 */

/** \name Light Loop Methods
 * \details
 * This is the recommended API for looping over light source samples.
 * \{
 */
AI_API void  AiLightsPrepare(AtShaderGlobals* sg);
AI_API bool  AiLightsGetSample(AtShaderGlobals* sg);
AI_API AtRGB AiLightsGetShadowMatte(AtShaderGlobals* sg);
AI_API void  AiLightsResetCache(AtShaderGlobals* sg);
/*\}*/

/** \name Light Getter Methods
 * \details
 * These getters are to avoid slow AiNodeGet*() calls to access light parameters.
 * Only the most common attributes are exposed for now.
 * \{
 */
AI_API AI_PURE AtRGB AiLightGetColor(const AtNode* node);
AI_API AI_PURE float AiLightGetIntensity(const AtNode* node);
AI_API AI_PURE bool  AiLightGetAffectDiffuse(const AtNode* node);
AI_API AI_PURE bool  AiLightGetAffectSpecular(const AtNode* node);
AI_API AI_PURE float AiLightGetDiffuse(const AtNode* node);
AI_API AI_PURE float AiLightGetSpecular(const AtNode* node);
AI_API AI_PURE float AiLightGetSSS(const AtNode* node);
AI_API AI_PURE float AiLightGetVolume(const AtNode* node);
/*\}*/

/*\}*/
