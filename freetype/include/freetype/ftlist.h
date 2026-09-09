
#ifndef FTLIST_H_
#define FTLIST_H_

#include <freetype/freetype.h>

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

FT_BEGIN_HEADER

  FT_EXPORT( FT_ListNode )
  FT_List_Find( FT_List  list,
                void*    data );

  FT_EXPORT( void )
  FT_List_Add( FT_List      list,
               FT_ListNode  node );

  FT_EXPORT( void )
  FT_List_Insert( FT_List      list,
                  FT_ListNode  node );

  FT_EXPORT( void )
  FT_List_Remove( FT_List      list,
                  FT_ListNode  node );

  FT_EXPORT( void )
  FT_List_Up( FT_List      list,
              FT_ListNode  node );

  typedef FT_Error
  (*FT_List_Iterator)( FT_ListNode  node,
                       void*        user );

  FT_EXPORT( FT_Error )
  FT_List_Iterate( FT_List           list,
                   FT_List_Iterator  iterator,
                   void*             user );

  typedef void
  (*FT_List_Destructor)( FT_Memory  memory,
                         void*      data,
                         void*      user );

  FT_EXPORT( void )
  FT_List_Finalize( FT_List             list,
                    FT_List_Destructor  destroy,
                    FT_Memory           memory,
                    void*               user );

FT_END_HEADER

#endif

