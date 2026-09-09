
#ifndef FTTRIGON_H_
#define FTTRIGON_H_

#include <freetype/freetype.h>

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

FT_BEGIN_HEADER

  typedef FT_Fixed  FT_Angle;

#define FT_ANGLE_PI  ( 180L << 16 )

#define FT_ANGLE_2PI  ( FT_ANGLE_PI * 2 )

#define FT_ANGLE_PI2  ( FT_ANGLE_PI / 2 )

#define FT_ANGLE_PI4  ( FT_ANGLE_PI / 4 )

  FT_EXPORT( FT_Fixed )
  FT_Sin( FT_Angle  angle );

  FT_EXPORT( FT_Fixed )
  FT_Cos( FT_Angle  angle );

  FT_EXPORT( FT_Fixed )
  FT_Tan( FT_Angle  angle );

  FT_EXPORT( FT_Angle )
  FT_Atan2( FT_Fixed  x,
            FT_Fixed  y );

  FT_EXPORT( FT_Angle )
  FT_Angle_Diff( FT_Angle  angle1,
                 FT_Angle  angle2 );

  FT_EXPORT( void )
  FT_Vector_Unit( FT_Vector*  vec,
                  FT_Angle    angle );

  FT_EXPORT( void )
  FT_Vector_Rotate( FT_Vector*  vec,
                    FT_Angle    angle );

  FT_EXPORT( FT_Fixed )
  FT_Vector_Length( FT_Vector*  vec );

  FT_EXPORT( void )
  FT_Vector_Polarize( FT_Vector*  vec,
                      FT_Fixed   *length,
                      FT_Angle   *angle );

  FT_EXPORT( void )
  FT_Vector_From_Polar( FT_Vector*  vec,
                        FT_Fixed    length,
                        FT_Angle    angle );

FT_END_HEADER

#endif

