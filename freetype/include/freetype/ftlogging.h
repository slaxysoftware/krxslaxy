
#ifndef FTLOGGING_H_
#define FTLOGGING_H_

#include <ft2build.h>
#include FT_CONFIG_CONFIG_H

FT_BEGIN_HEADER

  FT_EXPORT( void )
  FT_Trace_Set_Level( const char*  tracing_level );

  FT_EXPORT( void )
  FT_Trace_Set_Default_Level( void );

  typedef void
  (*FT_Custom_Log_Handler)( const char*  ft_component,
                            const char*  fmt,
                            va_list      args );

  FT_EXPORT( void )
  FT_Set_Log_Handler( FT_Custom_Log_Handler  handler );

  FT_EXPORT( void )
  FT_Set_Default_Log_Handler( void );

FT_END_HEADER

#endif

