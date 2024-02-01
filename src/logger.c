/**
@file
@purpose   LOGGER Lightweight logging component
@version   $Revision$
@license   $License$
@copyright $Copyright$
@brief     Logging component
*/
#define LOGGER_MAIN_
#include "printf.h"

#include <embetech/logger.h>

#include <inttypes.h>
#include <stdarg.h>


/// LOGGER runtime descriptor
typedef struct {
    bool                  isEnabled;      ///< Determines if the channel is active (1) or not (0)
    bool                  headerDisabled; ///< Determines whether log message header should not be printed
    LOGGER_OutputFunction out;            ///< Channel's output function
    void*                 outContext;     ///< Output function's context

#if 1 == LOGGER_THREAD_SAFETY_HOOKS
    LOGGER_LockFunction   lock;              ///< Channel's lock function
    LOGGER_UnlockFunction unlock;            ///< Channel's unlock function
    void*                 lockUnlockContext; ///< Lock/Unlock functions' context
#endif

#if 1 == LOGGER_TIMESTAMPS
    LOGGER_TimeSourceFunction getTime; ///< Channel's time source function
#endif
#if 1 == LOGGER_RUNTIME_VERBOSITY
    LOGGER_level level; ///< Runtime parameter to determine whether the message should be printed
#endif
#if 1 == LOGGER_CUSTOM_AFFIXES

    char const* prefix;       ///< Pointer to message prefix (MAY NOT be a c-string)
    size_t      prefixLength; ///< Prefix length

    char const* suffix;       ///< Pointer to message suffix (MAY NOT be a c-string)
    size_t      suffixLength; ///< Suffix length
#endif

#if 1 == LOGGER_FLUSH_HOOKS
    LOGGER_FlushFunction flushHook;
    bool                 flushOnNewLine;
#endif
} LOGGER_Descriptor;


static LOGGER_Descriptor LOGGER_RuntimeDescriptior = {
#if 1 == LOGGER_RUNTIME_VERBOSITY
    .level = LOGGER_LEVEL_TRACE
#else
    0 // Supress empty initialization list warning
#endif
};
static char const LOGGER_printableLevel[] = {'M', 'A', 'C', 'E', 'W', 'N', 'I', 'V', 'D', 'T'};


#if 1 == LOGGER_CUSTOM_AFFIXES
void LOGGER_SetPrefix(char const* data, size_t length) {
    LOGGER_RuntimeDescriptior.prefix       = data;
    LOGGER_RuntimeDescriptior.prefixLength = length;
}


void LOGGER_SetSuffix(char const* data, size_t length) {
    LOGGER_RuntimeDescriptior.suffix       = data;
    LOGGER_RuntimeDescriptior.suffixLength = length;
}


static void LOGGER_PrintPrefix(void) {
    if (LOGGER_RuntimeDescriptior.prefix != NULL) {
        for (size_t i = 0; i != LOGGER_RuntimeDescriptior.prefixLength; ++i) {
            LOGGER_RuntimeDescriptior.out(LOGGER_RuntimeDescriptior.prefix[i], LOGGER_RuntimeDescriptior.outContext);
        }
    }
}


static void LOGGER_PrintSuffix(void) {
    if (LOGGER_RuntimeDescriptior.suffix != NULL) {
        for (size_t i = 0; i != LOGGER_RuntimeDescriptior.suffixLength; ++i) {
            LOGGER_RuntimeDescriptior.out(LOGGER_RuntimeDescriptior.suffix[i], LOGGER_RuntimeDescriptior.outContext);
        }
    }
}
#else

void LOGGER_SetPrefix(char const* data, size_t length) {
    (void)data, (void)length; // Mark as unused
}


void LOGGER_SetSuffix(char const* data, size_t length) {
    (void)data, (void)length; // Mark as unused
}


static inline void LOGGER_PrintPrefix(void) {
}


static inline void LOGGER_PrintSuffix(void) {
}
#endif


#if 1 == LOGGER_THREAD_SAFETY_HOOKS
bool LOGGER_Lock(void) {
    // Return result if lock was successful or true, if lock function is not set
    return (LOGGER_RuntimeDescriptior.lock != NULL) ? LOGGER_RuntimeDescriptior.lock(LOGGER_RuntimeDescriptior.lockUnlockContext) : true;
}


void LOGGER_Unlock(void) {
    // Do not perform sanity checks - this is "internal function" that MUST be invoked in conjunction with LOGGER_Lock (which does sanity checks). By design it will not be invoked, if sanity check fails
    if (LOGGER_RuntimeDescriptior.unlock != NULL) {
        LOGGER_RuntimeDescriptior.unlock(LOGGER_RuntimeDescriptior.lockUnlockContext);
    }
}


bool LOGGER_SetLockingMechanism(LOGGER_LockFunction lock, LOGGER_UnlockFunction unlock, void* fContext) {
    if ((unlock != NULL) != (lock != NULL))
        return false; // Both function have to be either set or cleared
    // Attempt to lock LOGGER. In rare examples, when trying to change locking mechanism during program operation it will allow to finish printing with old locking mechanism and then change lock
    if (LOGGER_Lock()) {
        LOGGER_UnlockFunction oldUnlock             = LOGGER_RuntimeDescriptior.unlock;
        void*                 oldUnlockContext      = LOGGER_RuntimeDescriptior.lockUnlockContext;
        LOGGER_RuntimeDescriptior.lock              = lock;
        LOGGER_RuntimeDescriptior.unlock            = unlock;
        LOGGER_RuntimeDescriptior.lockUnlockContext = fContext;
        if (oldUnlock)
            oldUnlock(oldUnlockContext); // log was locked using old lock, so unlock with old lock
        return true;
    }
    return false;
}
#else


bool LOGGER_SetLockingMechanism(LOGGER_LockFunction lock, LOGGER_UnlockFunction unlock, void* fContext) {
    (void)lock, (void)unlock, (void)fContext; // Mark as unused
    return true;
}
#endif


#if 1 == LOGGER_FLUSH_HOOKS
void LOGGER_SetFlushHook(LOGGER_FlushFunction hook, bool flushOnNewLine) {
    LOGGER_RuntimeDescriptior.flushHook      = hook;
    LOGGER_RuntimeDescriptior.flushOnNewLine = flushOnNewLine;
}


void LOGGER_Flush(void) {
    if (LOGGER_RuntimeDescriptior.flushHook != NULL) {
        LOGGER_RuntimeDescriptior.flushHook();
    }
}


void LOGGER_PrintNL(void) {
    // Do not perform sanity checks or lock - this is "internal function" that MUST be invoked in conjunction with LOGGER_Lock (which does sanity checks). By design it will not be invoked, if sanity
    // check fails
    LOGGER_RuntimeDescriptior.out('\n', LOGGER_RuntimeDescriptior.outContext);
    if (LOGGER_RuntimeDescriptior.flushOnNewLine) {
        LOGGER_Flush();
    }
}
#else


void LOGGER_SetFlushHook(LOGGER_FlushFunction hook, bool flushOnNewLine) {
    (void)hook, (void)flushOnNewLine; // Mark as unused
}


void LOGGER_PrintNL(void) {
    // Do not perform sanity checks or lock - this is "internal function" that MUST be invoked in conjunction with LOGGER_Lock (which does sanity checks). By design it will not be invoked, if sanity
    // check fails
    LOGGER_RuntimeDescriptior.out('\n', LOGGER_RuntimeDescriptior.outContext);
}
#endif


#if 1 == LOGGER_RUNTIME_VERBOSITY
LOGGER_level LOGGER_DoGetRuntimeLevel(void) {
    return LOGGER_RuntimeDescriptior.level;
}


void LOGGER_SetRuntimeLevel(LOGGER_level level) {
    if (level < LOGGER_LEVEL_DISABLED) {
        level = LOGGER_LEVEL_DISABLED;
    }
    if (level > LOGGER_LEVEL_TRACE) {
        level = LOGGER_LEVEL_TRACE;
    }

    LOGGER_RuntimeDescriptior.level = level;
}
#else


void LOGGER_SetRuntimeLevel(LOGGER_level level) {
    (void)level; // Mark as unused
}
#endif


void LOGGER_PrintLine(LOGGER_HeaderDescriptor descr, char const* format, ...) {
    if (LOGGER_IsEnabled() && LOGGER_Lock()) {
        LOGGER_PrintPrefix();
        LOGGER_PrintHeader(descr);
        va_list args; // the scope of this variable is larger than necessary due to MSVC++2008 limitations
        va_start(args, format);
        vfctprintf(LOGGER_RuntimeDescriptior.out, LOGGER_RuntimeDescriptior.outContext, format, args);
        va_end(args);
        LOGGER_PrintNL();
        LOGGER_PrintSuffix();
        LOGGER_Unlock();
    }
}


void LOGGER_Print(char const* format, ...) {
    va_list args;
    // Do not perform sanity checks or lock - this is "internal function" that MUST be invoked in conjunction with LOGGER_Lock (which does sanity checks). By design it will not be invoked, if sanity
    // check fails Write the very log message
    va_start(args, format);
    vfctprintf(LOGGER_RuntimeDescriptior.out, LOGGER_RuntimeDescriptior.outContext, format, args);
    va_end(args);
}


bool LOGGER_StartSection(void) {
    if (LOGGER_IsEnabled() && LOGGER_Lock()) {
        LOGGER_PrintPrefix();
        return true;
    }
    return false;
}


void LOGGER_EndSection(void) {
    LOGGER_PrintSuffix();
    LOGGER_Unlock();
}


void LOGGER_PrintHeader(LOGGER_HeaderDescriptor descr) {
    // Do not perform sanity checks - this is "internal function" that MUST be invoked in conjunction with LOGGER_Lock (which does sanity checks). By design it will not be invoked, if sanity check fails
    if (false == LOGGER_RuntimeDescriptior.headerDisabled) {
#if 1 == LOGGER_TIMESTAMPS
        uint32_t now = 0;
        if (LOGGER_RuntimeDescriptior.getTime != NULL) {
            now = LOGGER_RuntimeDescriptior.getTime();
        }

#    if 1 == LOGGER_HUMAN_READABLE_TIMESTAMP
        uint32_t ms = now % UINT32_C(1000);
        now /= UINT32_C(1000);
        uint32_t seconds = now % UINT32_C(60);
        now /= UINT32_C(60);
        uint32_t minutes = now % UINT32_C(60);
        uint32_t hours   = now / UINT32_C(60);
#        define LOGGER_TIME_FORMAT_STR "%.02u:%.02u:%.02u.%.03u "
#        define LOGGER_TIME_ARGS       , hours, minutes, seconds, ms
#    else
#        define LOGGER_TIME_FORMAT_STR "%" PRIu32 " "
#        define LOGGER_TIME_ARGS       , now
#    endif
#else
#    define LOGGER_TIME_FORMAT_STR ""
#    define LOGGER_TIME_ARGS
#endif

#if 1 == LOGGER_HEADER_WITH_LOCATION
#    define LOGGER_LOCATION_FORMAT_STR " [%s:%d]"
#    define LOGGER_LOCATION_ARGS       , descr.file, descr.line
#else
#    define LOGGER_LOCATION_FORMAT_STR ""
#    define LOGGER_LOCATION_ARGS
#endif

        fctprintf(LOGGER_RuntimeDescriptior.out,
                  LOGGER_RuntimeDescriptior.outContext,
                  LOGGER_TIME_FORMAT_STR "%s (%c)" LOGGER_LOCATION_FORMAT_STR ": " LOGGER_TIME_ARGS,
                  descr.channel,
                  LOGGER_printableLevel[descr.level] LOGGER_LOCATION_ARGS);
    }
}


void LOGGER_SetOutput(LOGGER_OutputFunction f, void* fContext) {
    if (LOGGER_Lock()) {
        LOGGER_RuntimeDescriptior.out        = f;
        LOGGER_RuntimeDescriptior.outContext = fContext;
        LOGGER_RuntimeDescriptior.isEnabled  = (f != NULL);
        LOGGER_Unlock();
    }
}


void LOGGER_SetTimeSource(LOGGER_TimeSourceFunction f) {
#if 1 == LOGGER_TIMESTAMPS
    LOGGER_RuntimeDescriptior.getTime = f;
#else
    (void)f; // Mark as unused
#endif
}


bool LOGGER_Enable(void) {
    if (LOGGER_RuntimeDescriptior.out != NULL) {
        LOGGER_RuntimeDescriptior.isEnabled = true;
        return true;
    }
    return false;
}


void LOGGER_Disable(void) {
    LOGGER_RuntimeDescriptior.isEnabled = false;
}


bool LOGGER_IsEnabled(void) {
    return LOGGER_RuntimeDescriptior.isEnabled;
}


void LOGGER_DisableHeader(void) {
    LOGGER_RuntimeDescriptior.headerDisabled = true;
}


void LOGGER_EnableHeader(void) {
    LOGGER_RuntimeDescriptior.headerDisabled = false;
}
