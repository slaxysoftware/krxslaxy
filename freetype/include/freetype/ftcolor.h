
#ifndef FTCOLOR_H_
#define FTCOLOR_H_

#include <freetype/freetype.h>

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

FT_BEGIN_HEADER

  typedef struct  FT_Color_
  {
    FT_Byte  blue;
    FT_Byte  green;
    FT_Byte  red;
    FT_Byte  alpha;

  } FT_Color;

#define FT_PALETTE_FOR_LIGHT_BACKGROUND  0x01
#define FT_PALETTE_FOR_DARK_BACKGROUND   0x02

  typedef struct  FT_Palette_Data_ {
    FT_UShort         num_palettes;
    const FT_UShort*  palette_name_ids;
    const FT_UShort*  palette_flags;

    FT_UShort         num_palette_entries;
    const FT_UShort*  palette_entry_name_ids;

  } FT_Palette_Data;

  FT_EXPORT( FT_Error )
  FT_Palette_Data_Get( FT_Face           face,
                       FT_Palette_Data  *apalette );

  FT_EXPORT( FT_Error )
  FT_Palette_Select( FT_Face     face,
                     FT_UShort   palette_index,
                     FT_Color*  *apalette );

  FT_EXPORT( FT_Error )
  FT_Palette_Set_Foreground_Color( FT_Face   face,
                                   FT_Color  foreground_color );

  typedef struct  FT_LayerIterator_
  {
    FT_UInt   num_layers;
    FT_UInt   layer;
    FT_Byte*  p;

  } FT_LayerIterator;

  FT_EXPORT( FT_Bool )
  FT_Get_Color_Glyph_Layer( FT_Face            face,
                            FT_UInt            base_glyph,
                            FT_UInt           *aglyph_index,
                            FT_UInt           *acolor_index,
                            FT_LayerIterator*  iterator );

  typedef enum  FT_PaintFormat_
  {
    FT_COLR_PAINTFORMAT_COLR_LAYERS     = 1,
    FT_COLR_PAINTFORMAT_SOLID           = 2,
    FT_COLR_PAINTFORMAT_LINEAR_GRADIENT = 4,
    FT_COLR_PAINTFORMAT_RADIAL_GRADIENT = 6,
    FT_COLR_PAINTFORMAT_SWEEP_GRADIENT  = 8,
    FT_COLR_PAINTFORMAT_GLYPH           = 10,
    FT_COLR_PAINTFORMAT_COLR_GLYPH      = 11,
    FT_COLR_PAINTFORMAT_TRANSFORM       = 12,
    FT_COLR_PAINTFORMAT_TRANSLATE       = 14,
    FT_COLR_PAINTFORMAT_SCALE           = 16,
    FT_COLR_PAINTFORMAT_ROTATE          = 24,
    FT_COLR_PAINTFORMAT_SKEW            = 28,
    FT_COLR_PAINTFORMAT_COMPOSITE       = 32,
    FT_COLR_PAINT_FORMAT_MAX            = 33,
    FT_COLR_PAINTFORMAT_UNSUPPORTED     = 255

  } FT_PaintFormat;

  typedef struct  FT_ColorStopIterator_
  {
    FT_UInt  num_color_stops;
    FT_UInt  current_color_stop;

    FT_Byte*  p;

  } FT_ColorStopIterator;

  typedef struct  FT_ColorIndex_
  {
    FT_UInt16   palette_index;
    FT_F2Dot14  alpha;

  } FT_ColorIndex;

  typedef struct  FT_ColorStop_
  {
    FT_F2Dot14     stop_offset;
    FT_ColorIndex  color;

  } FT_ColorStop;

  typedef enum  FT_PaintExtend_
  {
    FT_COLR_PAINT_EXTEND_PAD     = 0,
    FT_COLR_PAINT_EXTEND_REPEAT  = 1,
    FT_COLR_PAINT_EXTEND_REFLECT = 2

  } FT_PaintExtend;

  typedef struct  FT_ColorLine_
  {
    FT_PaintExtend        extend;
    FT_ColorStopIterator  color_stop_iterator;

  } FT_ColorLine;

  typedef struct  FT_Affine_23_
  {
    FT_Fixed  xx, xy, dx;
    FT_Fixed  yx, yy, dy;

  } FT_Affine23;

  typedef enum  FT_Composite_Mode_
  {
    FT_COLR_COMPOSITE_CLEAR          = 0,
    FT_COLR_COMPOSITE_SRC            = 1,
    FT_COLR_COMPOSITE_DEST           = 2,
    FT_COLR_COMPOSITE_SRC_OVER       = 3,
    FT_COLR_COMPOSITE_DEST_OVER      = 4,
    FT_COLR_COMPOSITE_SRC_IN         = 5,
    FT_COLR_COMPOSITE_DEST_IN        = 6,
    FT_COLR_COMPOSITE_SRC_OUT        = 7,
    FT_COLR_COMPOSITE_DEST_OUT       = 8,
    FT_COLR_COMPOSITE_SRC_ATOP       = 9,
    FT_COLR_COMPOSITE_DEST_ATOP      = 10,
    FT_COLR_COMPOSITE_XOR            = 11,
    FT_COLR_COMPOSITE_PLUS           = 12,
    FT_COLR_COMPOSITE_SCREEN         = 13,
    FT_COLR_COMPOSITE_OVERLAY        = 14,
    FT_COLR_COMPOSITE_DARKEN         = 15,
    FT_COLR_COMPOSITE_LIGHTEN        = 16,
    FT_COLR_COMPOSITE_COLOR_DODGE    = 17,
    FT_COLR_COMPOSITE_COLOR_BURN     = 18,
    FT_COLR_COMPOSITE_HARD_LIGHT     = 19,
    FT_COLR_COMPOSITE_SOFT_LIGHT     = 20,
    FT_COLR_COMPOSITE_DIFFERENCE     = 21,
    FT_COLR_COMPOSITE_EXCLUSION      = 22,
    FT_COLR_COMPOSITE_MULTIPLY       = 23,
    FT_COLR_COMPOSITE_HSL_HUE        = 24,
    FT_COLR_COMPOSITE_HSL_SATURATION = 25,
    FT_COLR_COMPOSITE_HSL_COLOR      = 26,
    FT_COLR_COMPOSITE_HSL_LUMINOSITY = 27,
    FT_COLR_COMPOSITE_MAX            = 28

  } FT_Composite_Mode;

  typedef struct  FT_Opaque_Paint_
  {
    FT_Byte*  p;
    FT_Bool   insert_root_transform;
  } FT_OpaquePaint;

  typedef struct  FT_PaintColrLayers_
  {
    FT_LayerIterator  layer_iterator;

  } FT_PaintColrLayers;

  typedef struct  FT_PaintSolid_
  {
    FT_ColorIndex  color;

  } FT_PaintSolid;

  typedef struct  FT_PaintLinearGradient_
  {
    FT_ColorLine  colorline;

    FT_Vector  p0;
    FT_Vector  p1;
    FT_Vector  p2;

  } FT_PaintLinearGradient;

  typedef struct  FT_PaintRadialGradient_
  {
    FT_ColorLine  colorline;

    FT_Vector  c0;
    FT_Pos     r0;
    FT_Vector  c1;
    FT_Pos     r1;

  } FT_PaintRadialGradient;

  typedef struct  FT_PaintSweepGradient_
  {
    FT_ColorLine  colorline;

    FT_Vector  center;
    FT_Fixed   start_angle;
    FT_Fixed   end_angle;

  } FT_PaintSweepGradient;

  typedef struct  FT_PaintGlyph_
  {
    FT_OpaquePaint  paint;
    FT_UInt         glyphID;

  } FT_PaintGlyph;

  typedef struct  FT_PaintColrGlyph_
  {
    FT_UInt  glyphID;

  } FT_PaintColrGlyph;

  typedef struct  FT_PaintTransform_
  {
    FT_OpaquePaint  paint;
    FT_Affine23     affine;

  } FT_PaintTransform;

  typedef struct  FT_PaintTranslate_
  {
    FT_OpaquePaint  paint;

    FT_Fixed  dx;
    FT_Fixed  dy;

  } FT_PaintTranslate;

  typedef struct  FT_PaintScale_
  {
    FT_OpaquePaint  paint;

    FT_Fixed  scale_x;
    FT_Fixed  scale_y;

    FT_Fixed  center_x;
    FT_Fixed  center_y;

  } FT_PaintScale;

  typedef struct  FT_PaintRotate_
  {
    FT_OpaquePaint  paint;

    FT_Fixed  angle;

    FT_Fixed  center_x;
    FT_Fixed  center_y;

  } FT_PaintRotate;

  typedef struct  FT_PaintSkew_
  {
    FT_OpaquePaint  paint;

    FT_Fixed  x_skew_angle;
    FT_Fixed  y_skew_angle;

    FT_Fixed  center_x;
    FT_Fixed  center_y;

  } FT_PaintSkew;

  typedef struct  FT_PaintComposite_
  {
    FT_OpaquePaint     source_paint;
    FT_Composite_Mode  composite_mode;
    FT_OpaquePaint     backdrop_paint;

  } FT_PaintComposite;

  typedef struct  FT_COLR_Paint_
  {
    FT_PaintFormat format;

    union
    {
      FT_PaintColrLayers      colr_layers;
      FT_PaintGlyph           glyph;
      FT_PaintSolid           solid;
      FT_PaintLinearGradient  linear_gradient;
      FT_PaintRadialGradient  radial_gradient;
      FT_PaintSweepGradient   sweep_gradient;
      FT_PaintTransform       transform;
      FT_PaintTranslate       translate;
      FT_PaintScale           scale;
      FT_PaintRotate          rotate;
      FT_PaintSkew            skew;
      FT_PaintComposite       composite;
      FT_PaintColrGlyph       colr_glyph;

    } u;

  } FT_COLR_Paint;

  typedef enum  FT_Color_Root_Transform_
  {
    FT_COLOR_INCLUDE_ROOT_TRANSFORM,
    FT_COLOR_NO_ROOT_TRANSFORM,

    FT_COLOR_ROOT_TRANSFORM_MAX

  } FT_Color_Root_Transform;

  typedef struct  FT_ClipBox_
  {
    FT_Vector  bottom_left;
    FT_Vector  top_left;
    FT_Vector  top_right;
    FT_Vector  bottom_right;

  } FT_ClipBox;

  FT_EXPORT( FT_Bool )
  FT_Get_Color_Glyph_Paint( FT_Face                  face,
                            FT_UInt                  base_glyph,
                            FT_Color_Root_Transform  root_transform,
                            FT_OpaquePaint*          paint );

  FT_EXPORT( FT_Bool )
  FT_Get_Color_Glyph_ClipBox( FT_Face      face,
                              FT_UInt      base_glyph,
                              FT_ClipBox*  clip_box );

  FT_EXPORT( FT_Bool )
  FT_Get_Paint_Layers( FT_Face            face,
                       FT_LayerIterator*  iterator,
                       FT_OpaquePaint*    paint );

  FT_EXPORT( FT_Bool )
  FT_Get_Colorline_Stops( FT_Face                face,
                          FT_ColorStop*          color_stop,
                          FT_ColorStopIterator*  iterator );

  FT_EXPORT( FT_Bool )
  FT_Get_Paint( FT_Face         face,
                FT_OpaquePaint  opaque_paint,
                FT_COLR_Paint*  paint );

FT_END_HEADER

#endif

