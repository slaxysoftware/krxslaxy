
#ifndef FTDRIVER_H_
#define FTDRIVER_H_

#include <freetype/freetype.h>
#include <freetype/ftparams.h>

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

FT_BEGIN_HEADER

#define FT_HINTING_FREETYPE  0
#define FT_HINTING_ADOBE     1

#define FT_CFF_HINTING_FREETYPE  FT_HINTING_FREETYPE
#define FT_CFF_HINTING_ADOBE     FT_HINTING_ADOBE

#define TT_INTERPRETER_VERSION_35  35
#define TT_INTERPRETER_VERSION_38  38
#define TT_INTERPRETER_VERSION_40  40

#define FT_AUTOHINTER_SCRIPT_NONE   0
#define FT_AUTOHINTER_SCRIPT_LATIN  1
#define FT_AUTOHINTER_SCRIPT_CJK    2
#define FT_AUTOHINTER_SCRIPT_INDIC  3

  typedef struct  FT_Prop_GlyphToScriptMap_
  {
    FT_Face     face;
    FT_UShort*  map;

  } FT_Prop_GlyphToScriptMap;

  typedef struct  FT_Prop_IncreaseXHeight_
  {
    FT_Face  face;
    FT_UInt  limit;

  } FT_Prop_IncreaseXHeight;

FT_END_HEADER

#endif

