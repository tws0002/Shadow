/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2016 Solid Angle SL
 */

/**
 * \file 
 * Various BRDF functions available to shader writers
 */

#pragma once
#include "ai_color.h"
#include "ai_vector.h"
#include "ai_shaderglobals.h"

/** \defgroup ai_shader_brdf BRDF API
 * \{
 */

/** \name BRDF Evaluation Methods
 *
 * \details
 * We provide an interface for users to define a BRDF and its
 * associated methods. All the functions receive an arbitrary
 * void pointer where the programmer can store needed parameters.
 * An "instance" of a BRDF is meant to be located at a certain
 * point and to a certain viewing direction. The programmer must
 * take care of this and store it in the arbitrary data pointer.
 * \{
 */
/** Returns a direction according to some distribution based on the reflectance of the BRDF. */
typedef AtVector(*AtBRDFEvalSampleFunc)(const void* brdf_data, float rx, float ry);
/** Returns the actual reflectance from a given direction to the viewer's direction. */
typedef AtColor (*AtBRDFEvalBrdfFunc)(const void* brdf_data, const AtVector* indir);
/** Returns the probability density for the above sample function at a certain incoming direction. */
typedef float   (*AtBRDFEvalPdfFunc)(const void* brdf_data, const AtVector* indir);
/* \}*/

/** \name BRDF Integration
 * \{
 */
AI_API AtColor AiEvaluateLightSample(AtShaderGlobals* sg, const void* brdf_data, AtBRDFEvalSampleFunc eval_sample, AtBRDFEvalBrdfFunc eval_brdf, AtBRDFEvalPdfFunc eval_pdf);
inline AtColor AiBRDFIntegrate(AtShaderGlobals *sg, const void *brdf_data, AtBRDFEvalSampleFunc eval_sample, AtBRDFEvalBrdfFunc eval_brdf, AtBRDFEvalPdfFunc eval_pdf, AtUInt16 ray_type, const AtRGB& weight);
/* \}*/

/** \name Pre-defined BRDFs
 * \{
 */
AI_API AI_PURE AtColor  AiOrenNayarMISBRDF(const void* brdf_data, const AtVector* indir);
AI_API AI_PURE float    AiOrenNayarMISPDF(const void* brdf_data, const AtVector* indir);
AI_API AI_PURE AtVector AiOrenNayarMISSample(const void* brdf_data, float randx, float randy);
AI_API         void*    AiOrenNayarMISCreateData(const AtShaderGlobals* sg, float r);

AI_API AI_PURE AtColor  AiMicrofacetMISBRDF(const void* brdf_data, const AtVector* indir);
AI_API AI_PURE float    AiMicrofacetMISPDF(const void* brdf_data, const AtVector* indir);
AI_API AI_PURE AtVector AiMicrofacetMISSample(const void* brdf_data, float randx, float randy);
AI_API AI_PURE AtColor  AiMicrofacetMISAverageFresnel(const AtShaderGlobals* sg, const void* brdf_data);
AI_API         void*    AiMicrofacetMISCreateData(const AtShaderGlobals* sg, int distribution, const AtVector* u, float eta, float rx, float ry);

AI_API AI_PURE AtColor  AiWardDuerMISBRDF(const void* brdf_data, const AtVector* indir);
AI_API AI_PURE float    AiWardDuerMISPDF(const void* brdf_data, const AtVector* indir);
AI_API AI_PURE AtVector AiWardDuerMISSample(const void* brdf_data, float randx, float randy);
AI_API         void*    AiWardDuerMISCreateData(const AtShaderGlobals* sg, const AtVector* u, const AtVector* v, float rx, float ry);

AI_API AI_PURE AtColor  AiAshikhminShirleyMISBRDF(const void* brdf_data, const AtVector* indir);
AI_API AI_PURE float    AiAshikhminShirleyMISPDF(const void* brdf_data, const AtVector* indir);
AI_API AI_PURE AtVector AiAshikhminShirleyMISSample(const void* brdf_data, float randx, float randy);
AI_API         void*    AiAshikhminShirleyMISCreateData(const AtShaderGlobals* sg, const AtVector* u, const AtVector* v, float rx, float ry);

AI_API AI_PURE AtColor  AiStretchedPhongMISBRDF(const void* brdf_data, const AtVector* indir);
AI_API AI_PURE float    AiStretchedPhongMISPDF(const void* brdf_data, const AtVector* indir);
AI_API AI_PURE AtVector AiStretchedPhongMISSample(const void* brdf_data, float randx, float randy);
AI_API         void*    AiStretchedPhongMISCreateData(const AtShaderGlobals* sg, float pexp);
/* \}*/

/** \name Microfacet distributions
 * \{
 */
#define AI_MICROFACET_BECKMANN   0x00  /**< Beckmann distribution (used by Cook-Torrance) */
#define AI_MICROFACET_GGX        0x01  /**< GGX distribution */
/*\}*/


/** \name Deprecated BRDF Evaluation API
 * \{
 */
AI_API AI_PURE float AiOrenNayarBRDF       (const AtVector* L, const AtVector* V, const AtVector* N, float r);
AI_API AI_PURE float AiLommelSeeligerBRDF  (const AtVector* L, const AtVector* V, const AtVector* N);
AI_API AI_PURE float AiCookTorranceBRDF    (const AtVector* L, const AtVector* V, const AtVector* N, const AtVector* u, const AtVector* v, float rx, float ry);
AI_API AI_PURE float AiWardBRDF            (const AtVector* L, const AtVector* V, const AtVector* N, const AtVector* u, const AtVector* v, float rx, float ry);
AI_API AI_PURE float AiWardDuerBRDF        (const AtVector* L, const AtVector* V, const AtVector* N, const AtVector* u, const AtVector* v, float rx, float ry);
AI_API AI_PURE float AiAshikhminShirleyBRDF(const AtVector* L, const AtVector* V, const AtVector* N, const AtVector* u, const AtVector* v, float nx, float ny);
AI_API AI_PURE float AiStretchedPhongBRDF  (const AtVector* L, const AtVector* V, const AtVector* N, float n, bool retro = false);
/* \}*/

/** \name Deprecated BRDF Integration API
 * \{
 */
inline AtColor AiOrenNayarIntegrate(const AtVector* N, AtShaderGlobals* sg, float r, const AtRGB& weight);
AI_API AtColor AiLommelSeeligerIntegrate(const AtVector* N, AtShaderGlobals* sg);
AI_API AtColor AiCookTorranceIntegrate(const AtVector* N, AtShaderGlobals* sg, const AtVector* u, const AtVector* v, float rx, float ry);
AI_API AtColor AiWardIntegrate(const AtVector* N, AtShaderGlobals* sg, const AtVector* u, const AtVector* v, float rx, float ry);
AI_API AtColor AiWardDuerIntegrate(const AtVector* N, AtShaderGlobals* sg, const AtVector* u, const AtVector* v, float rx, float ry);
AI_API AtColor AiAshikhminShirleyIntegrate(const AtVector* N, AtShaderGlobals* sg, const AtVector* u, const AtVector* v, float nx, float ny);
inline AtColor AiMicrofacetBTDFIntegrate(const AtVector* N, AtShaderGlobals* sg, int distribution, const AtVector* u, float rx, float ry, float eta, bool entering, float dispersion, bool fresnel, AtColor transmittance, const AtColor& weight);
AI_API AtColor AiStretchedPhongIntegrate(const AtVector* N, AtShaderGlobals* sg, float exp);
/* \}*/

/** \name Deprecated BRDFs
 * \{
 */
AI_API AI_PURE AtColor  AiCookTorranceMISBRDF(const void* brdf_data, const AtVector* indir);
AI_API AI_PURE float    AiCookTorranceMISPDF(const void* brdf_data, const AtVector* indir);
AI_API AI_PURE AtVector AiCookTorranceMISSample(const void* brdf_data, float randx, float randy);
AI_API         void*    AiCookTorranceMISCreateData(const AtShaderGlobals* sg, const AtVector* u, const AtVector* v, float rx, float ry);
/* \}*/

/*\}*/

// Backwards Compatibility
/// @cond internal
AI_API AtColor AiBRDFIntegrate(AtShaderGlobals *sg, const void *brdf_data, AtBRDFEvalSampleFunc eval_sample, AtBRDFEvalBrdfFunc eval_brdf, AtBRDFEvalPdfFunc eval_pdf, AtUInt16 ray_type);
AI_API AtColor AiBRDFIntegrateWeighted(AtShaderGlobals *sg, const void *brdf_data, AtBRDFEvalSampleFunc eval_sample, AtBRDFEvalBrdfFunc eval_brdf, AtBRDFEvalPdfFunc eval_pdf, AtUInt16 ray_type, const AtRGB& weight);
inline AtColor AiBRDFIntegrate(AtShaderGlobals *sg, const void *brdf_data, AtBRDFEvalSampleFunc eval_sample, AtBRDFEvalBrdfFunc eval_brdf, AtBRDFEvalPdfFunc eval_pdf, AtUInt16 ray_type, const AtRGB& weight)
{ return AiBRDFIntegrateWeighted(sg, brdf_data, eval_sample, eval_brdf, eval_pdf, ray_type, weight); }
AI_API AtColor AiOrenNayarIntegrate(const AtVector* N, AtShaderGlobals* sg, float r);
AI_API AtColor AiOrenNayarIntegrateWeighted(const AtVector* N, AtShaderGlobals* sg, float r, const AtRGB& weight);
inline AtColor AiOrenNayarIntegrate(const AtVector* N, AtShaderGlobals* sg, float r, const AtRGB& weight)
{ return AiOrenNayarIntegrateWeighted(N, sg, r, weight); }
AI_API AtColor AiMicrofacetBTDFIntegrate(const AtVector* N, AtShaderGlobals* sg, const AtVector* u, const AtVector* v, float rx, float ry, float eta_i, float eta_o, AtColor transmittance);
AI_API AtColor AiMicrofacetBTDFIntegrateWeighted(const AtVector* N, AtShaderGlobals* sg, int distribution, const AtVector* u, float rx, float ry, float eta, bool entering, float dispersion, bool fresnel, AtColor transmittance, const AtColor& weight);
inline AtColor AiMicrofacetBTDFIntegrate(const AtVector* N, AtShaderGlobals* sg, int distribution, const AtVector* u, float rx, float ry, float eta, bool entering, float dispersion, bool fresnel, AtColor transmittance, const AtColor& weight)
{ return AiMicrofacetBTDFIntegrateWeighted(N, sg, distribution, u, rx, ry, eta, entering, dispersion, fresnel, transmittance, weight); }
/// @endcond
