/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2016 Solid Angle SL
 */

/**
 * \file 
 * Pointcloud iterator interface, deprecated, will be removed in a future release.
 */

#pragma once
#include "ai_vector.h"

// forward declaration
struct AtShaderGlobals;

/** \defgroup ai_pointcloud Pointcloud Iterator API
 * \{
 */

/*
 * Point cloud sample distributions
 */
#define AI_POINTCLOUD_BLUE_NOISE          0x00
#define AI_POINTCLOUD_TRIANGLE_MIDPOINT   0x01
#define AI_POINTCLOUD_POLYGON_MIDPOINT    0x02

// Iterator interface
struct AtPointCloudSample
{
   AtPoint  pref_position;
   AtPoint  world_position;
   AtVector normal;
   AtUInt32 face;
   AtPoint2 uv;
   float    area;
};
struct AtPointCloudIterator;
AI_API AtPointCloudIterator* AiPointCloudIteratorCreate  (const AtShaderGlobals* sg, int distribution, float spacing = 0.1f);
AI_API void                  AiPointCloudIteratorDestroy (AtPointCloudIterator* iter);
AI_API AtPointCloudSample    AiPointCloudIteratorGetNext (AtPointCloudIterator* iter);
AI_API AI_PURE bool          AiPointCloudIteratorFinished(const AtPointCloudIterator* iter);
AI_API AI_PURE AtUInt32      AiPointCloudGetSampleCount  (const AtPointCloudIterator* iter);

/*\}*/
