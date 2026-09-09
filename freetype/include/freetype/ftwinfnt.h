
#ifndef FTWINFNT_H_
#define FTWINFNT_H_

#include <freetype/freetype.h>

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

FT_BEGIN_HEADER

#define FT_WinFNT_ID_CP1252    0
#define FT_WinFNT_ID_DEFAULT   1
#define FT_WinFNT_ID_SYMBOL    2
#define FT_WinFNT_ID_MAC      77
#define FT_WinFNT_ID_CP932   128
#define FT_WinFNT_ID_CP949   129
#define FT_WinFNT_ID_CP1361  130
#define FT_WinFNT_ID_CP936   134
#define FT_WinFNT_ID_CP950   136
#define FT_WinFNT_ID_CP1253  161
#define FT_WinFNT_ID_CP1254  162
#define FT_WinFNT_ID_CP1258  163
#define FT_WinFNT_ID_CP1255  177
#define FT_WinFNT_ID_CP1256  178
#define FT_WinFNT_ID_CP1257  186
#define FT_WinFNT_ID_CP1251  204
#define FT_WinFNT_ID_CP874   222
#define FT_WinFNT_ID_CP1250  238
#define FT_WinFNT_ID_OEM     255

  typedef struct  FT_WinFNT_HeaderRec_
  {
    FT_UShort  version;
    FT_ULong   file_size;
    FT_Byte    copyright[60];
    FT_UShort  file_type;
    FT_UShort  nominal_point_size;
    FT_UShort  vertical_resolution;
    FT_UShort  horizontal_resolution;
    FT_UShort  ascent;
    FT_UShort  internal_leading;
    FT_UShort  external_leading;
    FT_Byte    italic;
    FT_Byte    underline;
    FT_Byte    strike_out;
    FT_UShort  weight;
    FT_Byte    charset;
    FT_UShort  pixel_width;
    FT_UShort  pixel_height;
    FT_Byte    pitch_and_family;
    FT_UShort  avg_width;
    FT_UShort  max_width;
    FT_Byte    first_char;
    FT_Byte    last_char;
    FT_Byte    default_char;
    FT_Byte    break_char;
    FT_UShort  bytes_per_row;
    FT_ULong   device_offset;
    FT_ULong   face_name_offset;
    FT_ULong   bits_pointer;
    FT_ULong   bits_offset;
    FT_Byte    reserved;
    FT_ULong   flags;
    FT_UShort  A_space;
    FT_UShort  B_space;
    FT_UShort  C_space;
    FT_UShort  color_table_offset;
    FT_ULong   reserved1[4];

  } FT_WinFNT_HeaderRec;

  typedef struct FT_WinFNT_HeaderRec_*  FT_WinFNT_Header;

  FT_EXPORT( FT_Error )
  FT_Get_WinFNT_Header( FT_Face               face,
                        FT_WinFNT_HeaderRec  *aheader );

FT_END_HEADER

#endif

