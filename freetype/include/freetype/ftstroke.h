
#ifndef FTSTROKE_H_
#define FTSTROKE_H_

#include <freetype/ftoutln.h>
#include <freetype/ftglyph.h>

FT_BEGIN_HEADER

  typedef struct FT_StrokerRec_*  FT_Stroker;

  typedef enum  FT_Stroker_LineJoin_
  {
    FT_STROKER_LINEJOIN_ROUND          = 0,
    FT_STROKER_LINEJOIN_BEVEL          = 1,
    FT_STROKER_LINEJOIN_MITER_VARIABLE = 2,
    FT_STROKER_LINEJOIN_MITER          = FT_STROKER_LINEJOIN_MITER_VARIABLE,
    FT_STROKER_LINEJOIN_MITER_FIXED    = 3

  } FT_Stroker_LineJoin;

  typedef enum  FT_Stroker_LineCap_
  {
    FT_STROKER_LINECAP_BUTT = 0,
    FT_STROKER_LINECAP_ROUND,
    FT_STROKER_LINECAP_SQUARE

  } FT_Stroker_LineCap;

  typedef enum  FT_StrokerBorder_
  {
    FT_STROKER_BORDER_LEFT = 0,
    FT_STROKER_BORDER_RIGHT

  } FT_StrokerBorder;

  FT_EXPORT( FT_StrokerBorder )
  FT_Outline_GetInsideBorder( FT_Outline*  outline );

  FT_EXPORT( FT_StrokerBorder )
  FT_Outline_GetOutsideBorder( FT_Outline*  outline );

  FT_EXPORT( FT_Error )
  FT_Stroker_New( FT_Library   library,
                  FT_Stroker  *astroker );

  FT_EXPORT( void )
  FT_Stroker_Set( FT_Stroker           stroker,
                  FT_Fixed             radius,
                  FT_Stroker_LineCap   line_cap,
                  FT_Stroker_LineJoin  line_join,
                  FT_Fixed             miter_limit );

  FT_EXPORT( void )
  FT_Stroker_Rewind( FT_Stroker  stroker );

  FT_EXPORT( FT_Error )
  FT_Stroker_ParseOutline( FT_Stroker   stroker,
                           FT_Outline*  outline,
                           FT_Bool      opened );

  FT_EXPORT( FT_Error )
  FT_Stroker_BeginSubPath( FT_Stroker  stroker,
                           FT_Vector*  to,
                           FT_Bool     open );

  FT_EXPORT( FT_Error )
  FT_Stroker_EndSubPath( FT_Stroker  stroker );

  FT_EXPORT( FT_Error )
  FT_Stroker_LineTo( FT_Stroker  stroker,
                     FT_Vector*  to );

  FT_EXPORT( FT_Error )
  FT_Stroker_ConicTo( FT_Stroker  stroker,
                      FT_Vector*  control,
                      FT_Vector*  to );

  FT_EXPORT( FT_Error )
  FT_Stroker_CubicTo( FT_Stroker  stroker,
                      FT_Vector*  control1,
                      FT_Vector*  control2,
                      FT_Vector*  to );

  FT_EXPORT( FT_Error )
  FT_Stroker_GetBorderCounts( FT_Stroker        stroker,
                              FT_StrokerBorder  border,
                              FT_UInt          *anum_points,
                              FT_UInt          *anum_contours );

  FT_EXPORT( void )
  FT_Stroker_ExportBorder( FT_Stroker        stroker,
                           FT_StrokerBorder  border,
                           FT_Outline*       outline );

  FT_EXPORT( FT_Error )
  FT_Stroker_GetCounts( FT_Stroker  stroker,
                        FT_UInt    *anum_points,
                        FT_UInt    *anum_contours );

  FT_EXPORT( void )
  FT_Stroker_Export( FT_Stroker   stroker,
                     FT_Outline*  outline );

  FT_EXPORT( void )
  FT_Stroker_Done( FT_Stroker  stroker );

  FT_EXPORT( FT_Error )
  FT_Glyph_Stroke( FT_Glyph    *pglyph,
                   FT_Stroker   stroker,
                   FT_Bool      destroy );

  FT_EXPORT( FT_Error )
  FT_Glyph_StrokeBorder( FT_Glyph    *pglyph,
                         FT_Stroker   stroker,
                         FT_Bool      inside,
                         FT_Bool      destroy );

FT_END_HEADER

#endif

