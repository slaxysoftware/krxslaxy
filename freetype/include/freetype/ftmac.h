
#ifndef FTMAC_H_
#define FTMAC_H_

FT_BEGIN_HEADER

#ifndef FT_DEPRECATED_ATTRIBUTE
#if defined( __GNUC__ )                                     && \
    ( ( __GNUC__ >= 4 )                                  ||    \
      ( ( __GNUC__ == 3 ) && ( __GNUC_MINOR__ >= 1 ) ) )
#define FT_DEPRECATED_ATTRIBUTE  __attribute__(( deprecated ))
#else
#define FT_DEPRECATED_ATTRIBUTE
#endif
#endif

  FT_EXPORT( FT_Error )
  FT_New_Face_From_FOND( FT_Library  library,
                         Handle      fond,
                         FT_Long     face_index,
                         FT_Face    *aface )
                       FT_DEPRECATED_ATTRIBUTE;

  FT_EXPORT( FT_Error )
  FT_GetFile_From_Mac_Name( const char*  fontName,
                            FSSpec*      pathSpec,
                            FT_Long*     face_index )
                          FT_DEPRECATED_ATTRIBUTE;

  FT_EXPORT( FT_Error )
  FT_GetFile_From_Mac_ATS_Name( const char*  fontName,
                                FSSpec*      pathSpec,
                                FT_Long*     face_index )
                              FT_DEPRECATED_ATTRIBUTE;

  FT_EXPORT( FT_Error )
  FT_GetFilePath_From_Mac_ATS_Name( const char*  fontName,
                                    UInt8*       path,
                                    UInt32       maxPathSize,
                                    FT_Long*     face_index )
                                  FT_DEPRECATED_ATTRIBUTE;

  FT_EXPORT( FT_Error )
  FT_New_Face_From_FSSpec( FT_Library     library,
                           const FSSpec  *spec,
                           FT_Long        face_index,
                           FT_Face       *aface )
                         FT_DEPRECATED_ATTRIBUTE;

  FT_EXPORT( FT_Error )
  FT_New_Face_From_FSRef( FT_Library    library,
                          const FSRef  *ref,
                          FT_Long       face_index,
                          FT_Face      *aface )
                        FT_DEPRECATED_ATTRIBUTE;

FT_END_HEADER

#endif

