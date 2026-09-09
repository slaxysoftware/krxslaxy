
#ifndef FREETYPE_CONFIG_PUBLIC_MACROS_H_
#define FREETYPE_CONFIG_PUBLIC_MACROS_H_

#ifndef FT_BEGIN_HEADER
#ifdef __cplusplus
#define FT_BEGIN_HEADER  extern "C" {
#else
#define FT_BEGIN_HEADER
#endif
#endif

#ifndef FT_END_HEADER
#ifdef __cplusplus
#define FT_END_HEADER  }
#else
#define FT_END_HEADER
#endif
#endif

FT_BEGIN_HEADER

#if defined( _WIN32 )

#if defined( FT2_BUILD_LIBRARY ) && defined( DLL_EXPORT )
#define FT_PUBLIC_FUNCTION_ATTRIBUTE  __declspec( dllexport )
#elif defined( DLL_IMPORT )
#define FT_PUBLIC_FUNCTION_ATTRIBUTE  __declspec( dllimport )
#endif

#elif ( defined( __GNUC__ ) && __GNUC__ >= 4 ) || defined( __clang__ )
#define FT_PUBLIC_FUNCTION_ATTRIBUTE \
          __attribute__(( visibility( "default" ) ))

#elif defined( __SUNPRO_C ) && __SUNPRO_C >= 0x550
#define FT_PUBLIC_FUNCTION_ATTRIBUTE  __global
#endif

#ifndef FT_PUBLIC_FUNCTION_ATTRIBUTE
#define FT_PUBLIC_FUNCTION_ATTRIBUTE
#endif

#define FT_EXPORT( x )  FT_PUBLIC_FUNCTION_ATTRIBUTE extern x

#ifndef FT_UNUSED
#define FT_UNUSED( arg )  ( (arg) = (arg) )
#endif

#ifdef __cplusplus
#define FT_STATIC_CAST( type, var )       static_cast<type>(var)
#define FT_REINTERPRET_CAST( type, var )  reinterpret_cast<type>(var)

#define FT_STATIC_BYTE_CAST( type, var )                         \
          static_cast<type>( static_cast<unsigned char>( var ) )
#else
#define FT_STATIC_CAST( type, var )       (type)(var)
#define FT_REINTERPRET_CAST( type, var )  (type)(var)

#define FT_STATIC_BYTE_CAST( type, var )  (type)(unsigned char)(var)
#endif

FT_END_HEADER

#endif

