/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2016 Solid Angle SL
 */

/**
 * \file
 * Macros for backwards compatibility only - do not include this in new code
 * 
 * IMPORTANT: Only include this file from .c or .cpp. Never include this in a
 *            .h file. Also, make this the _last_ thing you include to avoid
 *            problems with system or 3rd party header files.
 */

#pragma once

#define AiGamma(g,c) AiColorGamma(c, g);

#define AI_PNT2_ZERO AI_P2_ZERO
#define AI_PNT2_ONE  AI_P2_ONE

#ifndef _ARNOLDDLL
#define _name AiNodeGetName(node)
#endif

#define AiNodeInstall   AiNodeEntryInstall
#define AiNodeUninstall AiNodeEntryUninstall

struct AtCameraInput;
struct AtCameraOutput;
struct AtNode;

AI_API void AiCameraCreateRayDeprecated(const AtNode* node, const AtCameraInput* input, AtCameraOutput* output, int tid);

#define AI_BUCKET_CB_FRAME_BEGIN           0x00  /**< Called at beginning of frame to set the bucket list */
#define AI_BUCKET_CB_FRAME_END             0x01  /**< Called when the whole frame has finished rendering */
#define AI_BUCKET_CB_BUCKET_BEGIN          0x02  /**< Called at beginning of bucket to get bucket coords */
#define AI_BUCKET_CB_BUCKET_END            0x03  /**< Called when bucket has finished rendering */

struct AtBucketCoords;

/*
 * This callback allows the user to select the next bucket to be rendered. To make things easier, the bucket
 * scheduler makes use of the bucket scanning mode to precompute a list of buckets (as usual) and then passes
 * this list to the callback. The user can chose between reusing that list as-is, creating a permutation, or 
 * just ignoring it.
 *
 * Sequence of callback calls for normal process is:
 *
 * - mode == AI_BUCKET_CB_FRAME_BEGIN: In this call we can read the whole bucket list and store it or discard it.
 * - mode == AI_BUCKET_CB_BUCKET_BEGIN: The callback has to set the coords for the given bucket.
 * - mode == AI_BUCKET_CB_BUCKET_END: Notifies that the given bucket has finished rendering.
 * - mode == AI_BUCKET_CB_FRAME_END: Notifies that the whole frame has finished rendering.
 */
typedef void (*AtBucketCallback)(int mode, int index, int nbuckets, const AtBucketCoords* bucket_list, AtBucketCoords* coords);

AI_API void AiRenderSetCallback(AtBucketCallback cb);
