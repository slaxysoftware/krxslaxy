
#ifndef FTGZIP_H_
#define FTGZIP_H_

#include <freetype/freetype.h>

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

FT_BEGIN_HEADER

  FT_EXPORT( FT_Error )
  FT_Stream_OpenGzip( FT_Stream  stream,
                      FT_Stream  source );

  FT_EXPORT( FT_Error )
  FT_Gzip_Uncompress( FT_Memory       memory,
                      FT_Byte*        output,
                      FT_ULong*       output_len,
                      const FT_Byte*  input,
                      FT_ULong        input_len );

FT_END_HEADER

#endif

