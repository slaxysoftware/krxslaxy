
#ifndef FTBDF_H_
#define FTBDF_H_

#include <freetype/freetype.h>

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

FT_BEGIN_HEADER

  typedef enum  BDF_PropertyType_
  {
    BDF_PROPERTY_TYPE_NONE     = 0,
    BDF_PROPERTY_TYPE_ATOM     = 1,
    BDF_PROPERTY_TYPE_INTEGER  = 2,
    BDF_PROPERTY_TYPE_CARDINAL = 3

  } BDF_PropertyType;

  typedef struct BDF_PropertyRec_*  BDF_Property;

  typedef struct  BDF_PropertyRec_
  {
    BDF_PropertyType  type;
    union {
      const char*     atom;
      FT_Int32        integer;
      FT_UInt32       cardinal;

    } u;

  } BDF_PropertyRec;

  FT_EXPORT( FT_Error )
  FT_Get_BDF_Charset_ID( FT_Face       face,
                         const char*  *acharset_encoding,
                         const char*  *acharset_registry );

  FT_EXPORT( FT_Error )
  FT_Get_BDF_Property( FT_Face           face,
                       const char*       prop_name,
                       BDF_PropertyRec  *aproperty );

FT_END_HEADER

#endif

