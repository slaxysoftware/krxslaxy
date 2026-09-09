
#ifndef FTBBOX_H_
#define FTBBOX_H_

#include <freetype/freetype.h>

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

FT_BEGIN_HEADER

  FT_EXPORT( FT_Error )
  FT_Outline_Get_BBox( FT_Outline*  outline,
                       FT_BBox     *abbox );

FT_END_HEADER

#endif

