
#ifndef FREETYPE_CONFIG_INTEGER_TYPES_H_
#define FREETYPE_CONFIG_INTEGER_TYPES_H_

#ifndef FT_CHAR_BIT
#define FT_CHAR_BIT  CHAR_BIT
#endif

#ifndef FT_SIZEOF_INT

#if                                 FT_UINT_MAX == 0xFFFFUL
#define FT_SIZEOF_INT  ( 16 / FT_CHAR_BIT )
#elif                               FT_UINT_MAX == 0xFFFFFFFFUL
#define FT_SIZEOF_INT  ( 32 / FT_CHAR_BIT )
#elif FT_UINT_MAX > 0xFFFFFFFFUL && FT_UINT_MAX == 0xFFFFFFFFFFFFFFFFUL
#define FT_SIZEOF_INT  ( 64 / FT_CHAR_BIT )
#else
#error "Unsupported size of `int' type!"
#endif

#endif

#ifndef FT_SIZEOF_LONG

#if                                  FT_ULONG_MAX == 0xFFFFFFFFUL
#define FT_SIZEOF_LONG  ( 32 / FT_CHAR_BIT )
#elif FT_ULONG_MAX > 0xFFFFFFFFUL && FT_ULONG_MAX == 0xFFFFFFFFFFUL
#define FT_SIZEOF_LONG  ( 32 / FT_CHAR_BIT )
#elif FT_ULONG_MAX > 0xFFFFFFFFUL && FT_ULONG_MAX == 0xFFFFFFFFFFFFFFFFUL
#define FT_SIZEOF_LONG  ( 64 / FT_CHAR_BIT )
#else
#error "Unsupported size of `long' type!"
#endif

#endif

#ifndef FT_SIZEOF_LONG_LONG

#if defined( FT_ULLONG_MAX ) && FT_ULLONG_MAX >= 0xFFFFFFFFFFFFFFFFULL
#define FT_SIZEOF_LONG_LONG  ( 64 / FT_CHAR_BIT )
#else
#define FT_SIZEOF_LONG_LONG  0
#endif

#endif

  typedef signed short  FT_Int16;

  typedef unsigned short  FT_UInt16;

#if 0

  typedef signed XXX  FT_Int32;

  typedef unsigned XXX  FT_UInt32;

  typedef signed XXX  FT_Int64;

  typedef unsigned XXX  FT_UInt64;

#endif

#if FT_SIZEOF_INT == ( 32 / FT_CHAR_BIT )

  typedef signed int      FT_Int32;
  typedef unsigned int    FT_UInt32;

#elif FT_SIZEOF_LONG == ( 32 / FT_CHAR_BIT )

  typedef signed long     FT_Int32;
  typedef unsigned long   FT_UInt32;

#else
#error "no 32bit type found -- please check your configuration files"
#endif

#if FT_SIZEOF_INT >= ( 32 / FT_CHAR_BIT )

  typedef int            FT_Fast;
  typedef unsigned int   FT_UFast;

#elif FT_SIZEOF_LONG >= ( 32 / FT_CHAR_BIT )

  typedef long           FT_Fast;
  typedef unsigned long  FT_UFast;

#endif

#if FT_SIZEOF_LONG == ( 64 / FT_CHAR_BIT )

#define FT_INT64   long
#define FT_UINT64  unsigned long

#elif FT_SIZEOF_LONG_LONG >= ( 64 / FT_CHAR_BIT )

#define FT_INT64   long long int
#define FT_UINT64  unsigned long long int

#elif !defined( __STDC__ ) || defined( FT_CONFIG_OPTION_FORCE_INT64 )

#if defined( _MSC_VER ) && _MSC_VER >= 900

#define FT_INT64   __int64
#define FT_UINT64  unsigned __int64

#elif defined( __BORLANDC__ )

#define FT_INT64   __int64
#define FT_UINT64  unsigned __int64

#elif defined( __WATCOMC__ ) && __WATCOMC__ >= 1100

#define FT_INT64   long long int
#define FT_UINT64  unsigned long long int

#elif defined( __MWERKS__ )

#define FT_INT64   long long int
#define FT_UINT64  unsigned long long int

#elif defined( __GNUC__ )

#define FT_INT64   long long int
#define FT_UINT64  unsigned long long int

#endif

#endif

#ifdef FT_INT64
  typedef FT_INT64   FT_Int64;
  typedef FT_UINT64  FT_UInt64;
#endif

#endif

