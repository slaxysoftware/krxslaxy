
#ifndef FTSTDLIB_H_
#define FTSTDLIB_H_

#include <stddef.h>

#define ft_ptrdiff_t  ptrdiff_t

#include <limits.h>

#define FT_CHAR_BIT    CHAR_BIT
#define FT_USHORT_MAX  USHRT_MAX
#define FT_INT_MAX     INT_MAX
#define FT_INT_MIN     INT_MIN
#define FT_UINT_MAX    UINT_MAX
#define FT_LONG_MIN    LONG_MIN
#define FT_LONG_MAX    LONG_MAX
#define FT_ULONG_MAX   ULONG_MAX
#ifdef LLONG_MAX
#define FT_LLONG_MAX   LLONG_MAX
#endif
#ifdef LLONG_MIN
#define FT_LLONG_MIN   LLONG_MIN
#endif
#ifdef ULLONG_MAX
#define FT_ULLONG_MAX  ULLONG_MAX
#endif

#include <string.h>

#define ft_memchr   memchr
#define ft_memcmp   memcmp
#define ft_memcpy   memcpy
#define ft_memmove  memmove
#define ft_memset   memset
#define ft_strcat   strcat
#define ft_strcmp   strcmp
#define ft_strcpy   strcpy
#define ft_strlen   strlen
#define ft_strncmp  strncmp
#define ft_strncpy  strncpy
#define ft_strrchr  strrchr
#define ft_strstr   strstr

#include <stdio.h>

#define FT_FILE     FILE
#define ft_fclose   fclose
#define ft_fopen    fopen
#define ft_fread    fread
#define ft_fseek    fseek
#define ft_ftell    ftell
#define ft_sprintf  sprintf

#include <stdlib.h>

#define ft_qsort  qsort

#define ft_scalloc   calloc
#define ft_sfree     free
#define ft_smalloc   malloc
#define ft_srealloc  realloc

#define ft_strtol  strtol
#define ft_getenv  getenv

#include <setjmp.h>

#define ft_jmp_buf     jmp_buf

#define ft_longjmp     longjmp
#define ft_setjmp( b ) setjmp( *(ft_jmp_buf*) &(b) )

#include <stdarg.h>

#endif

