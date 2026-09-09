
#ifndef FTSNAMES_H_
#define FTSNAMES_H_

#include <freetype/freetype.h>
#include <freetype/ftparams.h>

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

FT_BEGIN_HEADER

  typedef struct  FT_SfntName_
  {
    FT_UShort  platform_id;
    FT_UShort  encoding_id;
    FT_UShort  language_id;
    FT_UShort  name_id;

    FT_Byte*   string;
    FT_UInt    string_len;

  } FT_SfntName;

  FT_EXPORT( FT_UInt )
  FT_Get_Sfnt_Name_Count( FT_Face  face );

  FT_EXPORT( FT_Error )
  FT_Get_Sfnt_Name( FT_Face       face,
                    FT_UInt       idx,
                    FT_SfntName  *aname );

  typedef struct  FT_SfntLangTag_
  {
    FT_Byte*  string;
    FT_UInt   string_len;

  } FT_SfntLangTag;

  FT_EXPORT( FT_Error )
  FT_Get_Sfnt_LangTag( FT_Face          face,
                       FT_UInt          langID,
                       FT_SfntLangTag  *alangTag );

FT_END_HEADER

#endif

