
#ifndef FTRENDER_H_
#define FTRENDER_H_

#include <freetype/ftmodapi.h>
#include <freetype/ftglyph.h>

FT_BEGIN_HEADER

  typedef FT_Error
  (*FT_Glyph_InitFunc)( FT_Glyph      glyph,
                        FT_GlyphSlot  slot );

  typedef void
  (*FT_Glyph_DoneFunc)( FT_Glyph  glyph );

  typedef void
  (*FT_Glyph_TransformFunc)( FT_Glyph          glyph,
                             const FT_Matrix*  matrix,
                             const FT_Vector*  delta );

  typedef void
  (*FT_Glyph_GetBBoxFunc)( FT_Glyph  glyph,
                           FT_BBox*  abbox );

  typedef FT_Error
  (*FT_Glyph_CopyFunc)( FT_Glyph   source,
                        FT_Glyph   target );

  typedef FT_Error
  (*FT_Glyph_PrepareFunc)( FT_Glyph      glyph,
                           FT_GlyphSlot  slot );

#define FT_Glyph_Init_Func       FT_Glyph_InitFunc
#define FT_Glyph_Done_Func       FT_Glyph_DoneFunc
#define FT_Glyph_Transform_Func  FT_Glyph_TransformFunc
#define FT_Glyph_BBox_Func       FT_Glyph_GetBBoxFunc
#define FT_Glyph_Copy_Func       FT_Glyph_CopyFunc
#define FT_Glyph_Prepare_Func    FT_Glyph_PrepareFunc

  struct  FT_Glyph_Class_
  {
    FT_Long                 glyph_size;
    FT_Glyph_Format         glyph_format;

    FT_Glyph_InitFunc       glyph_init;
    FT_Glyph_DoneFunc       glyph_done;
    FT_Glyph_CopyFunc       glyph_copy;
    FT_Glyph_TransformFunc  glyph_transform;
    FT_Glyph_GetBBoxFunc    glyph_bbox;
    FT_Glyph_PrepareFunc    glyph_prepare;
  };

  typedef FT_Error
  (*FT_Renderer_RenderFunc)( FT_Renderer       renderer,
                             FT_GlyphSlot      slot,
                             FT_Render_Mode    mode,
                             const FT_Vector*  origin );

  typedef FT_Error
  (*FT_Renderer_TransformFunc)( FT_Renderer       renderer,
                                FT_GlyphSlot      slot,
                                const FT_Matrix*  matrix,
                                const FT_Vector*  delta );

  typedef void
  (*FT_Renderer_GetCBoxFunc)( FT_Renderer   renderer,
                              FT_GlyphSlot  slot,
                              FT_BBox*      cbox );

  typedef FT_Error
  (*FT_Renderer_SetModeFunc)( FT_Renderer  renderer,
                              FT_ULong     mode_tag,
                              FT_Pointer   mode_ptr );

#define FTRenderer_render  FT_Renderer_RenderFunc
#define FTRenderer_transform  FT_Renderer_TransformFunc
#define FTRenderer_getCBox  FT_Renderer_GetCBoxFunc
#define FTRenderer_setMode  FT_Renderer_SetModeFunc

  typedef struct  FT_Renderer_Class_
  {
    FT_Module_Class            root;

    FT_Glyph_Format            glyph_format;

    FT_Renderer_RenderFunc     render_glyph;
    FT_Renderer_TransformFunc  transform_glyph;
    FT_Renderer_GetCBoxFunc    get_glyph_cbox;
    FT_Renderer_SetModeFunc    set_mode;

    FT_Raster_Funcs*           raster_class;

  } FT_Renderer_Class;

  FT_EXPORT( FT_Renderer )
  FT_Get_Renderer( FT_Library       library,
                   FT_Glyph_Format  format );

  FT_EXPORT( FT_Error )
  FT_Set_Renderer( FT_Library     library,
                   FT_Renderer    renderer,
                   FT_UInt        num_params,
                   FT_Parameter*  parameters );

FT_END_HEADER

#endif

