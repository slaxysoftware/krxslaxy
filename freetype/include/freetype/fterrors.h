
#if !( defined( FTERRORS_H_ ) && defined ( __FTERRORS_H__ ) )
#define FTERRORS_H_
#define __FTERRORS_H__

#include <freetype/ftmoderr.h>

#undef  FT_NEED_EXTERN_C

#ifndef FT_ERR_PREFIX
#define FT_ERR_PREFIX  FT_Err_
#endif

#ifdef FT_CONFIG_OPTION_USE_MODULE_ERRORS

#ifndef FT_ERR_BASE
#define FT_ERR_BASE  FT_Mod_Err_Base
#endif

#else

#undef FT_ERR_BASE
#define FT_ERR_BASE  0

#endif

#ifndef FT_ERRORDEF

#define FT_INCLUDE_ERR_PROTOS

#define FT_ERRORDEF( e, v, s )  e = v,
#define FT_ERROR_START_LIST     enum {
#define FT_ERROR_END_LIST       FT_ERR_CAT( FT_ERR_PREFIX, Max ) };

#ifdef __cplusplus
#define FT_NEED_EXTERN_C
  extern "C" {
#endif

#endif

#define FT_ERRORDEF_( e, v, s )                                             \
          FT_ERRORDEF( FT_ERR_CAT( FT_ERR_PREFIX, e ), v + FT_ERR_BASE, s )

#define FT_NOERRORDEF_( e, v, s )                             \
          FT_ERRORDEF( FT_ERR_CAT( FT_ERR_PREFIX, e ), v, s )

#ifdef FT_ERROR_START_LIST
  FT_ERROR_START_LIST
#endif

#include <freetype/fterrdef.h>

#ifdef FT_ERROR_END_LIST
  FT_ERROR_END_LIST
#endif

#ifdef FT_NEED_EXTERN_C
  }
#endif

#undef FT_ERROR_START_LIST
#undef FT_ERROR_END_LIST

#undef FT_ERRORDEF
#undef FT_ERRORDEF_
#undef FT_NOERRORDEF_

#undef FT_NEED_EXTERN_C
#undef FT_ERR_BASE

#ifndef FT2_BUILD_LIBRARY
#undef FT_ERR_PREFIX
#endif

#ifdef FT_INCLUDE_ERR_PROTOS
#undef FT_INCLUDE_ERR_PROTOS

#ifndef FT_ERR_PROTOS_DEFINED
#define FT_ERR_PROTOS_DEFINED

FT_BEGIN_HEADER

  FT_EXPORT( const char* )
  FT_Error_String( FT_Error  error_code );

FT_END_HEADER

#endif

#endif

#endif

