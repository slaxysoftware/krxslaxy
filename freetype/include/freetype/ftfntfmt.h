
#ifndef FTFNTFMT_H_
#define FTFNTFMT_H_

#include <freetype/freetype.h>

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

FT_BEGIN_HEADER

  FT_EXPORT( const char* )
  FT_Get_Font_Format( FT_Face  face );

  FT_EXPORT( const char* )
  FT_Get_X11_Font_Format( FT_Face  face );

FT_END_HEADER

#endif

