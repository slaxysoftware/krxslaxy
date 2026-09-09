
#ifndef FREETYPE_CONFIG_MAC_SUPPORT_H_
#define FREETYPE_CONFIG_MAC_SUPPORT_H_

#if defined( __APPLE__ ) || ( defined( __MWERKS__ ) && defined( macintosh ) )

#include <errno.h>
#ifdef ECANCELED
#include "AvailabilityMacros.h"
#endif
#if defined( __LP64__ ) && \
    ( MAC_OS_X_VERSION_MIN_REQUIRED <= MAC_OS_X_VERSION_10_4 )
#undef FT_MACINTOSH
#endif

#elif defined( __SC__ ) || defined( __MRC__ )

#include "ConditionalMacros.h"
#if TARGET_OS_MAC
#define FT_MACINTOSH 1
#endif

#endif

#endif

