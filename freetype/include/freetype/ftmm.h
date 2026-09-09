
#ifndef FTMM_H_
#define FTMM_H_

#include <freetype/t1tables.h>

FT_BEGIN_HEADER

  typedef struct  FT_MM_Axis_
  {
    FT_String*  name;
    FT_Long     minimum;
    FT_Long     maximum;

  } FT_MM_Axis;

  typedef struct  FT_Multi_Master_
  {
    FT_UInt     num_axis;
    FT_UInt     num_designs;
    FT_MM_Axis  axis[T1_MAX_MM_AXIS];

  } FT_Multi_Master;

  typedef struct  FT_Var_Axis_
  {
    FT_String*  name;

    FT_Fixed    minimum;
    FT_Fixed    def;
    FT_Fixed    maximum;

    FT_ULong    tag;
    FT_UInt     strid;

  } FT_Var_Axis;

  typedef struct  FT_Var_Named_Style_
  {
    FT_Fixed*  coords;
    FT_UInt    strid;
    FT_UInt    psid;

  } FT_Var_Named_Style;

  typedef struct  FT_MM_Var_
  {
    FT_UInt              num_axis;
    FT_UInt              num_designs;
    FT_UInt              num_namedstyles;
    FT_Var_Axis*         axis;
    FT_Var_Named_Style*  namedstyle;

  } FT_MM_Var;

  FT_EXPORT( FT_Error )
  FT_Get_Multi_Master( FT_Face           face,
                       FT_Multi_Master  *amaster );

  FT_EXPORT( FT_Error )
  FT_Get_MM_Var( FT_Face      face,
                 FT_MM_Var*  *amaster );

  FT_EXPORT( FT_Error )
  FT_Done_MM_Var( FT_Library   library,
                  FT_MM_Var   *amaster );

  FT_EXPORT( FT_Error )
  FT_Set_MM_Design_Coordinates( FT_Face   face,
                                FT_UInt   num_coords,
                                FT_Long*  coords );

  FT_EXPORT( FT_Error )
  FT_Set_Var_Design_Coordinates( FT_Face    face,
                                 FT_UInt    num_coords,
                                 FT_Fixed*  coords );

  FT_EXPORT( FT_Error )
  FT_Get_Var_Design_Coordinates( FT_Face    face,
                                 FT_UInt    num_coords,
                                 FT_Fixed*  coords );

  FT_EXPORT( FT_Error )
  FT_Set_MM_Blend_Coordinates( FT_Face    face,
                               FT_UInt    num_coords,
                               FT_Fixed*  coords );

  FT_EXPORT( FT_Error )
  FT_Get_MM_Blend_Coordinates( FT_Face    face,
                               FT_UInt    num_coords,
                               FT_Fixed*  coords );

  FT_EXPORT( FT_Error )
  FT_Set_Var_Blend_Coordinates( FT_Face    face,
                                FT_UInt    num_coords,
                                FT_Fixed*  coords );

  FT_EXPORT( FT_Error )
  FT_Get_Var_Blend_Coordinates( FT_Face    face,
                                FT_UInt    num_coords,
                                FT_Fixed*  coords );

  FT_EXPORT( FT_Error )
  FT_Set_MM_WeightVector( FT_Face    face,
                          FT_UInt    len,
                          FT_Fixed*  weightvector );

  FT_EXPORT( FT_Error )
  FT_Get_MM_WeightVector( FT_Face    face,
                          FT_UInt*   len,
                          FT_Fixed*  weightvector );

#define FT_VAR_AXIS_FLAG_HIDDEN  1

  FT_EXPORT( FT_Error )
  FT_Get_Var_Axis_Flags( FT_MM_Var*  master,
                         FT_UInt     axis_index,
                         FT_UInt*    flags );

  FT_EXPORT( FT_Error )
  FT_Set_Named_Instance( FT_Face  face,
                         FT_UInt  instance_index );

FT_END_HEADER

#endif

