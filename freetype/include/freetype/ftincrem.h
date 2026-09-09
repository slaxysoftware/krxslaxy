
#ifndef FTINCREM_H_
#define FTINCREM_H_

#include <freetype/freetype.h>
#include <freetype/ftparams.h>

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

FT_BEGIN_HEADER

  typedef struct FT_IncrementalRec_*  FT_Incremental;

  typedef struct  FT_Incremental_MetricsRec_
  {
    FT_Long  bearing_x;
    FT_Long  bearing_y;
    FT_Long  advance;
    FT_Long  advance_v;

  } FT_Incremental_MetricsRec;

   typedef struct FT_Incremental_MetricsRec_*  FT_Incremental_Metrics;

  typedef FT_Error
  (*FT_Incremental_GetGlyphDataFunc)( FT_Incremental  incremental,
                                      FT_UInt         glyph_index,
                                      FT_Data*        adata );

  typedef void
  (*FT_Incremental_FreeGlyphDataFunc)( FT_Incremental  incremental,
                                       FT_Data*        data );

  typedef FT_Error
  (*FT_Incremental_GetGlyphMetricsFunc)
                      ( FT_Incremental              incremental,
                        FT_UInt                     glyph_index,
                        FT_Bool                     vertical,
                        FT_Incremental_MetricsRec  *ametrics );

  typedef struct  FT_Incremental_FuncsRec_
  {
    FT_Incremental_GetGlyphDataFunc     get_glyph_data;
    FT_Incremental_FreeGlyphDataFunc    free_glyph_data;
    FT_Incremental_GetGlyphMetricsFunc  get_glyph_metrics;

  } FT_Incremental_FuncsRec;

  typedef struct  FT_Incremental_InterfaceRec_
  {
    const FT_Incremental_FuncsRec*  funcs;
    FT_Incremental                  object;

  } FT_Incremental_InterfaceRec;

  typedef FT_Incremental_InterfaceRec*   FT_Incremental_Interface;

FT_END_HEADER

#endif

