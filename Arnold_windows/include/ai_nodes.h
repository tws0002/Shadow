/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2016 Solid Angle SL
 */

/**
 * \file 
 * AtNode struct and methods
 */

#pragma once
#include "ai_node_entry.h"
#include "ai_params.h"
#include "ai_api.h"
#include "ai_string.h"

// forward declarations
struct AtList;
struct AtMetaDataStore;
struct AtNodeEntry;
struct AtUserParamEntry;
struct AtUserParamIterator;

/** \defgroup ai_nodes AtNode API
 *
 *  Arnold has a pluggable node-management system for the standard primitives
 *  such as lights, cameras, geometry, shaders, filters, drivers, etc.  Each
 *  node type has a set of associated methods (member functions) and
 *  parameters.   Like C++'s objects, Arnold's node system allows new nodes to
 *  inherit both parameters and methods from the "parent" node.  For example,
 *  all nodes have a "name" parameter and a "node_initialize{}" method (among
 *  others) because they are declared in the base node from which all nodes
 *  inherit.  
 *
 *  In addition to these standard methods and parameters, derived nodes may
 *  have their own type-specific parameters and methods.  For example, all
 *  shaders have a "shader_evaluate{}" method and all driver nodes have a
 *  "driver_write_bucket{}" method (among others).  User-derived nodes may have their
 *  own new parameters but are not permitted to add new methods.
 *
 *  Two of the important data-structures in this object-oriented-like system are:
 *  \li  AtNodeEntry -- this contains the "description" of a particular node type 
 *                      ("polymesh", "persp_camera", etc.)
 *  \li  AtNode -- this contains the "instantiation" of a particular node type
 * \{
 */

/**
 * \struct AtNode
 * 
 * This represents a node in Arnold.
 */
struct AtNode;

/**
 * \name Node Method Declarations
 * \{
 */

/** Parameter declaration method */
#define node_parameters \
static void Parameters(AtList* params, AtMetaDataStore* mds)

/** Node initialization method */
#define node_initialize \
static void Initialize(AtNode* node, AtParamValue* params)

/** Node update method */
#define node_update \
static void Update(AtNode* node, AtParamValue* params)

/** Node de-initialization method */
#define node_finish \
static void Finish(AtNode* node)

/** Node loading method (for plugin nodes in dynamic libraries) */
#define node_loader \
 AI_EXPORT_LIB bool NodeLoader(int i, AtNodeLib* node)
/* \}*/

/** Exporter for common methods */
#define AI_INSTANCE_COMMON_METHODS         \
node_parameters;                           \
node_initialize;                           \
node_update;                               \
node_finish;                               \
static AtCommonMethods ai_common_mtds = {  \
   Parameters,                             \
   Initialize,                             \
   Update,                                 \
   Finish                                  \
};

/**
 * \name AtNode Methods
 * \{
 */
AI_API AI_DEPRECATED AtNode*   AiNode                    (const char* name);
AI_API               AtNode*   AiNodeAtString            (const AtString name);
inline               AtNode*   AiNode                    (const AtString name) { return AiNodeAtString(name); }

AI_API AI_DEPRECATED AI_PURE AtNode* AiNodeLookUpByName        (const char* name);
AI_API AI_PURE               AtNode* AiNodeLookUpByNameAtString(const AtString name);
AI_PURE inline               AtNode* AiNodeLookUpByName        (const AtString name) { return AiNodeLookUpByNameAtString(name); }

AI_API void                    AiNodeReset               (AtNode* node);
AI_API void                    AiNodeResetParameter      (AtNode* node, const char* param);
AI_API AtNode*                 AiNodeClone               (const AtNode* node);
AI_API bool                    AiNodeDestroy             (AtNode* node);
AI_API AI_PURE bool            AiNodeIs                  (const AtNode* node, const char* str);

AI_API AI_DEPRECATED bool      AiNodeDeclare             (AtNode* node, const char* param, const char* declaration);
AI_API               bool      AiNodeDeclareAtString     (AtNode* node, const AtString param, const char* declaration);
inline               bool      AiNodeDeclare             (AtNode* node, const AtString param, const char* declaration) { return AiNodeDeclareAtString(node, param, declaration); }

AI_API bool                    AiNodeLink                (AtNode* src,  const char* input, AtNode* target);
AI_API bool                    AiNodeLinkOutput          (AtNode* src,  const char* output, AtNode* target, const char* input);
AI_API bool                    AiNodeUnlink              (AtNode* node, const char* input);
AI_API AI_PURE bool            AiNodeIsLinked            (const AtNode* node, const char* input);
AI_API AI_PURE AtNode*         AiNodeGetLink             (const AtNode* node, const char* input, int* comp = NULL);
AI_API AI_PURE const char*     AiNodeGetName             (const AtNode* node);
AI_API AI_PURE const AtNodeEntry* AiNodeGetNodeEntry        (const AtNode* node);
AI_API AI_PURE AtParamValue*   AiNodeGetParams           (const AtNode* node);
AI_API AI_PURE void*           AiNodeGetLocalData        (const AtNode* node);
AI_API void                    AiNodeSetLocalData        (AtNode* node, void* data);
AI_API void                    AiNodeSetDisabled         (AtNode* node, bool disabled);
AI_API AI_PURE bool            AiNodeIsDisabled          (const AtNode* node);
AI_API AI_PURE AtNode*         AiNodeGetParent           (const AtNode* node);

AI_API AI_DEPRECATED AI_PURE const AtUserParamEntry* AiNodeLookUpUserParameter         (const AtNode* node, const char* param);
AI_API               AI_PURE const AtUserParamEntry* AiNodeLookUpUserParameterAtString (const AtNode* node, const AtString param);
              AI_PURE inline const AtUserParamEntry* AiNodeLookUpUserParameter         (const AtNode* node, const AtString param) { return AiNodeLookUpUserParameterAtString(node, param); }

AI_API AtUserParamIterator*    AiNodeGetUserParamIterator(const AtNode* node);
/* \}*/

/**
 * \name AtUserParamIterator Methods
 * \{
 */
AI_API void                    AiUserParamIteratorDestroy(AtUserParamIterator* iter);
AI_API const AtUserParamEntry* AiUserParamIteratorGetNext(AtUserParamIterator* iter);
AI_API bool                    AiUserParamIteratorFinished(const AtUserParamIterator* iter);
/* \}*/

/**
 * \name Parameter Writers
 * \{
 */
#define AiCreateFuncs(_name, _type)                                                                  \
AI_API AI_DEPRECATED void AiNodeSet##_name          (AtNode* node, const char* param,    _type val); \
AI_API               void AiNodeSet##_name##AtString(AtNode* node, const AtString param, _type val); \
inline               void AiNodeSet##_name          (AtNode* node, const AtString param, _type val)  \
{                                                                                                    \
   AiNodeSet##_name##AtString(node, param, val);                                                     \
}

AiCreateFuncs(Byte, AtByte)
AiCreateFuncs(Int, int)
AiCreateFuncs(UInt, unsigned int)
AiCreateFuncs(Bool, bool)
AiCreateFuncs(Flt, float)
AiCreateFuncs(Ptr, void*)
AiCreateFuncs(Array, AtArray*)
AiCreateFuncs(Matrix, AtMatrix)
#undef AiCreateFuncs

AI_API AI_DEPRECATED void AiNodeSetStr        (AtNode* node, const char* param,    const char* str);
AI_API               void AiNodeSetStrAtString(AtNode* node, const AtString param, const AtString str);
inline               void AiNodeSetStr        (AtNode* node, const AtString param, const AtString str) { AiNodeSetStrAtString(node, AtString(param), AtString(str)); }

AI_API AI_DEPRECATED void AiNodeSetRGB        (AtNode* node, const char* param,    float r, float g, float b);
AI_API               void AiNodeSetRGBAtString(AtNode* node, const AtString param, float r, float g, float b);
inline               void AiNodeSetRGB        (AtNode* node, const AtString param, float r, float g, float b) { AiNodeSetRGBAtString(node, param, r, g, b); }

AI_API AI_DEPRECATED void AiNodeSetRGBA        (AtNode* node, const char* param,    float r, float g, float b, float a);
AI_API               void AiNodeSetRGBAAtString(AtNode* node, const AtString param, float r, float g, float b, float a);
inline               void AiNodeSetRGBA        (AtNode* node, const AtString param, float r, float g, float b, float a) { AiNodeSetRGBAAtString(node, param, r, g, b, a); }

AI_API AI_DEPRECATED void AiNodeSetVec        (AtNode* node, const char* param,    float x, float y, float z);
AI_API               void AiNodeSetVecAtString(AtNode* node, const AtString param, float x, float y, float z);
inline               void AiNodeSetVec        (AtNode* node, const AtString param, float x, float y, float z) { AiNodeSetVecAtString(node, param, x, y, z); }

AI_API AI_DEPRECATED void AiNodeSetPnt        (AtNode* node, const char* param,    float x, float y, float z);
AI_API               void AiNodeSetPntAtString(AtNode* node, const AtString param, float x, float y, float z);
inline               void AiNodeSetPnt        (AtNode* node, const AtString param, float x, float y, float z) { AiNodeSetPntAtString(node, param, x, y, z); }

AI_API AI_DEPRECATED void AiNodeSetPnt2        (AtNode* node, const char* param,    float x, float y);
AI_API               void AiNodeSetPnt2AtString(AtNode* node, const AtString param, float x, float y);
inline               void AiNodeSetPnt2        (AtNode* node, const AtString param, float x, float y) { AiNodeSetPnt2AtString(node, param, x, y); }

AI_API void AiNodeSetAttributes(AtNode *node, const char* attributes);
/* \}*/

/**
 * \name Parameter Readers
 * \{
 */
#define AiCreateFuncs(_name, _type)                                                              \
AI_API AI_DEPRECATED _type AiNodeGet##_name             (const AtNode* node, const char* param); \
AI_API               _type AiNodeGet##_name##AtString(const AtNode* node, const AtString param); \
inline               _type AiNodeGet##_name          (const AtNode* node, const AtString param)  \
{                                                                                                \
   return AiNodeGet##_name##AtString(node, param);                                               \
}
AiCreateFuncs(Byte, AtByte)
AiCreateFuncs(Int, int)
AiCreateFuncs(UInt, unsigned int)
AiCreateFuncs(Bool, bool)
AiCreateFuncs(Flt, float)
AiCreateFuncs(RGB, AtRGB)
AiCreateFuncs(RGBA, AtRGBA)
AiCreateFuncs(Vec, AtVector)
AiCreateFuncs(Pnt, AtPoint)
AiCreateFuncs(Pnt2, AtPoint2)
//AiCreateFuncs(Str, const char*)
AiCreateFuncs(Ptr, void*)
AiCreateFuncs(Array, AtArray*)
#undef AiCreateFuncs

#ifdef __clang__
# pragma GCC diagnostic ignored "-Wreturn-type-c-linkage"
#elif defined (_MSC_VER)
#pragma warning( disable : 4190 )
#endif
AI_API AI_DEPRECATED const char* AiNodeGetStr        (const AtNode* node, const char* param);
AI_API                  AtString AiNodeGetStrAtString(const AtNode* node, const AtString param);
inline                  AtString AiNodeGetStr        (const AtNode* node, const AtString param)
{
   return AiNodeGetStrAtString(node, param);
}

AI_API AI_DEPRECATED void AiNodeGetMatrix        (const AtNode* node, const char* param, AtMatrix matrix);
AI_API               void AiNodeGetMatrixAtString(const AtNode* node, const AtString param, AtMatrix matrix);
inline               void AiNodeGetMatrix        (const AtNode* node, const AtString param, AtMatrix matrix) { AiNodeGetMatrixAtString(node, param, matrix); }
/* \}*/

/*\}*/
