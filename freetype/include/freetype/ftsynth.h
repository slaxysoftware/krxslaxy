
#ifndef FTSYNTH_H_
#define FTSYNTH_H_

#include <freetype/freetype.h>

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

FT_BEGIN_HEADER

  FT_EXPORT( void )
  FT_GlyphSlot_Embolden( FT_GlyphSlot  slot );

  FT_EXPORT( void )
  FT_GlyphSlot_Oblique( FT_GlyphSlot  slot );

FT_END_HEADER

#endif

