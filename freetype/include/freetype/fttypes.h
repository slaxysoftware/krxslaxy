
#ifndef FTTYPES_H_
#define FTTYPES_H_

#include <ft2build.h>
#include FT_CONFIG_CONFIG_H
#include <freetype/ftsystem.h>
#include <freetype/ftimage.h>

#include <stddef.h>

FT_BEGIN_HEADER

  typedef unsigned char  FT_Bool;

  typedef signed short  FT_FWord;

  typedef unsigned short  FT_UFWord;

  typedef signed char  FT_Char;

  typedef unsigned char  FT_Byte;

  typedef const FT_Byte*  FT_Bytes;

  typedef FT_UInt32  FT_Tag;

  typedef char  FT_String;

  typedef signed short  FT_Short;

  typedef unsigned short  FT_UShort;

  typedef signed int  FT_Int;

  typedef unsigned int  FT_UInt;

  typedef signed long  FT_Long;

  typedef unsigned long  FT_ULong;

  typedef signed short  FT_F2Dot14;

  typedef signed long  FT_F26Dot6;

  typedef signed long  FT_Fixed;

  typedef int  FT_Error;

  typedef void*  FT_Pointer;

  typedef size_t  FT_Offset;

  typedef ft_ptrdiff_t  FT_PtrDist;

  typedef struct  FT_UnitVector_
  {
    FT_F2Dot14  x;
    FT_F2Dot14  y;

  } FT_UnitVector;

  typedef struct  FT_Matrix_
  {
    FT_Fixed  xx, xy;
    FT_Fixed  yx, yy;

  } FT_Matrix;

  typedef struct  FT_Data_
  {
    const FT_Byte*  pointer;
    FT_UInt         length;

  } FT_Data;

  typedef void  (*FT_Generic_Finalizer)( void*  object );

  typedef struct  FT_Generic_
  {
    void*                 data;
    FT_Generic_Finalizer  finalizer;

  } FT_Generic;

#define FT_MAKE_TAG( _x1, _x2, _x3, _x4 )                  \
          ( ( FT_STATIC_BYTE_CAST( FT_Tag, _x1 ) << 24 ) | \
            ( FT_STATIC_BYTE_CAST( FT_Tag, _x2 ) << 16 ) | \
            ( FT_STATIC_BYTE_CAST( FT_Tag, _x3 ) <<  8 ) | \
              FT_STATIC_BYTE_CAST( FT_Tag, _x4 )         )

  typedef struct FT_ListNodeRec_*  FT_ListNode;

  typedef struct FT_ListRec_*  FT_List;

  typedef struct  FT_ListNodeRec_
  {
    FT_ListNode  prev;
    FT_ListNode  next;
    void*        data;

  } FT_ListNodeRec;

  typedef struct  FT_ListRec_
  {
    FT_ListNode  head;
    FT_ListNode  tail;

  } FT_ListRec;

#define FT_IS_EMPTY( list )  ( (list).head == 0 )
#define FT_BOOL( x )         FT_STATIC_CAST( FT_Bool, (x) != 0 )

#define FT_ERR_XCAT( x, y )  x ## y
#define FT_ERR_CAT( x, y )   FT_ERR_XCAT( x, y )

#define FT_ERR( e )  FT_ERR_CAT( FT_ERR_PREFIX, e )

#define FT_ERROR_BASE( x )    ( (x) & 0xFF )
#define FT_ERROR_MODULE( x )  ( (x) & 0xFF00U )

#define FT_ERR_EQ( x, e )                                        \
          ( FT_ERROR_BASE( x ) == FT_ERROR_BASE( FT_ERR( e ) ) )
#define FT_ERR_NEQ( x, e )                                       \
          ( FT_ERROR_BASE( x ) != FT_ERROR_BASE( FT_ERR( e ) ) )

FT_END_HEADER

#endif

