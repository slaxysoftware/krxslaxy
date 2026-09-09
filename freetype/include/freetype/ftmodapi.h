
#ifndef FTMODAPI_H_
#define FTMODAPI_H_

#include <freetype/freetype.h>

#ifdef FREETYPE_H
#error "freetype.h of FreeType 1 has been loaded!"
#error "Please fix the directory search order for header files"
#error "so that freetype.h of FreeType 2 is found first."
#endif

FT_BEGIN_HEADER

#define FT_MODULE_FONT_DRIVER         1
#define FT_MODULE_RENDERER            2
#define FT_MODULE_HINTER              4
#define FT_MODULE_STYLER              8

#define FT_MODULE_DRIVER_SCALABLE      0x100

#define FT_MODULE_DRIVER_NO_OUTLINES   0x200

#define FT_MODULE_DRIVER_HAS_HINTER    0x400

#define FT_MODULE_DRIVER_HINTS_LIGHTLY 0x800

#define ft_module_font_driver         FT_MODULE_FONT_DRIVER
#define ft_module_renderer            FT_MODULE_RENDERER
#define ft_module_hinter              FT_MODULE_HINTER
#define ft_module_styler              FT_MODULE_STYLER

#define ft_module_driver_scalable       FT_MODULE_DRIVER_SCALABLE
#define ft_module_driver_no_outlines    FT_MODULE_DRIVER_NO_OUTLINES
#define ft_module_driver_has_hinter     FT_MODULE_DRIVER_HAS_HINTER
#define ft_module_driver_hints_lightly  FT_MODULE_DRIVER_HINTS_LIGHTLY

  typedef FT_Pointer  FT_Module_Interface;

  typedef FT_Error
  (*FT_Module_Constructor)( FT_Module  module );

  typedef void
  (*FT_Module_Destructor)( FT_Module  module );

  typedef FT_Module_Interface
  (*FT_Module_Requester)( FT_Module    module,
                          const char*  name );

  typedef struct  FT_Module_Class_
  {
    FT_ULong               module_flags;
    FT_Long                module_size;
    const FT_String*       module_name;
    FT_Fixed               module_version;
    FT_Fixed               module_requires;

    const void*            module_interface;

    FT_Module_Constructor  module_init;
    FT_Module_Destructor   module_done;
    FT_Module_Requester    get_interface;

  } FT_Module_Class;

  FT_EXPORT( FT_Error )
  FT_Add_Module( FT_Library              library,
                 const FT_Module_Class*  clazz );

  FT_EXPORT( FT_Module )
  FT_Get_Module( FT_Library   library,
                 const char*  module_name );

  FT_EXPORT( FT_Error )
  FT_Remove_Module( FT_Library  library,
                    FT_Module   module );

#define FT_FACE_DRIVER_NAME( face )                                     \
          ( ( *FT_REINTERPRET_CAST( FT_Module_Class**,                  \
                                    ( face )->driver ) )->module_name )

  FT_EXPORT( FT_Error )
  FT_Property_Set( FT_Library        library,
                   const FT_String*  module_name,
                   const FT_String*  property_name,
                   const void*       value );

  FT_EXPORT( FT_Error )
  FT_Property_Get( FT_Library        library,
                   const FT_String*  module_name,
                   const FT_String*  property_name,
                   void*             value );

  FT_EXPORT( void )
  FT_Set_Default_Properties( FT_Library  library );

  FT_EXPORT( FT_Error )
  FT_Reference_Library( FT_Library  library );

  FT_EXPORT( FT_Error )
  FT_New_Library( FT_Memory    memory,
                  FT_Library  *alibrary );

  FT_EXPORT( FT_Error )
  FT_Done_Library( FT_Library  library );

  typedef FT_Error
  (*FT_DebugHook_Func)( void*  arg );

#define FT_DEBUG_HOOK_TRUETYPE  0

  FT_EXPORT( void )
  FT_Set_Debug_Hook( FT_Library         library,
                     FT_UInt            hook_index,
                     FT_DebugHook_Func  debug_hook );

  FT_EXPORT( void )
  FT_Add_Default_Modules( FT_Library  library );

  typedef enum  FT_TrueTypeEngineType_
  {
    FT_TRUETYPE_ENGINE_TYPE_NONE = 0,
    FT_TRUETYPE_ENGINE_TYPE_UNPATENTED,
    FT_TRUETYPE_ENGINE_TYPE_PATENTED

  } FT_TrueTypeEngineType;

  FT_EXPORT( FT_TrueTypeEngineType )
  FT_Get_TrueType_Engine_Type( FT_Library  library );

FT_END_HEADER

#endif

