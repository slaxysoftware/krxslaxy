
#ifndef FREETYPE_H_
#define FREETYPE_H_

#include <ft2build.h>
#include FT_CONFIG_CONFIG_H
#include <freetype/fttypes.h>
#include <freetype/fterrors.h>

FT_BEGIN_HEADER

  typedef struct  FT_Glyph_Metrics_
  {
    FT_Pos  width;
    FT_Pos  height;

    FT_Pos  horiBearingX;
    FT_Pos  horiBearingY;
    FT_Pos  horiAdvance;

    FT_Pos  vertBearingX;
    FT_Pos  vertBearingY;
    FT_Pos  vertAdvance;

  } FT_Glyph_Metrics;

  typedef struct  FT_Bitmap_Size_
  {
    FT_Short  height;
    FT_Short  width;

    FT_Pos    size;

    FT_Pos    x_ppem;
    FT_Pos    y_ppem;

  } FT_Bitmap_Size;

  typedef struct FT_LibraryRec_  *FT_Library;

  typedef struct FT_ModuleRec_*  FT_Module;

  typedef struct FT_DriverRec_*  FT_Driver;

  typedef struct FT_RendererRec_*  FT_Renderer;

  typedef struct FT_FaceRec_*  FT_Face;

  typedef struct FT_SizeRec_*  FT_Size;

  typedef struct FT_GlyphSlotRec_*  FT_GlyphSlot;

  typedef struct FT_CharMapRec_*  FT_CharMap;

#ifndef FT_ENC_TAG

#define FT_ENC_TAG( value, a, b, c, d )                             \
          value = ( ( FT_STATIC_BYTE_CAST( FT_UInt32, a ) << 24 ) | \
                    ( FT_STATIC_BYTE_CAST( FT_UInt32, b ) << 16 ) | \
                    ( FT_STATIC_BYTE_CAST( FT_UInt32, c ) <<  8 ) | \
                      FT_STATIC_BYTE_CAST( FT_UInt32, d )         )

#endif

  typedef enum  FT_Encoding_
  {
    FT_ENC_TAG( FT_ENCODING_NONE, 0, 0, 0, 0 ),

    FT_ENC_TAG( FT_ENCODING_MS_SYMBOL, 's', 'y', 'm', 'b' ),
    FT_ENC_TAG( FT_ENCODING_UNICODE,   'u', 'n', 'i', 'c' ),

    FT_ENC_TAG( FT_ENCODING_SJIS,    's', 'j', 'i', 's' ),
    FT_ENC_TAG( FT_ENCODING_PRC,     'g', 'b', ' ', ' ' ),
    FT_ENC_TAG( FT_ENCODING_BIG5,    'b', 'i', 'g', '5' ),
    FT_ENC_TAG( FT_ENCODING_WANSUNG, 'w', 'a', 'n', 's' ),
    FT_ENC_TAG( FT_ENCODING_JOHAB,   'j', 'o', 'h', 'a' ),

    FT_ENCODING_GB2312     = FT_ENCODING_PRC,
    FT_ENCODING_MS_SJIS    = FT_ENCODING_SJIS,
    FT_ENCODING_MS_GB2312  = FT_ENCODING_PRC,
    FT_ENCODING_MS_BIG5    = FT_ENCODING_BIG5,
    FT_ENCODING_MS_WANSUNG = FT_ENCODING_WANSUNG,
    FT_ENCODING_MS_JOHAB   = FT_ENCODING_JOHAB,

    FT_ENC_TAG( FT_ENCODING_ADOBE_STANDARD, 'A', 'D', 'O', 'B' ),
    FT_ENC_TAG( FT_ENCODING_ADOBE_EXPERT,   'A', 'D', 'B', 'E' ),
    FT_ENC_TAG( FT_ENCODING_ADOBE_CUSTOM,   'A', 'D', 'B', 'C' ),
    FT_ENC_TAG( FT_ENCODING_ADOBE_LATIN_1,  'l', 'a', 't', '1' ),

    FT_ENC_TAG( FT_ENCODING_OLD_LATIN_2, 'l', 'a', 't', '2' ),

    FT_ENC_TAG( FT_ENCODING_APPLE_ROMAN, 'a', 'r', 'm', 'n' )

  } FT_Encoding;

#define ft_encoding_none            FT_ENCODING_NONE
#define ft_encoding_unicode         FT_ENCODING_UNICODE
#define ft_encoding_symbol          FT_ENCODING_MS_SYMBOL
#define ft_encoding_latin_1         FT_ENCODING_ADOBE_LATIN_1
#define ft_encoding_latin_2         FT_ENCODING_OLD_LATIN_2
#define ft_encoding_sjis            FT_ENCODING_SJIS
#define ft_encoding_gb2312          FT_ENCODING_PRC
#define ft_encoding_big5            FT_ENCODING_BIG5
#define ft_encoding_wansung         FT_ENCODING_WANSUNG
#define ft_encoding_johab           FT_ENCODING_JOHAB

#define ft_encoding_adobe_standard  FT_ENCODING_ADOBE_STANDARD
#define ft_encoding_adobe_expert    FT_ENCODING_ADOBE_EXPERT
#define ft_encoding_adobe_custom    FT_ENCODING_ADOBE_CUSTOM
#define ft_encoding_apple_roman     FT_ENCODING_APPLE_ROMAN

  typedef struct  FT_CharMapRec_
  {
    FT_Face      face;
    FT_Encoding  encoding;
    FT_UShort    platform_id;
    FT_UShort    encoding_id;

  } FT_CharMapRec;

  typedef struct FT_Face_InternalRec_*  FT_Face_Internal;

  typedef struct  FT_FaceRec_
  {
    FT_Long           num_faces;
    FT_Long           face_index;

    FT_Long           face_flags;
    FT_Long           style_flags;

    FT_Long           num_glyphs;

    FT_String*        family_name;
    FT_String*        style_name;

    FT_Int            num_fixed_sizes;
    FT_Bitmap_Size*   available_sizes;

    FT_Int            num_charmaps;
    FT_CharMap*       charmaps;

    FT_Generic        generic;

    FT_BBox           bbox;

    FT_UShort         units_per_EM;
    FT_Short          ascender;
    FT_Short          descender;
    FT_Short          height;

    FT_Short          max_advance_width;
    FT_Short          max_advance_height;

    FT_Short          underline_position;
    FT_Short          underline_thickness;

    FT_GlyphSlot      glyph;
    FT_Size           size;
    FT_CharMap        charmap;

    FT_Driver         driver;
    FT_Memory         memory;
    FT_Stream         stream;

    FT_ListRec        sizes_list;

    FT_Generic        autohint;
    void*             extensions;

    FT_Face_Internal  internal;

  } FT_FaceRec;

#define FT_FACE_FLAG_SCALABLE          ( 1L <<  0 )
#define FT_FACE_FLAG_FIXED_SIZES       ( 1L <<  1 )
#define FT_FACE_FLAG_FIXED_WIDTH       ( 1L <<  2 )
#define FT_FACE_FLAG_SFNT              ( 1L <<  3 )
#define FT_FACE_FLAG_HORIZONTAL        ( 1L <<  4 )
#define FT_FACE_FLAG_VERTICAL          ( 1L <<  5 )
#define FT_FACE_FLAG_KERNING           ( 1L <<  6 )
#define FT_FACE_FLAG_FAST_GLYPHS       ( 1L <<  7 )
#define FT_FACE_FLAG_MULTIPLE_MASTERS  ( 1L <<  8 )
#define FT_FACE_FLAG_GLYPH_NAMES       ( 1L <<  9 )
#define FT_FACE_FLAG_EXTERNAL_STREAM   ( 1L << 10 )
#define FT_FACE_FLAG_HINTER            ( 1L << 11 )
#define FT_FACE_FLAG_CID_KEYED         ( 1L << 12 )
#define FT_FACE_FLAG_TRICKY            ( 1L << 13 )
#define FT_FACE_FLAG_COLOR             ( 1L << 14 )
#define FT_FACE_FLAG_VARIATION         ( 1L << 15 )
#define FT_FACE_FLAG_SVG               ( 1L << 16 )
#define FT_FACE_FLAG_SBIX              ( 1L << 17 )
#define FT_FACE_FLAG_SBIX_OVERLAY      ( 1L << 18 )

#define FT_HAS_HORIZONTAL( face ) \
          ( !!( (face)->face_flags & FT_FACE_FLAG_HORIZONTAL ) )

#define FT_HAS_VERTICAL( face ) \
          ( !!( (face)->face_flags & FT_FACE_FLAG_VERTICAL ) )

#define FT_HAS_KERNING( face ) \
          ( !!( (face)->face_flags & FT_FACE_FLAG_KERNING ) )

#define FT_IS_SCALABLE( face ) \
          ( !!( (face)->face_flags & FT_FACE_FLAG_SCALABLE ) )

#define FT_IS_SFNT( face ) \
          ( !!( (face)->face_flags & FT_FACE_FLAG_SFNT ) )

#define FT_IS_FIXED_WIDTH( face ) \
          ( !!( (face)->face_flags & FT_FACE_FLAG_FIXED_WIDTH ) )

#define FT_HAS_FIXED_SIZES( face ) \
          ( !!( (face)->face_flags & FT_FACE_FLAG_FIXED_SIZES ) )

#define FT_HAS_FAST_GLYPHS( face )  0

#define FT_HAS_GLYPH_NAMES( face ) \
          ( !!( (face)->face_flags & FT_FACE_FLAG_GLYPH_NAMES ) )

#define FT_HAS_MULTIPLE_MASTERS( face ) \
          ( !!( (face)->face_flags & FT_FACE_FLAG_MULTIPLE_MASTERS ) )

#define FT_IS_NAMED_INSTANCE( face ) \
          ( !!( (face)->face_index & 0x7FFF0000L ) )

#define FT_IS_VARIATION( face ) \
          ( !!( (face)->face_flags & FT_FACE_FLAG_VARIATION ) )

#define FT_IS_CID_KEYED( face ) \
          ( !!( (face)->face_flags & FT_FACE_FLAG_CID_KEYED ) )

#define FT_IS_TRICKY( face ) \
          ( !!( (face)->face_flags & FT_FACE_FLAG_TRICKY ) )

#define FT_HAS_COLOR( face ) \
          ( !!( (face)->face_flags & FT_FACE_FLAG_COLOR ) )

#define FT_HAS_SVG( face ) \
          ( !!( (face)->face_flags & FT_FACE_FLAG_SVG ) )

#define FT_HAS_SBIX( face ) \
          ( !!( (face)->face_flags & FT_FACE_FLAG_SBIX ) )

#define FT_HAS_SBIX_OVERLAY( face ) \
          ( !!( (face)->face_flags & FT_FACE_FLAG_SBIX_OVERLAY ) )

#define FT_STYLE_FLAG_ITALIC  ( 1 << 0 )
#define FT_STYLE_FLAG_BOLD    ( 1 << 1 )

  typedef struct FT_Size_InternalRec_*  FT_Size_Internal;

  typedef struct  FT_Size_Metrics_
  {
    FT_UShort  x_ppem;
    FT_UShort  y_ppem;

    FT_Fixed   x_scale;
    FT_Fixed   y_scale;

    FT_Pos     ascender;
    FT_Pos     descender;
    FT_Pos     height;
    FT_Pos     max_advance;

  } FT_Size_Metrics;

  typedef struct  FT_SizeRec_
  {
    FT_Face           face;
    FT_Generic        generic;
    FT_Size_Metrics   metrics;
    FT_Size_Internal  internal;

  } FT_SizeRec;

  typedef struct FT_SubGlyphRec_*  FT_SubGlyph;

  typedef struct FT_Slot_InternalRec_*  FT_Slot_Internal;

  typedef struct  FT_GlyphSlotRec_
  {
    FT_Library        library;
    FT_Face           face;
    FT_GlyphSlot      next;
    FT_UInt           glyph_index;
    FT_Generic        generic;

    FT_Glyph_Metrics  metrics;
    FT_Fixed          linearHoriAdvance;
    FT_Fixed          linearVertAdvance;
    FT_Vector         advance;

    FT_Glyph_Format   format;

    FT_Bitmap         bitmap;
    FT_Int            bitmap_left;
    FT_Int            bitmap_top;

    FT_Outline        outline;

    FT_UInt           num_subglyphs;
    FT_SubGlyph       subglyphs;

    void*             control_data;
    long              control_len;

    FT_Pos            lsb_delta;
    FT_Pos            rsb_delta;

    void*             other;

    FT_Slot_Internal  internal;

  } FT_GlyphSlotRec;

  FT_EXPORT( FT_Error )
  FT_Init_FreeType( FT_Library  *alibrary );

  FT_EXPORT( FT_Error )
  FT_Done_FreeType( FT_Library  library );

#define FT_OPEN_MEMORY    0x1
#define FT_OPEN_STREAM    0x2
#define FT_OPEN_PATHNAME  0x4
#define FT_OPEN_DRIVER    0x8
#define FT_OPEN_PARAMS    0x10

#define ft_open_memory    FT_OPEN_MEMORY
#define ft_open_stream    FT_OPEN_STREAM
#define ft_open_pathname  FT_OPEN_PATHNAME
#define ft_open_driver    FT_OPEN_DRIVER
#define ft_open_params    FT_OPEN_PARAMS

  typedef struct  FT_Parameter_
  {
    FT_ULong    tag;
    FT_Pointer  data;

  } FT_Parameter;

  typedef struct  FT_Open_Args_
  {
    FT_UInt         flags;
    const FT_Byte*  memory_base;
    FT_Long         memory_size;
    FT_String*      pathname;
    FT_Stream       stream;
    FT_Module       driver;
    FT_Int          num_params;
    FT_Parameter*   params;

  } FT_Open_Args;

  FT_EXPORT( FT_Error )
  FT_New_Face( FT_Library   library,
               const char*  filepathname,
               FT_Long      face_index,
               FT_Face     *aface );

  FT_EXPORT( FT_Error )
  FT_New_Memory_Face( FT_Library      library,
                      const FT_Byte*  file_base,
                      FT_Long         file_size,
                      FT_Long         face_index,
                      FT_Face        *aface );

  FT_EXPORT( FT_Error )
  FT_Open_Face( FT_Library           library,
                const FT_Open_Args*  args,
                FT_Long              face_index,
                FT_Face             *aface );

  FT_EXPORT( FT_Error )
  FT_Attach_File( FT_Face      face,
                  const char*  filepathname );

  FT_EXPORT( FT_Error )
  FT_Attach_Stream( FT_Face        face,
                    FT_Open_Args*  parameters );

  FT_EXPORT( FT_Error )
  FT_Reference_Face( FT_Face  face );

  FT_EXPORT( FT_Error )
  FT_Done_Face( FT_Face  face );

  FT_EXPORT( FT_Error )
  FT_Select_Size( FT_Face  face,
                  FT_Int   strike_index );

  typedef enum  FT_Size_Request_Type_
  {
    FT_SIZE_REQUEST_TYPE_NOMINAL,
    FT_SIZE_REQUEST_TYPE_REAL_DIM,
    FT_SIZE_REQUEST_TYPE_BBOX,
    FT_SIZE_REQUEST_TYPE_CELL,
    FT_SIZE_REQUEST_TYPE_SCALES,

    FT_SIZE_REQUEST_TYPE_MAX

  } FT_Size_Request_Type;

  typedef struct  FT_Size_RequestRec_
  {
    FT_Size_Request_Type  type;
    FT_Long               width;
    FT_Long               height;
    FT_UInt               horiResolution;
    FT_UInt               vertResolution;

  } FT_Size_RequestRec;

  typedef struct FT_Size_RequestRec_  *FT_Size_Request;

  FT_EXPORT( FT_Error )
  FT_Request_Size( FT_Face          face,
                   FT_Size_Request  req );

  FT_EXPORT( FT_Error )
  FT_Set_Char_Size( FT_Face     face,
                    FT_F26Dot6  char_width,
                    FT_F26Dot6  char_height,
                    FT_UInt     horz_resolution,
                    FT_UInt     vert_resolution );

  FT_EXPORT( FT_Error )
  FT_Set_Pixel_Sizes( FT_Face  face,
                      FT_UInt  pixel_width,
                      FT_UInt  pixel_height );

  FT_EXPORT( FT_Error )
  FT_Load_Glyph( FT_Face   face,
                 FT_UInt   glyph_index,
                 FT_Int32  load_flags );

  FT_EXPORT( FT_Error )
  FT_Load_Char( FT_Face   face,
                FT_ULong  char_code,
                FT_Int32  load_flags );

#define FT_LOAD_DEFAULT                      0x0
#define FT_LOAD_NO_SCALE                     ( 1L << 0  )
#define FT_LOAD_NO_HINTING                   ( 1L << 1  )
#define FT_LOAD_RENDER                       ( 1L << 2  )
#define FT_LOAD_NO_BITMAP                    ( 1L << 3  )
#define FT_LOAD_VERTICAL_LAYOUT              ( 1L << 4  )
#define FT_LOAD_FORCE_AUTOHINT               ( 1L << 5  )
#define FT_LOAD_CROP_BITMAP                  ( 1L << 6  )
#define FT_LOAD_PEDANTIC                     ( 1L << 7  )
#define FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH  ( 1L << 9  )
#define FT_LOAD_NO_RECURSE                   ( 1L << 10 )
#define FT_LOAD_IGNORE_TRANSFORM             ( 1L << 11 )
#define FT_LOAD_MONOCHROME                   ( 1L << 12 )
#define FT_LOAD_LINEAR_DESIGN                ( 1L << 13 )
#define FT_LOAD_SBITS_ONLY                   ( 1L << 14 )
#define FT_LOAD_NO_AUTOHINT                  ( 1L << 15 )

#define FT_LOAD_COLOR                        ( 1L << 20 )
#define FT_LOAD_COMPUTE_METRICS              ( 1L << 21 )
#define FT_LOAD_BITMAP_METRICS_ONLY          ( 1L << 22 )

#define FT_LOAD_ADVANCE_ONLY                 ( 1L << 8  )
#define FT_LOAD_SVG_ONLY                     ( 1L << 23 )

#define FT_LOAD_TARGET_( x )   ( FT_STATIC_CAST( FT_Int32, (x) & 15 ) << 16 )

#define FT_LOAD_TARGET_NORMAL  FT_LOAD_TARGET_( FT_RENDER_MODE_NORMAL )
#define FT_LOAD_TARGET_LIGHT   FT_LOAD_TARGET_( FT_RENDER_MODE_LIGHT  )
#define FT_LOAD_TARGET_MONO    FT_LOAD_TARGET_( FT_RENDER_MODE_MONO   )
#define FT_LOAD_TARGET_LCD     FT_LOAD_TARGET_( FT_RENDER_MODE_LCD    )
#define FT_LOAD_TARGET_LCD_V   FT_LOAD_TARGET_( FT_RENDER_MODE_LCD_V  )

#define FT_LOAD_TARGET_MODE( x )                               \
          FT_STATIC_CAST( FT_Render_Mode, ( (x) >> 16 ) & 15 )

  FT_EXPORT( void )
  FT_Set_Transform( FT_Face     face,
                    FT_Matrix*  matrix,
                    FT_Vector*  delta );

  FT_EXPORT( void )
  FT_Get_Transform( FT_Face     face,
                    FT_Matrix*  matrix,
                    FT_Vector*  delta );

  typedef enum  FT_Render_Mode_
  {
    FT_RENDER_MODE_NORMAL = 0,
    FT_RENDER_MODE_LIGHT,
    FT_RENDER_MODE_MONO,
    FT_RENDER_MODE_LCD,
    FT_RENDER_MODE_LCD_V,
    FT_RENDER_MODE_SDF,

    FT_RENDER_MODE_MAX

  } FT_Render_Mode;

#define ft_render_mode_normal  FT_RENDER_MODE_NORMAL
#define ft_render_mode_mono    FT_RENDER_MODE_MONO

  FT_EXPORT( FT_Error )
  FT_Render_Glyph( FT_GlyphSlot    slot,
                   FT_Render_Mode  render_mode );

  typedef enum  FT_Kerning_Mode_
  {
    FT_KERNING_DEFAULT = 0,
    FT_KERNING_UNFITTED,
    FT_KERNING_UNSCALED

  } FT_Kerning_Mode;

#define ft_kerning_default   FT_KERNING_DEFAULT
#define ft_kerning_unfitted  FT_KERNING_UNFITTED
#define ft_kerning_unscaled  FT_KERNING_UNSCALED

  FT_EXPORT( FT_Error )
  FT_Get_Kerning( FT_Face     face,
                  FT_UInt     left_glyph,
                  FT_UInt     right_glyph,
                  FT_UInt     kern_mode,
                  FT_Vector  *akerning );

  FT_EXPORT( FT_Error )
  FT_Get_Track_Kerning( FT_Face    face,
                        FT_Fixed   point_size,
                        FT_Int     degree,
                        FT_Fixed*  akerning );

  FT_EXPORT( FT_Error )
  FT_Get_Glyph_Name( FT_Face     face,
                     FT_UInt     glyph_index,
                     FT_Pointer  buffer,
                     FT_UInt     buffer_max );

  FT_EXPORT( const char* )
  FT_Get_Postscript_Name( FT_Face  face );

  FT_EXPORT( FT_Error )
  FT_Select_Charmap( FT_Face      face,
                     FT_Encoding  encoding );

  FT_EXPORT( FT_Error )
  FT_Set_Charmap( FT_Face     face,
                  FT_CharMap  charmap );

  FT_EXPORT( FT_Int )
  FT_Get_Charmap_Index( FT_CharMap  charmap );

  FT_EXPORT( FT_UInt )
  FT_Get_Char_Index( FT_Face   face,
                     FT_ULong  charcode );

  FT_EXPORT( FT_ULong )
  FT_Get_First_Char( FT_Face   face,
                     FT_UInt  *agindex );

  FT_EXPORT( FT_ULong )
  FT_Get_Next_Char( FT_Face    face,
                    FT_ULong   char_code,
                    FT_UInt   *agindex );

  FT_EXPORT( FT_Error )
  FT_Face_Properties( FT_Face        face,
                      FT_UInt        num_properties,
                      FT_Parameter*  properties );

  FT_EXPORT( FT_UInt )
  FT_Get_Name_Index( FT_Face           face,
                     const FT_String*  glyph_name );

#define FT_SUBGLYPH_FLAG_ARGS_ARE_WORDS          1
#define FT_SUBGLYPH_FLAG_ARGS_ARE_XY_VALUES      2
#define FT_SUBGLYPH_FLAG_ROUND_XY_TO_GRID        4
#define FT_SUBGLYPH_FLAG_SCALE                   8
#define FT_SUBGLYPH_FLAG_XY_SCALE             0x40
#define FT_SUBGLYPH_FLAG_2X2                  0x80
#define FT_SUBGLYPH_FLAG_USE_MY_METRICS      0x200

  FT_EXPORT( FT_Error )
  FT_Get_SubGlyph_Info( FT_GlyphSlot  glyph,
                        FT_UInt       sub_index,
                        FT_Int       *p_index,
                        FT_UInt      *p_flags,
                        FT_Int       *p_arg1,
                        FT_Int       *p_arg2,
                        FT_Matrix    *p_transform );

#define FT_FSTYPE_INSTALLABLE_EMBEDDING         0x0000
#define FT_FSTYPE_RESTRICTED_LICENSE_EMBEDDING  0x0002
#define FT_FSTYPE_PREVIEW_AND_PRINT_EMBEDDING   0x0004
#define FT_FSTYPE_EDITABLE_EMBEDDING            0x0008
#define FT_FSTYPE_NO_SUBSETTING                 0x0100
#define FT_FSTYPE_BITMAP_EMBEDDING_ONLY         0x0200

  FT_EXPORT( FT_UShort )
  FT_Get_FSType_Flags( FT_Face  face );

  FT_EXPORT( FT_UInt )
  FT_Face_GetCharVariantIndex( FT_Face   face,
                               FT_ULong  charcode,
                               FT_ULong  variantSelector );

  FT_EXPORT( FT_Int )
  FT_Face_GetCharVariantIsDefault( FT_Face   face,
                                   FT_ULong  charcode,
                                   FT_ULong  variantSelector );

  FT_EXPORT( FT_UInt32* )
  FT_Face_GetVariantSelectors( FT_Face  face );

  FT_EXPORT( FT_UInt32* )
  FT_Face_GetVariantsOfChar( FT_Face   face,
                             FT_ULong  charcode );

  FT_EXPORT( FT_UInt32* )
  FT_Face_GetCharsOfVariant( FT_Face   face,
                             FT_ULong  variantSelector );

  FT_EXPORT( FT_Long )
  FT_MulDiv( FT_Long  a,
             FT_Long  b,
             FT_Long  c );

  FT_EXPORT( FT_Long )
  FT_MulFix( FT_Long  a,
             FT_Long  b );

  FT_EXPORT( FT_Long )
  FT_DivFix( FT_Long  a,
             FT_Long  b );

  FT_EXPORT( FT_Fixed )
  FT_RoundFix( FT_Fixed  a );

  FT_EXPORT( FT_Fixed )
  FT_CeilFix( FT_Fixed  a );

  FT_EXPORT( FT_Fixed )
  FT_FloorFix( FT_Fixed  a );

  FT_EXPORT( void )
  FT_Vector_Transform( FT_Vector*        vector,
                       const FT_Matrix*  matrix );

#define FREETYPE_MAJOR  2
#define FREETYPE_MINOR  12
#define FREETYPE_PATCH  1

  FT_EXPORT( void )
  FT_Library_Version( FT_Library   library,
                      FT_Int      *amajor,
                      FT_Int      *aminor,
                      FT_Int      *apatch );

  FT_EXPORT( FT_Bool )
  FT_Face_CheckTrueTypePatents( FT_Face  face );

  FT_EXPORT( FT_Bool )
  FT_Face_SetUnpatentedHinting( FT_Face  face,
                                FT_Bool  value );

FT_END_HEADER

#endif

