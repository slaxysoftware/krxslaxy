
#ifndef OTSVG_H_
#define OTSVG_H_

#include <freetype/freetype.h>

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

FT_BEGIN_HEADER

  typedef FT_Error
  (*SVG_Lib_Init_Func)( FT_Pointer  *data_pointer );

  typedef void
  (*SVG_Lib_Free_Func)( FT_Pointer  *data_pointer );

  typedef FT_Error
  (*SVG_Lib_Render_Func)( FT_GlyphSlot  slot,
                          FT_Pointer   *data_pointer );

  typedef FT_Error
  (*SVG_Lib_Preset_Slot_Func)( FT_GlyphSlot  slot,
                               FT_Bool       cache,
                               FT_Pointer   *state );

  typedef struct SVG_RendererHooks_
  {
    SVG_Lib_Init_Func    init_svg;
    SVG_Lib_Free_Func    free_svg;
    SVG_Lib_Render_Func  render_svg;

    SVG_Lib_Preset_Slot_Func  preset_slot;

  } SVG_RendererHooks;

  typedef struct  FT_SVG_DocumentRec_
  {
    FT_Byte*  svg_document;
    FT_ULong  svg_document_length;

    FT_Size_Metrics  metrics;
    FT_UShort        units_per_EM;

    FT_UShort  start_glyph_id;
    FT_UShort  end_glyph_id;

    FT_Matrix  transform;
    FT_Vector  delta;

  } FT_SVG_DocumentRec;

  typedef struct FT_SVG_DocumentRec_*  FT_SVG_Document;

FT_END_HEADER

#endif

