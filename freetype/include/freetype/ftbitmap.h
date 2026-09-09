
#ifndef FTBITMAP_H_
#define FTBITMAP_H_

#include <freetype/freetype.h>
#include <freetype/ftcolor.h>

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

FT_BEGIN_HEADER

  FT_EXPORT( void )
  FT_Bitmap_Init( FT_Bitmap  *abitmap );

  FT_EXPORT( void )
  FT_Bitmap_New( FT_Bitmap  *abitmap );

  FT_EXPORT( FT_Error )
  FT_Bitmap_Copy( FT_Library        library,
                  const FT_Bitmap  *source,
                  FT_Bitmap        *target );

  FT_EXPORT( FT_Error )
  FT_Bitmap_Embolden( FT_Library  library,
                      FT_Bitmap*  bitmap,
                      FT_Pos      xStrength,
                      FT_Pos      yStrength );

  FT_EXPORT( FT_Error )
  FT_Bitmap_Convert( FT_Library        library,
                     const FT_Bitmap  *source,
                     FT_Bitmap        *target,
                     FT_Int            alignment );

  FT_EXPORT( FT_Error )
  FT_Bitmap_Blend( FT_Library         library,
                   const FT_Bitmap*   source,
                   const FT_Vector    source_offset,
                   FT_Bitmap*         target,
                   FT_Vector         *atarget_offset,
                   FT_Color           color );

  FT_EXPORT( FT_Error )
  FT_GlyphSlot_Own_Bitmap( FT_GlyphSlot  slot );

  FT_EXPORT( FT_Error )
  FT_Bitmap_Done( FT_Library  library,
                  FT_Bitmap  *bitmap );

FT_END_HEADER

#endif

