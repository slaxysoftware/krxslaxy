
#ifndef TTTABLES_H_
#define TTTABLES_H_

#include <freetype/freetype.h>

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

FT_BEGIN_HEADER

  typedef struct  TT_Header_
  {
    FT_Fixed   Table_Version;
    FT_Fixed   Font_Revision;

    FT_Long    CheckSum_Adjust;
    FT_Long    Magic_Number;

    FT_UShort  Flags;
    FT_UShort  Units_Per_EM;

    FT_ULong   Created [2];
    FT_ULong   Modified[2];

    FT_Short   xMin;
    FT_Short   yMin;
    FT_Short   xMax;
    FT_Short   yMax;

    FT_UShort  Mac_Style;
    FT_UShort  Lowest_Rec_PPEM;

    FT_Short   Font_Direction;
    FT_Short   Index_To_Loc_Format;
    FT_Short   Glyph_Data_Format;

  } TT_Header;

  typedef struct  TT_HoriHeader_
  {
    FT_Fixed   Version;
    FT_Short   Ascender;
    FT_Short   Descender;
    FT_Short   Line_Gap;

    FT_UShort  advance_Width_Max;

    FT_Short   min_Left_Side_Bearing;
    FT_Short   min_Right_Side_Bearing;
    FT_Short   xMax_Extent;
    FT_Short   caret_Slope_Rise;
    FT_Short   caret_Slope_Run;
    FT_Short   caret_Offset;

    FT_Short   Reserved[4];

    FT_Short   metric_Data_Format;
    FT_UShort  number_Of_HMetrics;

    void*      long_metrics;
    void*      short_metrics;

  } TT_HoriHeader;

  typedef struct  TT_VertHeader_
  {
    FT_Fixed   Version;
    FT_Short   Ascender;
    FT_Short   Descender;
    FT_Short   Line_Gap;

    FT_UShort  advance_Height_Max;

    FT_Short   min_Top_Side_Bearing;
    FT_Short   min_Bottom_Side_Bearing;
    FT_Short   yMax_Extent;
    FT_Short   caret_Slope_Rise;
    FT_Short   caret_Slope_Run;
    FT_Short   caret_Offset;

    FT_Short   Reserved[4];

    FT_Short   metric_Data_Format;
    FT_UShort  number_Of_VMetrics;

    void*      long_metrics;
    void*      short_metrics;

  } TT_VertHeader;

  typedef struct  TT_OS2_
  {
    FT_UShort  version;
    FT_Short   xAvgCharWidth;
    FT_UShort  usWeightClass;
    FT_UShort  usWidthClass;
    FT_UShort  fsType;
    FT_Short   ySubscriptXSize;
    FT_Short   ySubscriptYSize;
    FT_Short   ySubscriptXOffset;
    FT_Short   ySubscriptYOffset;
    FT_Short   ySuperscriptXSize;
    FT_Short   ySuperscriptYSize;
    FT_Short   ySuperscriptXOffset;
    FT_Short   ySuperscriptYOffset;
    FT_Short   yStrikeoutSize;
    FT_Short   yStrikeoutPosition;
    FT_Short   sFamilyClass;

    FT_Byte    panose[10];

    FT_ULong   ulUnicodeRange1;
    FT_ULong   ulUnicodeRange2;
    FT_ULong   ulUnicodeRange3;
    FT_ULong   ulUnicodeRange4;

    FT_Char    achVendID[4];

    FT_UShort  fsSelection;
    FT_UShort  usFirstCharIndex;
    FT_UShort  usLastCharIndex;
    FT_Short   sTypoAscender;
    FT_Short   sTypoDescender;
    FT_Short   sTypoLineGap;
    FT_UShort  usWinAscent;
    FT_UShort  usWinDescent;

    FT_ULong   ulCodePageRange1;
    FT_ULong   ulCodePageRange2;

    FT_Short   sxHeight;
    FT_Short   sCapHeight;
    FT_UShort  usDefaultChar;
    FT_UShort  usBreakChar;
    FT_UShort  usMaxContext;

    FT_UShort  usLowerOpticalPointSize;
    FT_UShort  usUpperOpticalPointSize;

  } TT_OS2;

  typedef struct  TT_Postscript_
  {
    FT_Fixed  FormatType;
    FT_Fixed  italicAngle;
    FT_Short  underlinePosition;
    FT_Short  underlineThickness;
    FT_ULong  isFixedPitch;
    FT_ULong  minMemType42;
    FT_ULong  maxMemType42;
    FT_ULong  minMemType1;
    FT_ULong  maxMemType1;

  } TT_Postscript;

  typedef struct  TT_PCLT_
  {
    FT_Fixed   Version;
    FT_ULong   FontNumber;
    FT_UShort  Pitch;
    FT_UShort  xHeight;
    FT_UShort  Style;
    FT_UShort  TypeFamily;
    FT_UShort  CapHeight;
    FT_UShort  SymbolSet;
    FT_Char    TypeFace[16];
    FT_Char    CharacterComplement[8];
    FT_Char    FileName[6];
    FT_Char    StrokeWeight;
    FT_Char    WidthType;
    FT_Byte    SerifStyle;
    FT_Byte    Reserved;

  } TT_PCLT;

  typedef struct  TT_MaxProfile_
  {
    FT_Fixed   version;
    FT_UShort  numGlyphs;
    FT_UShort  maxPoints;
    FT_UShort  maxContours;
    FT_UShort  maxCompositePoints;
    FT_UShort  maxCompositeContours;
    FT_UShort  maxZones;
    FT_UShort  maxTwilightPoints;
    FT_UShort  maxStorage;
    FT_UShort  maxFunctionDefs;
    FT_UShort  maxInstructionDefs;
    FT_UShort  maxStackElements;
    FT_UShort  maxSizeOfInstructions;
    FT_UShort  maxComponentElements;
    FT_UShort  maxComponentDepth;

  } TT_MaxProfile;

  typedef enum  FT_Sfnt_Tag_
  {
    FT_SFNT_HEAD,
    FT_SFNT_MAXP,
    FT_SFNT_OS2,
    FT_SFNT_HHEA,
    FT_SFNT_VHEA,
    FT_SFNT_POST,
    FT_SFNT_PCLT,

    FT_SFNT_MAX

  } FT_Sfnt_Tag;

#define ft_sfnt_head  FT_SFNT_HEAD
#define ft_sfnt_maxp  FT_SFNT_MAXP
#define ft_sfnt_os2   FT_SFNT_OS2
#define ft_sfnt_hhea  FT_SFNT_HHEA
#define ft_sfnt_vhea  FT_SFNT_VHEA
#define ft_sfnt_post  FT_SFNT_POST
#define ft_sfnt_pclt  FT_SFNT_PCLT

  FT_EXPORT( void* )
  FT_Get_Sfnt_Table( FT_Face      face,
                     FT_Sfnt_Tag  tag );

  FT_EXPORT( FT_Error )
  FT_Load_Sfnt_Table( FT_Face    face,
                      FT_ULong   tag,
                      FT_Long    offset,
                      FT_Byte*   buffer,
                      FT_ULong*  length );

  FT_EXPORT( FT_Error )
  FT_Sfnt_Table_Info( FT_Face    face,
                      FT_UInt    table_index,
                      FT_ULong  *tag,
                      FT_ULong  *length );

  FT_EXPORT( FT_ULong )
  FT_Get_CMap_Language_ID( FT_CharMap  charmap );

  FT_EXPORT( FT_Long )
  FT_Get_CMap_Format( FT_CharMap  charmap );

FT_END_HEADER

#endif

