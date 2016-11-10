/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2016 Solid Angle SL
 */

/**
 * \file 
 * API for writing volume plugins
 */

#pragma once

#include "ai_bbox.h"
#include "ai_color.h"
#include "ai_params.h"
#include "ai_string.h"

// forward declaration
struct AtNode;
struct AtShaderGlobals;

/** \defgroup ai_volumes Volume Plugin API
 * 
 * This API is used to interact with custom volumes and formats at render time.
 * This is accomplished by providing the renderer some callback functions which
 * are called as needed (when the volume is hit by a ray).  Volume plugins are
 * intended to provide tight ray intervals surrounding actual volumetric data,
 * as well as sampling methods for sampling data channels from volumes.
 *
 * Note that volume plugins have a thread-locked part: init and cleanup, while
 * the rest of the callbacks may be called concurrently from multiple threads.
 *
 * \{                                                                              
 */       

/** \name Volume interpolation quality
 * \{
 */
#define AI_VOLUME_INTERP_CLOSEST   0   /**< closest data from the volume */
#define AI_VOLUME_INTERP_TRILINEAR 1   /**< trilinearly sampled volume */
#define AI_VOLUME_INTERP_TRICUBIC  2   /**< tricubicly sampled volume */
/* \} */

/**
 * Volume data, as returned by \ref AtVolumePluginCreateVolume
 */
struct AtVolumeData
{
   void*  private_info;    /**< Volume plugin private data, used how the plugin likes */
   AtBBox bbox;            /**< Bounding box for this volume */
   float  auto_step_size;  /**< Recommended step size for ray marching through this data */
};

struct AtVolumeIntersectionInfo;

AI_API void AiVolumeAddIntersection(const AtVolumeIntersectionInfo* info, float t0, float t1);


/**
 * Volume plugin init method.
 * 
 * This method will be called first and should perform any initialization required
 * by this volume type. This might involve initializing an external volume format
 * API, for example.
 *
 * \param[out] user_ptr  This is a general-purpose, user-supplied data pointer that
 *                       Arnold will pass along to the other volume plugin methods.
 * \return               true upon success
 */
typedef bool (*AtVolumePluginInit)(void** user_ptr);

/**
 * Volume plugin cleanup method.
 * 
 * This method will be called last and should perform any cleanup required
 * by your volume plugin. Make sure you release any memory you allocated that is
 * no longer needed by Arnold.
 *
 * \param user_ptr  User data pointer, as returned from \ref AtVolumePluginInit
 * \return          true upon success
 */
typedef bool (*AtVolumePluginCleanup)(void* user_ptr);

/**
 * Volume plugin volume creation.
 * 
 * This method will be called for each volume node before usage.
 *
 * \param      user_ptr     User data pointer, as returned from \ref AtVolumePluginInit
 * \param      user_string  User configuration string
 * \param      node         Owner node where the volume was requested
 * \param[out] data         Volume data with all fields to be set by the callback
 * \return                  true if volume creation succeeded
 */
typedef bool (*AtVolumePluginCreateVolume)(void* user_ptr,
                                           const char* user_string,
                                           const AtNode* node,
                                           AtVolumeData* data);

/**
 * Volume plugin volume update.
 * 
 * This optional method will be called for each volume node before each render
 * pass or scene change, so that the volume can be update accordingly. If not
 * provided, the volume will be destroyed and recreated.
 *
 * \param      user_ptr     User data pointer, as returned from \ref AtVolumePluginInit
 * \param      user_string  User configuration string
 * \param      node         Owner node where the volume was requested
 * \param[out] data         Volume data with all fields to be set by the callback
 * \return                  true if volume data was modified
 */
typedef bool (*AtVolumePluginUpdateVolume)(void* user_ptr,
                                           const char* user_string,
                                           const AtNode* node,
                                           AtVolumeData* data);

/**
 * Volume plugin volume cleanup method.
 * 
 * This method will be called once for each volume that was created by a call to
 * \ref AtVolumePluginCreateVolume to allow a chance to clean up any private data.
 *
 * \param user_ptr  User data pointer, as returned from \ref AtVolumePluginInit
 * \param data      Volume data returned from \ref AtVolumePluginCreateVolume
 * \return          true upon success
 */
typedef bool (*AtVolumePluginCleanupVolume)(void* user_ptr,
                                            AtVolumeData* data,
                                            const AtNode* node);

/**
 * Volume plugin sample method.
 * 
 * This method will be called concurrently to sample data from a given channel
 * with the specified interpolation.  Implementors should use sg->Po as the
 * sampling position.
 *
 * \param      user_ptr  User data pointer, as returned from \ref AtVolumePluginInit
 * \param      data      Volume data returned from \ref AtVolumePluginCreateVolume
 * \param      channel   Data channel name from the volume
 * \param      sg        Shader globals for the sampling context
 * \param      interp    Volume interpolation quality, one of \c AI_VOLUME_INTERP_*
 * \param[out] value     Resulting sampled value, matching the type output in out_type
 * \param[out] type      Resulting value type, one of \c AI_TYPE_FLOAT, \c AI_TYPE_POINT, \c AI_TYPE_POINT2, \c AI_TYPE_RGB, \c AI_TYPE_RGBA, or \c AI_TYPE_VECTOR
 * \return               true upon success
 */
typedef bool (*AtVolumePluginSample)(void* user_ptr,
                                     const AtVolumeData* data,
                                     const AtString channel,
                                     const AtShaderGlobals* sg,
                                     int interp,
                                     AtParamValue *value,
                                     AtByte *type);

typedef bool (*AtVolumePluginSampleDeprecated)(void* user_ptr,
                                               const AtVolumeData* data,
                                               const char* channel,
                                               const AtShaderGlobals* sg,
                                               int interp,
                                               AtParamValue *value,
                                               AtByte *type);

/**
 * Volume plugin method for submitting extents along a ray where there is data.
 * 
 * For each ray interval where there is volumetric potentially to be integrated
 * this callback should call \ref AiVolumeAddIntersection to submit the extent
 * along the ray.
 *
 * \warning
 * Any ray extents submitted that overlap will have the shaders run more than
 * once for each extent.  If shaders should only be run once, then those extents
 * should be merged and submitted just once to \ref AiVolumeAddIntersection instead.
 *
 * \param user_ptr  User data pointer, as returned from \ref AtVolumePluginInit
 * \param data      Volume data returned from \ref AtVolumePluginCreateVolume
 * \param info      Opaque ptr to intersection info passed to \ref AiVolumeAddIntersection
 * \param tid       Current thread ID, use for thread-local access as needed
 * \param time      Time at which the volume is being sampled (for motion blur)
 * \param origin    Ray origin in object space
 * \param direction Ray direction, normalized and in object space
 * \param t0        Start of the master ray interval in which to check for extents
 * \param t1        End of the master ray interval in which to check for extents
 */
typedef void (*AtVolumePluginRayExtents)(void* user_ptr,
                                         const AtVolumeData* data,
                                         const AtVolumeIntersectionInfo* info,
                                         AtByte tid,
                                         float time,
                                         const AtPoint* origin,
                                         const AtVector* direction,
                                         float t0,
                                         float t1);

/**
 * Volume plugin function table
 * 
 * This structure is used to report the function pointers that the
 * renderer needs to call at runtime. The version field is used for runtime
 * compatibility checking.
 */
struct AtVolumePluginVtable
{
   AtVolumePluginInit              Init;              /**< This method is called first and should initialize the volume plugin */
   AtVolumePluginCleanup           Cleanup;           /**< This method is called last and should clean up any memory used by the volume plugin */
   AtVolumePluginCreateVolume      CreateVolume;      /**< This method is called to load/create a volume */
   AtVolumePluginCleanupVolume     CleanupVolume;     /**< This method is called to clean up a volume when it's no longer needed */
   AtVolumePluginSampleDeprecated  SampleDeprecated;  /**< This method is called to sample a volume's named channel using a given point and interpolation */
   AtVolumePluginRayExtents        RayExtents;        /**< This method is called to get all tightly-bounded extents along a ray where the volume exists */
   char                            version[32];       /**< You must copy the contents of the AI_VERSION macro here */
   AtVolumePluginUpdateVolume      UpdateVolume;      /**< This method is called to update a volume before render passes or on scene changes (optional) */
   AtVolumePluginSample            Sample;            /**< This method is called to sample a volume's named channel using a given point and interpolation */
};

/**
 * Volume plugin entry-point symbol
 *
 * Volume plugin DSOs must define a symbol named "VolumePluginLoader" of this
 * type. It should fill in the fields of a \ref AtVolumePluginVtable structure.
 *
 * \param[out] vtable  List of volume plugin methods to be supplied by the user
 * \return             true upon success
 */
typedef bool (*AtVolumePluginLoader)(AtVolumePluginVtable* vtable);

/**
 * Handy shortcut for writing volume plugins.
 *
 * This is a shorter, alternate way to declare the \ref AtVolumePluginLoader symbol.
 * Here is an example:
 * \code
 * volume_plugin_loader 
 * {
 *    vtable->Init           = MyInit;
 *    vtable->Cleanup        = MyCleanup;
 *    vtable->CreateVolume   = MyCreateVolume;
 *    vtable->UpdateVolume   = MyUpdateVolume;
 *    vtable->CleanupVolume  = MyCleanupVolume;
 *    vtable->Sample         = MySample;
 *    vtable->RayExtents     = MyRayExtents;
 *    strcpy(vtable->version, AI_VERSION);
 *    return true;
 * }
 * \endcode
 */
#define volume_plugin_loader AI_EXPORT_LIB bool VolumePluginLoader(AtVolumePluginVtable* vtable)

/*\}*/
