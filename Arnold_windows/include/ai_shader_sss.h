/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2016 Solid Angle SL
 */

/**
 * \file 
 * SSS functions for shaders
 */

#pragma once
#include "ai_color.h"

// forward declaration
struct AtShaderGlobals;

/** \defgroup ai_shader_sss Sub-Surface Scattering API
 * \{
 */

AI_API AI_PURE AtColor AiBSSRDFCubic(const AtShaderGlobals* sg, const float* radius, const AtColor* weight, unsigned int num = 1);
AI_API         void    AiBSSRDFCubicSeparate(const AtShaderGlobals* sg, AtRGB& direct, AtRGB& indirect, const float* radius, const AtColor* weight, unsigned int num = 1);
AI_API AI_PURE AtColor AiBSSRDFGaussian(const AtShaderGlobals* sg, const float* variance, const AtColor* weight, unsigned int num = 1);
AI_API         void    AiBSSRDFGaussianSeparate(const AtShaderGlobals* sg, AtRGB& direct, AtRGB& indirect, const float* variance, const AtColor* weight, unsigned int num = 1);
AI_API         void    AiBSSRDFEmpirical(const AtShaderGlobals* sg, AtRGB& direct, AtRGB& indirect, const float* mfp, const float* albedo, const AtRGB* weight, unsigned int num = 1);
AI_API         AtColor AiSSSTraceSingleScatter(AtShaderGlobals* sg, AtColor Rd, AtColor mfp, float g, float eta);
AI_API AI_PURE bool    AiShaderGlobalsFromSSS(const AtShaderGlobals* sg);

/*\}*/
