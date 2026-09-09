
#ifndef FTOUTLN_H_
#define FTOUTLN_H_

#include <freetype/freetype.h>

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

FT_BEGIN_HEADER

  FT_EXPORT( FT_Error )
  FT_Outline_Decompose( FT_Outline*              outline,
                        const FT_Outline_Funcs*  func_interface,
                        void*                    user );

  FT_EXPORT( FT_Error )
  FT_Outline_New( FT_Library   library,
                  FT_UInt      numPoints,
                  FT_Int       numContours,
                  FT_Outline  *anoutline );

  FT_EXPORT( FT_Error )
  FT_Outline_Done( FT_Library   library,
                   FT_Outline*  outline );

  FT_EXPORT( FT_Error )
  FT_Outline_Check( FT_Outline*  outline );

  FT_EXPORT( void )
  FT_Outline_Get_CBox( const FT_Outline*  outline,
                       FT_BBox           *acbox );

  FT_EXPORT( void )
  FT_Outline_Translate( const FT_Outline*  outline,
                        FT_Pos             xOffset,
                        FT_Pos             yOffset );

  FT_EXPORT( FT_Error )
  FT_Outline_Copy( const FT_Outline*  source,
                   FT_Outline        *target );

  FT_EXPORT( void )
  FT_Outline_Transform( const FT_Outline*  outline,
                        const FT_Matrix*   matrix );

  FT_EXPORT( FT_Error )
  FT_Outline_Embolden( FT_Outline*  outline,
                       FT_Pos       strength );

  FT_EXPORT( FT_Error )
  FT_Outline_EmboldenXY( FT_Outline*  outline,
                         FT_Pos       xstrength,
                         FT_Pos       ystrength );

  FT_EXPORT( void )
  FT_Outline_Reverse( FT_Outline*  outline );

  FT_EXPORT( FT_Error )
  FT_Outline_Get_Bitmap( FT_Library        library,
                         FT_Outline*       outline,
                         const FT_Bitmap  *abitmap );

  FT_EXPORT( FT_Error )
  FT_Outline_Render( FT_Library         library,
                     FT_Outline*        outline,
                     FT_Raster_Params*  params );

  typedef enum  FT_Orientation_
  {
    FT_ORIENTATION_TRUETYPE   = 0,
    FT_ORIENTATION_POSTSCRIPT = 1,
    FT_ORIENTATION_FILL_RIGHT = FT_ORIENTATION_TRUETYPE,
    FT_ORIENTATION_FILL_LEFT  = FT_ORIENTATION_POSTSCRIPT,
    FT_ORIENTATION_NONE

  } FT_Orientation;

  FT_EXPORT( FT_Orientation )
  FT_Outline_Get_Orientation( FT_Outline*  outline );

FT_END_HEADER

#endif

