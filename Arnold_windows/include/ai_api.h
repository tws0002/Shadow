/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2016 Solid Angle SL
 */

/**
 * \file
 * DLL export prefix for API functions (necessary for multi-platform development)
 */

#pragma once

#ifdef __cplusplus
#define AI_EXTERN_C extern "C"
#else
#define AI_EXTERN_C extern
#endif

#if _MSC_VER && !__INTEL_COMPILER
// currently the microsoft compiler does not support these annotations
# define AI_PURE_ATTRIBUTE
# define AI_CONST_ATTRIBUTE
#else
// gcc, clang, and icc are ok with these
# define AI_PURE_ATTRIBUTE  pure
# define AI_CONST_ATTRIBUTE const
#endif

#ifdef _WIN32
/*
 * Public functions need a special declaration in Win32
 */
# ifdef _ARNOLDDLL
#  define AI_DEPRECATED      __declspec(deprecated)
#  define AI_API AI_EXTERN_C __declspec(dllexport)
# else
#  define AI_DEPRECATED
#  define AI_API AI_EXTERN_C __declspec(dllimport)
# endif
# define AI_PURE             __declspec(AI_PURE_ATTRIBUTE)
# define AI_CONST            __declspec(AI_CONST_ATTRIBUTE)
# define AI_UNAVAILABLE      __declspec(deprecated)
#else
# ifdef _ARNOLDDLL
#  define AI_DEPRECATED      __attribute__(( deprecated ))
# else
#  define AI_DEPRECATED
# endif
# define AI_API  AI_EXTERN_C __attribute__(( visibility("default") ))
# define AI_PURE             __attribute__(( AI_PURE_ATTRIBUTE ))
# define AI_CONST            __attribute__(( AI_CONST_ATTRIBUTE ))
# ifdef __clang__
#  define AI_UNAVAILABLE __attribute__((unavailable))
# elif __GNUC__ >= 4 && __GNUC_MINOR__ >= 3 && !__INTEL_COMPILER
#  define AI_UNAVAILABLE __attribute__((error("This function is not allowed to be used")))
# else
   // Using deprecated isn't quite strong enough, but it's the best we can do in this compiler
#  define AI_UNAVAILABLE __attribute__((deprecated))
# endif
#endif
