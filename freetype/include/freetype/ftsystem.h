
#ifndef FTSYSTEM_H_
#define FTSYSTEM_H_

FT_BEGIN_HEADER

  typedef struct FT_MemoryRec_*  FT_Memory;

  typedef void*
  (*FT_Alloc_Func)( FT_Memory  memory,
                    long       size );

  typedef void
  (*FT_Free_Func)( FT_Memory  memory,
                   void*      block );

  typedef void*
  (*FT_Realloc_Func)( FT_Memory  memory,
                      long       cur_size,
                      long       new_size,
                      void*      block );

  struct  FT_MemoryRec_
  {
    void*            user;
    FT_Alloc_Func    alloc;
    FT_Free_Func     free;
    FT_Realloc_Func  realloc;
  };

  typedef struct FT_StreamRec_*  FT_Stream;

  typedef union  FT_StreamDesc_
  {
    long   value;
    void*  pointer;

  } FT_StreamDesc;

  typedef unsigned long
  (*FT_Stream_IoFunc)( FT_Stream       stream,
                       unsigned long   offset,
                       unsigned char*  buffer,
                       unsigned long   count );

  typedef void
  (*FT_Stream_CloseFunc)( FT_Stream  stream );

  typedef struct  FT_StreamRec_
  {
    unsigned char*       base;
    unsigned long        size;
    unsigned long        pos;

    FT_StreamDesc        descriptor;
    FT_StreamDesc        pathname;
    FT_Stream_IoFunc     read;
    FT_Stream_CloseFunc  close;

    FT_Memory            memory;
    unsigned char*       cursor;
    unsigned char*       limit;

  } FT_StreamRec;

FT_END_HEADER

#endif

