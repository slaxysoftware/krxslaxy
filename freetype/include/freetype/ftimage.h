
#ifndef FTIMAGE_H_
#define FTIMAGE_H_

FT_BEGIN_HEADER

  typedef signed long  FT_Pos;

  typedef struct  FT_Vector_
  {
    FT_Pos  x;
    FT_Pos  y;

  } FT_Vector;

  typedef struct  FT_BBox_
  {
    FT_Pos  xMin, yMin;
    FT_Pos  xMax, yMax;

  } FT_BBox;

  typedef enum  FT_Pixel_Mode_
  {
    FT_PIXEL_MODE_NONE = 0,
    FT_PIXEL_MODE_MONO,
    FT_PIXEL_MODE_GRAY,
    FT_PIXEL_MODE_GRAY2,
    FT_PIXEL_MODE_GRAY4,
    FT_PIXEL_MODE_LCD,
    FT_PIXEL_MODE_LCD_V,
    FT_PIXEL_MODE_BGRA,

    FT_PIXEL_MODE_MAX

  } FT_Pixel_Mode;

#define ft_pixel_mode_none   FT_PIXEL_MODE_NONE
#define ft_pixel_mode_mono   FT_PIXEL_MODE_MONO
#define ft_pixel_mode_grays  FT_PIXEL_MODE_GRAY
#define ft_pixel_mode_pal2   FT_PIXEL_MODE_GRAY2
#define ft_pixel_mode_pal4   FT_PIXEL_MODE_GRAY4

  typedef struct  FT_Bitmap_
  {
    unsigned int    rows;
    unsigned int    width;
    int             pitch;
    unsigned char*  buffer;
    unsigned short  num_grays;
    unsigned char   pixel_mode;
    unsigned char   palette_mode;
    void*           palette;

  } FT_Bitmap;

  typedef struct  FT_Outline_
  {
    short       n_contours;
    short       n_points;

    FT_Vector*  points;
    char*       tags;
    short*      contours;

    int         flags;

  } FT_Outline;

#define FT_OUTLINE_CONTOURS_MAX  SHRT_MAX
#define FT_OUTLINE_POINTS_MAX    SHRT_MAX

#define FT_OUTLINE_NONE             0x0
#define FT_OUTLINE_OWNER            0x1
#define FT_OUTLINE_EVEN_ODD_FILL    0x2
#define FT_OUTLINE_REVERSE_FILL     0x4
#define FT_OUTLINE_IGNORE_DROPOUTS  0x8
#define FT_OUTLINE_SMART_DROPOUTS   0x10
#define FT_OUTLINE_INCLUDE_STUBS    0x20
#define FT_OUTLINE_OVERLAP          0x40

#define FT_OUTLINE_HIGH_PRECISION   0x100
#define FT_OUTLINE_SINGLE_PASS      0x200

#define ft_outline_none             FT_OUTLINE_NONE
#define ft_outline_owner            FT_OUTLINE_OWNER
#define ft_outline_even_odd_fill    FT_OUTLINE_EVEN_ODD_FILL
#define ft_outline_reverse_fill     FT_OUTLINE_REVERSE_FILL
#define ft_outline_ignore_dropouts  FT_OUTLINE_IGNORE_DROPOUTS
#define ft_outline_high_precision   FT_OUTLINE_HIGH_PRECISION
#define ft_outline_single_pass      FT_OUTLINE_SINGLE_PASS

#define FT_CURVE_TAG( flag )  ( flag & 0x03 )

#define FT_CURVE_TAG_ON            0x01
#define FT_CURVE_TAG_CONIC         0x00
#define FT_CURVE_TAG_CUBIC         0x02

#define FT_CURVE_TAG_HAS_SCANMODE  0x04

#define FT_CURVE_TAG_TOUCH_X       0x08
#define FT_CURVE_TAG_TOUCH_Y       0x10

#define FT_CURVE_TAG_TOUCH_BOTH    ( FT_CURVE_TAG_TOUCH_X | \
                                     FT_CURVE_TAG_TOUCH_Y )

#define FT_Curve_Tag_On       FT_CURVE_TAG_ON
#define FT_Curve_Tag_Conic    FT_CURVE_TAG_CONIC
#define FT_Curve_Tag_Cubic    FT_CURVE_TAG_CUBIC
#define FT_Curve_Tag_Touch_X  FT_CURVE_TAG_TOUCH_X
#define FT_Curve_Tag_Touch_Y  FT_CURVE_TAG_TOUCH_Y

  typedef int
  (*FT_Outline_MoveToFunc)( const FT_Vector*  to,
                            void*             user );

#define FT_Outline_MoveTo_Func  FT_Outline_MoveToFunc

  typedef int
  (*FT_Outline_LineToFunc)( const FT_Vector*  to,
                            void*             user );

#define FT_Outline_LineTo_Func  FT_Outline_LineToFunc

  typedef int
  (*FT_Outline_ConicToFunc)( const FT_Vector*  control,
                             const FT_Vector*  to,
                             void*             user );

#define FT_Outline_ConicTo_Func  FT_Outline_ConicToFunc

  typedef int
  (*FT_Outline_CubicToFunc)( const FT_Vector*  control1,
                             const FT_Vector*  control2,
                             const FT_Vector*  to,
                             void*             user );

#define FT_Outline_CubicTo_Func  FT_Outline_CubicToFunc

  typedef struct  FT_Outline_Funcs_
  {
    FT_Outline_MoveToFunc   move_to;
    FT_Outline_LineToFunc   line_to;
    FT_Outline_ConicToFunc  conic_to;
    FT_Outline_CubicToFunc  cubic_to;

    int                     shift;
    FT_Pos                  delta;

  } FT_Outline_Funcs;

#ifndef FT_IMAGE_TAG

#define FT_IMAGE_TAG( value, _x1, _x2, _x3, _x4 )                         \
          value = ( ( FT_STATIC_BYTE_CAST( unsigned long, _x1 ) << 24 ) | \
                    ( FT_STATIC_BYTE_CAST( unsigned long, _x2 ) << 16 ) | \
                    ( FT_STATIC_BYTE_CAST( unsigned long, _x3 ) << 8  ) | \
                      FT_STATIC_BYTE_CAST( unsigned long, _x4 )         )

#endif

  typedef enum  FT_Glyph_Format_
  {
    FT_IMAGE_TAG( FT_GLYPH_FORMAT_NONE, 0, 0, 0, 0 ),

    FT_IMAGE_TAG( FT_GLYPH_FORMAT_COMPOSITE, 'c', 'o', 'm', 'p' ),
    FT_IMAGE_TAG( FT_GLYPH_FORMAT_BITMAP,    'b', 'i', 't', 's' ),
    FT_IMAGE_TAG( FT_GLYPH_FORMAT_OUTLINE,   'o', 'u', 't', 'l' ),
    FT_IMAGE_TAG( FT_GLYPH_FORMAT_PLOTTER,   'p', 'l', 'o', 't' ),
    FT_IMAGE_TAG( FT_GLYPH_FORMAT_SVG,       'S', 'V', 'G', ' ' )

  } FT_Glyph_Format;

#define ft_glyph_format_none       FT_GLYPH_FORMAT_NONE
#define ft_glyph_format_composite  FT_GLYPH_FORMAT_COMPOSITE
#define ft_glyph_format_bitmap     FT_GLYPH_FORMAT_BITMAP
#define ft_glyph_format_outline    FT_GLYPH_FORMAT_OUTLINE
#define ft_glyph_format_plotter    FT_GLYPH_FORMAT_PLOTTER

  typedef struct  FT_Span_
  {
    short           x;
    unsigned short  len;
    unsigned char   coverage;

  } FT_Span;

  typedef void
  (*FT_SpanFunc)( int             y,
                  int             count,
                  const FT_Span*  spans,
                  void*           user );

#define FT_Raster_Span_Func  FT_SpanFunc

  typedef int
  (*FT_Raster_BitTest_Func)( int    y,
                             int    x,
                             void*  user );

  typedef void
  (*FT_Raster_BitSet_Func)( int    y,
                            int    x,
                            void*  user );

#define FT_RASTER_FLAG_DEFAULT  0x0
#define FT_RASTER_FLAG_AA       0x1
#define FT_RASTER_FLAG_DIRECT   0x2
#define FT_RASTER_FLAG_CLIP     0x4
#define FT_RASTER_FLAG_SDF      0x8

#define ft_raster_flag_default  FT_RASTER_FLAG_DEFAULT
#define ft_raster_flag_aa       FT_RASTER_FLAG_AA
#define ft_raster_flag_direct   FT_RASTER_FLAG_DIRECT
#define ft_raster_flag_clip     FT_RASTER_FLAG_CLIP

  typedef struct  FT_Raster_Params_
  {
    const FT_Bitmap*        target;
    const void*             source;
    int                     flags;
    FT_SpanFunc             gray_spans;
    FT_SpanFunc             black_spans;
    FT_Raster_BitTest_Func  bit_test;
    FT_Raster_BitSet_Func   bit_set;
    void*                   user;
    FT_BBox                 clip_box;

  } FT_Raster_Params;

  typedef struct FT_RasterRec_*  FT_Raster;

  typedef int
  (*FT_Raster_NewFunc)( void*       memory,
                        FT_Raster*  raster );

#define FT_Raster_New_Func  FT_Raster_NewFunc

  typedef void
  (*FT_Raster_DoneFunc)( FT_Raster  raster );

#define FT_Raster_Done_Func  FT_Raster_DoneFunc

  typedef void
  (*FT_Raster_ResetFunc)( FT_Raster       raster,
                          unsigned char*  pool_base,
                          unsigned long   pool_size );

#define FT_Raster_Reset_Func  FT_Raster_ResetFunc

  typedef int
  (*FT_Raster_SetModeFunc)( FT_Raster      raster,
                            unsigned long  mode,
                            void*          args );

#define FT_Raster_Set_Mode_Func  FT_Raster_SetModeFunc

  typedef int
  (*FT_Raster_RenderFunc)( FT_Raster                raster,
                           const FT_Raster_Params*  params );

#define FT_Raster_Render_Func  FT_Raster_RenderFunc

  typedef struct  FT_Raster_Funcs_
  {
    FT_Glyph_Format        glyph_format;

    FT_Raster_NewFunc      raster_new;
    FT_Raster_ResetFunc    raster_reset;
    FT_Raster_SetModeFunc  raster_set_mode;
    FT_Raster_RenderFunc   raster_render;
    FT_Raster_DoneFunc     raster_done;

  } FT_Raster_Funcs;

FT_END_HEADER

#endif

