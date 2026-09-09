
#ifndef FTCACHE_H_
#define FTCACHE_H_

#include <freetype/ftglyph.h>

FT_BEGIN_HEADER

  typedef FT_Pointer  FTC_FaceID;

  typedef FT_Error
  (*FTC_Face_Requester)( FTC_FaceID  face_id,
                         FT_Library  library,
                         FT_Pointer  req_data,
                         FT_Face*    aface );

  typedef struct FTC_ManagerRec_*  FTC_Manager;

  typedef struct FTC_NodeRec_*  FTC_Node;

  FT_EXPORT( FT_Error )
  FTC_Manager_New( FT_Library          library,
                   FT_UInt             max_faces,
                   FT_UInt             max_sizes,
                   FT_ULong            max_bytes,
                   FTC_Face_Requester  requester,
                   FT_Pointer          req_data,
                   FTC_Manager        *amanager );

  FT_EXPORT( void )
  FTC_Manager_Reset( FTC_Manager  manager );

  FT_EXPORT( void )
  FTC_Manager_Done( FTC_Manager  manager );

  FT_EXPORT( FT_Error )
  FTC_Manager_LookupFace( FTC_Manager  manager,
                          FTC_FaceID   face_id,
                          FT_Face     *aface );

  typedef struct  FTC_ScalerRec_
  {
    FTC_FaceID  face_id;
    FT_UInt     width;
    FT_UInt     height;
    FT_Int      pixel;
    FT_UInt     x_res;
    FT_UInt     y_res;

  } FTC_ScalerRec;

  typedef struct FTC_ScalerRec_*  FTC_Scaler;

  FT_EXPORT( FT_Error )
  FTC_Manager_LookupSize( FTC_Manager  manager,
                          FTC_Scaler   scaler,
                          FT_Size     *asize );

  FT_EXPORT( void )
  FTC_Node_Unref( FTC_Node     node,
                  FTC_Manager  manager );

  FT_EXPORT( void )
  FTC_Manager_RemoveFaceID( FTC_Manager  manager,
                            FTC_FaceID   face_id );

  typedef struct FTC_CMapCacheRec_*  FTC_CMapCache;

  FT_EXPORT( FT_Error )
  FTC_CMapCache_New( FTC_Manager     manager,
                     FTC_CMapCache  *acache );

  FT_EXPORT( FT_UInt )
  FTC_CMapCache_Lookup( FTC_CMapCache  cache,
                        FTC_FaceID     face_id,
                        FT_Int         cmap_index,
                        FT_UInt32      char_code );

  typedef struct  FTC_ImageTypeRec_
  {
    FTC_FaceID  face_id;
    FT_UInt     width;
    FT_UInt     height;
    FT_Int32    flags;

  } FTC_ImageTypeRec;

  typedef struct FTC_ImageTypeRec_*  FTC_ImageType;

#define FTC_IMAGE_TYPE_COMPARE( d1, d2 )      \
          ( (d1)->face_id == (d2)->face_id && \
            (d1)->width   == (d2)->width   && \
            (d1)->flags   == (d2)->flags   )

  typedef struct FTC_ImageCacheRec_*  FTC_ImageCache;

  FT_EXPORT( FT_Error )
  FTC_ImageCache_New( FTC_Manager      manager,
                      FTC_ImageCache  *acache );

  FT_EXPORT( FT_Error )
  FTC_ImageCache_Lookup( FTC_ImageCache  cache,
                         FTC_ImageType   type,
                         FT_UInt         gindex,
                         FT_Glyph       *aglyph,
                         FTC_Node       *anode );

  FT_EXPORT( FT_Error )
  FTC_ImageCache_LookupScaler( FTC_ImageCache  cache,
                               FTC_Scaler      scaler,
                               FT_ULong        load_flags,
                               FT_UInt         gindex,
                               FT_Glyph       *aglyph,
                               FTC_Node       *anode );

  typedef struct FTC_SBitRec_*  FTC_SBit;

  typedef struct  FTC_SBitRec_
  {
    FT_Byte   width;
    FT_Byte   height;
    FT_Char   left;
    FT_Char   top;

    FT_Byte   format;
    FT_Byte   max_grays;
    FT_Short  pitch;
    FT_Char   xadvance;
    FT_Char   yadvance;

    FT_Byte*  buffer;

  } FTC_SBitRec;

  typedef struct FTC_SBitCacheRec_*  FTC_SBitCache;

  FT_EXPORT( FT_Error )
  FTC_SBitCache_New( FTC_Manager     manager,
                     FTC_SBitCache  *acache );

  FT_EXPORT( FT_Error )
  FTC_SBitCache_Lookup( FTC_SBitCache    cache,
                        FTC_ImageType    type,
                        FT_UInt          gindex,
                        FTC_SBit        *sbit,
                        FTC_Node        *anode );

  FT_EXPORT( FT_Error )
  FTC_SBitCache_LookupScaler( FTC_SBitCache  cache,
                              FTC_Scaler     scaler,
                              FT_ULong       load_flags,
                              FT_UInt        gindex,
                              FTC_SBit      *sbit,
                              FTC_Node      *anode );

FT_END_HEADER

#endif

