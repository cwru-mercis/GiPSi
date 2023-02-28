
// -*- C++ -*-
// quoter_common_export.h,v 1.2 2004/01/15 16:10:56 jwillemsen Exp
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl
// ------------------------------
#ifndef HAPTICS_COMMON_EXPORT_H
#define HAPTICS_COMMON_EXPORT_H

#include "ace/config-all.h"

#if defined (TAO_AS_STATIC_LIBS)
#  if !defined (HAPTICS_COMMON_HAS_DLL)
#    define HAPTICS_COMMON_HAS_DLL 0
#  endif /* ! HAPTICS_COMMON_HAS_DLL */
#else
#  if !defined (HAPTICS_COMMON_HAS_DLL)
#    define HAPTICS_COMMON_HAS_DLL 1
#  endif /* ! HAPTICS_COMMON_HAS_DLL */
#endif

#if defined (HAPTICS_COMMON_HAS_DLL) && (HAPTICS_COMMON_HAS_DLL == 1)
#  if defined (HAPTICS_COMMON_BUILD_DLL)
#    define HAPTICS_COMMON_Export ACE_Proper_Export_Flag
#    define HAPTICS_COMMON_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define HAPTICS_COMMON_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* HAPTICS_COMMON_BUILD_DLL */
#    define HAPTICS_COMMON_Export ACE_Proper_Import_Flag
#    define HAPTICS_COMMON_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define HAPTICS_COMMON_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* HAPTICS_COMMON_BUILD_DLL */
#else /* HAPTICS_COMMON_HAS_DLL == 1 */
#  define HAPTICS_COMMON_Export
#  define HAPTICS_COMMON_SINGLETON_DECLARATION(T)
#  define HAPTICS_COMMON_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* HAPTICS_COMMON_HAS_DLL == 1 */

#endif /* HAPTICS_COMMON_EXPORT_H */

// End of auto generated file.
