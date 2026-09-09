
#ifndef FTGLYPH_H_
#define FTGLYPH_H_

#include <freetype/freetype.h>

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

FT_BEGIN_HEADER

  typedef struct FT_Glyph_Class_  FT_Glyph_Class;

  typedef struct FT_GlyphRec_*  FT_Glyph;

  typedef struct  FT_GlyphRec_
  {
    FT_Library             library;
    const FT_Glyph_Class*  clazz;
    FT_Glyph_Format        format;
    FT_Vector              advance;

  } FT_GlyphRec;

  typedef struct FT_BitmapGlyphRec_*  FT_BitmapGlyph;

  typedef struct  FT_BitmapGlyphRec_
  {
    FT_GlyphRec  root;
    FT_Int       left;
    FT_Int       top;
    FT_Bitmap    bitmap;

  } FT_BitmapGlyphRec;

  typedef struct FT_OutlineGlyphRec_*  FT_OutlineGlyph;

  typedef struct  FT_OutlineGlyphRec_
  {
    FT_GlyphRec  root;
    FT_Outline   outline;

  } FT_OutlineGlyphRec;

  typedef struct FT_SvgGlyphRec_*  FT_SvgGlyph;

  typedef struct  FT_SvgGlyphRec_
  {
    FT_GlyphRec  root;

    FT_Byte*  svg_document;
    FT_ULong  svg_document_length;

    FT_UInt  glyph_index;

    FT_Size_Metrics  metrics;
    FT_UShort        units_per_EM;

    FT_UShort  start_glyph_id;
    FT_UShort  end_glyph_id;

    FT_Matrix  transform;
    FT_Vector  delta;

  } FT_SvgGlyphRec;

  FT_EXPORT( FT_Error )
  FT_New_Glyph( FT_Library       library,
                FT_Glyph_Format  format,
                FT_Glyph         *aglyph );

  FT_EXPORT( FT_Error )
  FT_Get_Glyph( FT_GlyphSlot  slot,
                FT_Glyph     *aglyph );

  FT_EXPORT( FT_Error )
  FT_Glyph_Copy( FT_Glyph   source,
                 FT_Glyph  *target );

  FT_EXPORT( FT_Error )
  FT_Glyph_Transform( FT_Glyph          glyph,
                      const FT_Matrix*  matrix,
                      const FT_Vector*  delta );

  typedef enum  FT_Glyph_BBox_Mode_
  {
    FT_GLYPH_BBOX_UNSCALED  = 0,
    FT_GLYPH_BBOX_SUBPIXELS = 0,
    FT_GLYPH_BBOX_GRIDFIT   = 1,
    FT_GLYPH_BBOX_TRUNCATE  = 2,
    FT_GLYPH_BBOX_PIXELS    = 3

  } FT_Glyph_BBox_Mode;

#define ft_glyph_bbox_unscaled   FT_GLYPH_BBOX_UNSCALED
#define ft_glyph_bbox_subpixels  FT_GLYPH_BBOX_SUBPIXELS
#define ft_glyph_bbox_gridfit    FT_GLYPH_BBOX_GRIDFIT
#define ft_glyph_bbox_truncate   FT_GLYPH_BBOX_TRUNCATE
#define ft_glyph_bbox_pixels     FT_GLYPH_BBOX_PIXELS

  FT_EXPORT( void )
  FT_Glyph_Get_CBox( FT_Glyph  glyph,
                     FT_UInt   bbox_mode,
                     FT_BBox  *acbox );

  FT_EXPORT( FT_Error )
  FT_Glyph_To_Bitmap( FT_Glyph*         the_glyph,
                      FT_Render_Mode    render_mode,
                      const FT_Vector*  origin,
                      FT_Bool           destroy );

  FT_EXPORT( void )
  FT_Done_Glyph( FT_Glyph  glyph );

  FT_EXPORT( void )
  FT_Matrix_Multiply( const FT_Matrix*  a,
                      FT_Matrix*        b );

  FT_EXPORT( FT_Error )
  FT_Matrix_Invert( FT_Matrix*  matrix );

FT_END_HEADER

#endif

