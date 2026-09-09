
#ifndef FTBZIP2_H_
#define FTBZIP2_H_

#include <freetype/freetype.h>

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

FT_BEGIN_HEADER

  FT_EXPORT( FT_Error )
  FT_Stream_OpenBzip2( FT_Stream  stream,
                       FT_Stream  source );

FT_END_HEADER

#endif

