/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2016 Solid Angle SL
 */

/**
 * \file 
 * Perlin noise and Worley cellular noise, in both scalar and vector flavors
 */

#pragma once
#include "ai_vector.h"
#include "ai_api.h"

/** \defgroup ai_noise Procedural texturing API
 * \{                                                                              
 */       

/** \name Perlin Noise
 * \{
 */
AI_API AI_CONST float AiPerlin2(AtPoint2 p);
AI_API AI_CONST float AiPerlin3(AtPoint  p);
AI_API AI_CONST float AiPerlin4(AtPoint  p, float time);
/*\}*/

/** \name Periodic Perlin Noise
 * \{
 */
AI_API AI_CONST float AiPeriodicPerlin2(AtPoint2 p, int periodx, int periody);
AI_API AI_CONST float AiPeriodicPerlin3(AtPoint  p, int periodx, int periody, int periodz);
AI_API AI_CONST float AiPeriodicPerlin4(AtPoint  p, float time, int periodx, int periody, int periodz, int periodt);
/*\}*/

/** \name Summed-Noise
 * \{
 */
AI_API AI_CONST float     AiNoise2 (AtPoint2 p, int octaves, float distortion, float lacunarity);
AI_API AI_CONST float     AiNoise3 (AtPoint  p, int octaves, float distortion, float lacunarity);
AI_API AI_CONST float     AiNoise4 (AtPoint  p, float time, int octaves, float distortion, float lacunarity);
AI_API AI_CONST AtVector2 AiVNoise2(AtPoint2 p, int octaves, float distortion, float lacunarity);
AI_API AI_CONST AtVector  AiVNoise3(AtPoint  p, int octaves, float distortion, float lacunarity);
AI_API AI_CONST AtVector  AiVNoise4(AtPoint  p, float time, int octaves, float distortion, float lacunarity);
/*\}*/

/** \name Cellular Noise
 * \{
 */
AI_API void               AiCellular(AtPoint p, int n, int octaves, float lacunarity, float randomness, float* F, AtVector* delta = NULL, AtUInt32* ID = NULL);
AI_API AI_CONST float     AiCellNoise2(AtPoint2 p);
AI_API AI_CONST float     AiCellNoise3(AtPoint  p);
AI_API AI_CONST float     AiCellNoise4(AtPoint  p, float t);
AI_API AI_CONST AtVector2 AiVCellNoise2(AtPoint2 p);
AI_API AI_CONST AtVector  AiVCellNoise3(AtPoint  p);
AI_API AI_CONST AtVector  AiVCellNoise4(AtPoint  p, float t);
/*\}*/

/*\}*/
