/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2016 Solid Angle SL
 */

/**
 * \file 
 * API for writing procedural geometry nodes
 */

#pragma once
#include "ai_version.h"

// forward declaration
struct AtBBox;
struct AtNode;

/** \defgroup ai_procedurals Procedural API
 * 
 * This API is used to create geometry procedurally at render time, rather than
 * upfront. This is accomplished by providing the renderer some callback functions
 * which are called as needed, on-demand, possibly by multiple threads, when the
 * procedural's bounding box is hit by any ray. Procedural nodes should only declare
 * geometry and perhaps shader nodes. In particular, light sources cannot be created
 * on-demand during rendering.
 *
 * Note that procedurals can recursively create other procedural nodes. This allows
 * small scene files to define huge scenes, where every piece of geometry is loaded
 * on-demand: geometry that is not visible by the camera (or by other types of rays,
 * e.g. reflection) will never be loaded in memory.
 *
 * There is a special mode, enabled by setting the procedural's load_at_init=true,
 * that will force the loading of the procedural during the pre-render initialization
 * process. In this mode, the procedural will be loaded serially (unless it was
 * created from another procedural which is being created during rendering, in which
 * case it will be executed in the current thread, possibly in parallel with others).
 * This mode can be enabled for all procedural nodes simultaneously by setting the
 * global option \c procedural_force_expand.
 *
 * So, in most cases, when rendering with multiple threads, procedural nodes can be
 * expanded in parallel. It is therefore necessary that the code in a procedural node
 * is properly designed to be re-entrant. If the code is not re-entrant, the global
 * option \c enable_threaded_procedurals can be used to prevent undefined behaviour.
 *
 * \{                                                                              
 */

AI_API void* AiProceduralGetPluginData(const AtNode* node);

/**
 * Procedural plugin init method.
 *
 * This method will be called once for the entire session in which the procedural
 * is used, no matter how many procedurals of this type there are.  If there is
 * data to be shared between procedural nodes using this procedural plugin, it
 * can be created here.  This is an optional method.
 *
 * \param[out] plugin_user_ptr  This is a general-purpose, user-supplied data
 *                              pointer that later can be accessed via
 *                              \ref AiProceduralGetPluginData.
 * \return                      true upon success
 */
typedef bool (*AtProcInitPlugin)(void** plugin_user_ptr);

/**
 * Procedural plugin cleanup method.
 *
 * This method will be called once at the end of the entire session in which the
 * procedural is used, no matter how many procedurals of this type there are.
 * If there was data shared between procedural nodes using this plugin, it
 * should be destroyed here.  This is an optional method.
 *
 * \param plugin_user_ptr  This is a general-purpose, user-supplied data
 *                         pointer that came from \ref AtProcInitPlugin.
 * \return                 true upon success
 */
typedef bool (*AtProcCleanupPlugin)(void* plugin_user_ptr);

/**
 * Procedural init bounds method.
 * 
 * This method will be called when a procedural node is initialized, to compute
 * bounds at runtime. This is an optional method and if not provided the bounds
 * specified in the node min and max parameters will be used. If this method is
 * provided it can override the bounds.
 *
 * The user-supplied data pointer will be passed along to the other procedural
 * methods, and can store data that is re-used when the procedural is expanded,
 * to avoid doing the same computations twice.
 *
 * Since the procedural may never be expanded, this method should not create any
 * nodes and should execute quickly, with the heavier computations performed in
 * \ref AtProcInit.
 *
 * This method may be called concurrently with other uses of the same procedural
 * plugin, unless "options.enable_threaded_procedurals" is off.
 *
 * \param      node      This is the procedural node itself
 * \param      bounds    Runtime computed bounds of the procedural
 * \param[out] user_ptr  This is a general-purpose, user-supplied data pointer that
 *                       Arnold will pass along to the other procedural methods.
 * \return               true upon success
 */
typedef bool (*AtProcInitBounds)(AtNode* node, AtBBox* bounds, void** user_ptr);

/**
 * Procedural init method.
 * 
 * This method will be called first and should perform any initialization required
 * by your procedural. You probably want to create new nodes inside this method
 * but you should return them through \ref AtProcGetNode and correctly return
 * the number of created nodes from \ref AtProcNumNodes, otherwise the behavior
 * is undefined. Alternatively, if you know ahead of time exactly how many nodes
 * you are going to create, you can create them in \ref AtProcGetNode too.
 *
 * This method may be called concurrently with other uses of the same procedural
 * plugin, unless "options.enable_threaded_procedurals" is off.
 *
 * \param      node      This is the procedural node itself
 * \param[out] user_ptr  This is a general-purpose, user-supplied data pointer that
 *                       Arnold will pass along to the other procedural methods.
 * \return               true upon success
 */
typedef int (*AtProcInit)(AtNode* node, void** user_ptr);

/**
 * Procedural cleanup method.
 * 
 * This method will be called last and should perform any cleanup required
 * by your procedural. Make sure you release any memory you allocated that is no
 * longer needed by Arnold.
 *
 * This method may be called concurrently with other uses of the same procedural
 * plugin, unless "options.enable_threaded_procedurals" is off.
 *
 * \param user_ptr  User data pointer, as returned from \ref AtProcInit
 * \return          true upon success
 */
typedef int (*AtProcCleanup)(void* user_ptr);

/**
 * Procedural node count method.
 * 
 * This method will be called after initialization and should report the exact
 * number of nodes to be created.
 *
 * This method may be called concurrently with other uses of the same procedural
 * plugin, unless "options.enable_threaded_procedurals" is off.
 *
 * \param user_ptr  User data pointer, as returned from \ref AtProcInit
 * \return          The number of nodes in the procedural
 */
typedef int (*AtProcNumNodes)(void* user_ptr);

/**
 * Procedural node fetching method.
 * 
 * This method will be called once for each node to be created (as determined by
 * \ref AtProcNumNodes). Note that if you created any node in \ref AtProcInit, they
 * also should be returned here, otherwise the behaviour would be undefined.
 *
 * This method may be called concurrently with other uses of the same procedural
 * plugin, unless "options.enable_threaded_procedurals" is off.
 *
 * \param user_ptr  User data pointer, as returned from \ref AtProcInit
 * \param i         Node index, in the range 0 to \ref AtProcNumNodes - 1
 * \return          The i'th node in the procedural
 */
typedef AtNode* (*AtProcGetNode)(void* user_ptr, int i);

/**
 * Procedural function table
 * 
 * This structure is used to report the function pointers that the renderer needs
 * to call at runtime. The version field is used for runtime compatibility checking.
 * These fields are pre-initialized to NULL when passed to ProcLoader.
 */
typedef struct AtProcVtable {
   AtProcInit          Init;                         /**< This is called before expanding the procedural */
   AtProcCleanup       Cleanup;                      /**< This is called last and should clean up any (temporary) memory used by the procedural */
   AtProcNumNodes      NumNodes;                     /**< This is called to find out how many nodes this procedural will generate */
   AtProcGetNode       GetNode;                      /**< This is called NumNodes times, once for each node the procedural creates */
   char                version[AI_MAXSIZE_VERSION];  /**< You must copy the contents of the AI_VERSION macro here */
   AtProcInitPlugin    InitPlugin;                   /**< This is called once for the plugin for the entire session (optional) */
   AtProcCleanupPlugin CleanupPlugin;                /**< this is called once for the plugin at the end of the entire session (optional) */
   AtProcInitBounds    InitBounds;                   /**< This is called on node initialization to compute bounds at runtime (optional) */
} AtProcVtable;

/**
 * Procedural entry-point symbol
 *
 * Procedural DSO's must define a symbol named "ProcLoader" of this
 * type. It should fill in the fields of a \ref AtProcVtable structure.
 *
 * \param[out] vtable  List of procedural methods (some of which are optional) to be supplied by the user
 * \return             true upon success
 */
typedef int (*AtProcLoader)(AtProcVtable* vtable);

/**
 * Handy shortcut for writing procedurals.
 *
 * This is a shorter, alternate way to declare the \ref AtProcLoader symbol.
 * Here is an example:
 * \code
 * node_loader 
 * {
 *    vtable->InitPlugin    = MyInitPlugin;
 *    vtable->CleanupPlugin = MyCleanupPlugin;
 *    vtable->InitBounds    = MyInitBounds;
 *    vtable->Init          = MyInit;
 *    vtable->Cleanup       = MyCleanup;
 *    vtable->NumNodes      = MyNumNodes;
 *    vtable->GetNode       = MyGetNode;
 *    strcpy(vtable->version, AI_VERSION);
 *    return true;
 * }
 * \endcode
 */
#define proc_loader AI_EXPORT_LIB int ProcLoader(AtProcVtable* vtable)

/*\}*/
