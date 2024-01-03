
/**
@file
@purpose   LOGGER Lightweight logging component
@version   $Revision$
@license   $License$
@copyright $Copyright$
@brief     Logging component
*/

#ifndef LOGGER_H_
#define LOGGER_H_

#if (!defined LOGGER_MAIN_) && __has_include(<logger_config.h>)
#    include <logger_config.h>
#endif

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif


// Definition of valid log levels (according to syslog)
#define LOGGER_LEVEL_DISABLED  -1 ///< Logger level that removes given line from compilation unit entirely
#define LOGGER_LEVEL_EMERGENCY 0  ///< Syslog compliant level with description: A panic condition
#define LOGGER_LEVEL_ALERT     1  ///< Syslog compliant level with description: A condition that should be corrected immediately, such as a corrupted system database
#define LOGGER_LEVEL_CRITICAL  2  ///< Syslog compliant level with description: Hard device errors
#define LOGGER_LEVEL_ERROR     3  ///< Syslog compliant error level
#define LOGGER_LEVEL_WARNING   4  ///< Syslog compliant warning level
#define LOGGER_LEVEL_NOTICE    5  ///< Syslog compliant level with description: Conditions that are not error conditions, but that may require special handling
#define LOGGER_LEVEL_INFO      6  ///< Syslog compliant info level
#define LOGGER_LEVEL_VERBOSE   7  ///< Additional verbosity level for informational messages that displaying is optional and yield significant performance penalty
#define LOGGER_LEVEL_DEBUG     8  ///< Syslog compliant level with description: Messages that contain information normally of use only when debugging a program
#define LOGGER_LEVEL_TRACE     9  ///< Additional verbosity level intended for detailed debugging


// The usage of the macros below is similar to cstdio's printf function:
// The first argument is the format string, the latter arguments should be variables that will be substituted in the format string
#define LOGGER_DISABLED(...) \
    LOGGER_PRINT_LINE(LOGGER_LEVEL_DISABLED, __VA_ARGS__) ///< Disables printing message. Useful during development, to leave some test information in code without memory/performnance penalties
#define LOGGER_EMERGENCY(...) LOGGER_PRINT_LINE(LOGGER_LEVEL_EMERGENCY, __VA_ARGS__) ///< Prints Emergency message
#define LOGGER_ALERT(...)     LOGGER_PRINT_LINE(LOGGER_LEVEL_ALERT, __VA_ARGS__)     ///< Prints Alert message
#define LOGGER_CRITICAL(...)  LOGGER_PRINT_LINE(LOGGER_LEVEL_CRITICAL, __VA_ARGS__)  ///< Prints Critical message
#define LOGGER_ERROR(...)     LOGGER_PRINT_LINE(LOGGER_LEVEL_ERROR, __VA_ARGS__)     ///< Prints Error message
#define LOGGER_WARNING(...)   LOGGER_PRINT_LINE(LOGGER_LEVEL_WARNING, __VA_ARGS__)   ///< Prints Warning message
#define LOGGER_NOTICE(...)    LOGGER_PRINT_LINE(LOGGER_LEVEL_NOTICE, __VA_ARGS__)    ///< Prints Notice message
#define LOGGER_INFO(...)      LOGGER_PRINT_LINE(LOGGER_LEVEL_INFO, __VA_ARGS__)      ///< Prints Info message
#define LOGGER_VERBOSE(...)   LOGGER_PRINT_LINE(LOGGER_LEVEL_VERBOSE, __VA_ARGS__)   ///< Prints Verbose message
#define LOGGER_DEBUG(...)     LOGGER_PRINT_LINE(LOGGER_LEVEL_DEBUG, __VA_ARGS__)     ///< Prints Debug message
#define LOGGER_TRACE(...)     LOGGER_PRINT_LINE(LOGGER_LEVEL_TRACE, __VA_ARGS__)     ///< Prints Trace message


/**
 * @brief Returns boolean value whether the given verbosity level may be printed with current runtime and compile-time verbosity setting
 * @param[in] level level to be checked
 */
#define LOGGER_VERBOSITY_ENABLED(level) LOGGER_IS_VERBOSITY_ENABLED(level)

/**
 * @brief Starts Logging section
 * If user needs to print composite log message (i.e. has overloaded print function that uses LOGGER), this macro might be used to construct inline message:
 * @param[in] level predefined log level macro
 * @note Usage example: LOGGER_START(level); {command sequence that uses LOGGER_CONTINUE macro}; LOGGER_END(); //The example above will print single LOGGER header.
 * @note Declaring variables inside LOGGER_START/LOGGER_END scope reduces their scope and lifetime. However, it might be beneficial if the variables are user only for logging (they will be optimized
 * out if log is disabled)
 * @warning Using macros other than LOGGER_CONTINUE LOGGER_START/LOGGER_END scope is prohibited and will cause unexpected output
 */
#define LOGGER_START(level) LOGGER_SECTION_START(level)


/**
 * @brief Prints formatted output in LOGGER_START/LOGGER_END scope.
 * @warning Use of macro outside LOGGER_START/LOGGER_END scope is strictly prohibited as LOGGER_START performs sanity checks
 * @param[in] ... printf-like formatted parameters
 */
#define LOGGER_CONTINUE(...) LOGGER_SECTION_CONTINUE(__VA_ARGS__)


/**
 * @brief Ends LOGGER_START scope
 * No additional characters will be printed
 * @warning Use of macro without invoking LOGGER_START before will cause undefined behaviour (compiler error at best)
 */
#define LOGGER_END() LOGGER_SECTION_END()


/**
 * @brief Ends LOGGER_START scope
 * Additional new line character ('\n') will be printed to output
 * @warning Use of macro without invoking LOGGER_START before will cause undefined behaviour (compiler error at best)
 */
#define LOGGER_ENDL() LOGGER_SECTION_ENDL()


typedef void (*LOGGER_OutputFunction)(char c, void* context); ///< Log output function type


/**
 * @brief Binds output function to log
 * @param[in] f output function
 * @param[in] fContext output function's context
 */
void LOGGER_SetOutput(LOGGER_OutputFunction f, void* fContext);


typedef uint32_t LOGGER_Time;                         ///< Default time type
typedef bool (*LOGGER_LockFunction)(void* context);   ///< Log lock function type
typedef void (*LOGGER_UnlockFunction)(void* context); ///< Log unlock function type


/**
 * @brief Binds lock/unlock callbacks
 * @param[in] lock locking function
 * @param[in] unlock unlocking function
 * @param[in] fContext output function's context
 * @note since LOGGER will not initialize/deinitialze the locking mechanism, it is user responsibility to manage its lifetime
 * @note lock and unlock function pointer has to be both either null or non-null
 * @note if CONFIG_LOGGER_THREAD_SAFETY_HOOKS is set to 0, the function can be called, but this setting has no effect
 * @retval true when locking mechains has been changed, false if provided arguments are invalid or lock-timeout ocurred
 */
bool LOGGER_SetLockingMechanism(LOGGER_LockFunction lock, LOGGER_UnlockFunction unlock, void* fContext);


typedef uint32_t (*LOGGER_TimeSourceFunction)(void); ///< Log time source function type


/**
 * @brief Binds time source to log
 * @param[in] f function that returns current system time when invoked
 */
void LOGGER_SetTimeSource(LOGGER_TimeSourceFunction f);


typedef void (*LOGGER_FlushFunction)(void); ///< Flush function prototype


/**
 * @brief Binds function to be invoked after each LOGGER message (LOGGER_INFO, LOGGER_NOTICE, etc.), or after each LOGGER Scope (LOGGER_START/LOGGER_END)
 * @param[in] hook flush hook
 * @param[in] flushOnNewLine sets runtime behaviour, whether the hook function should be called automatically on each new line character.
 * @note The function accepts nullptr as a way to unbind current hook
 * @note if CONFIG_LOGGER_HEADER_WITH_LOCATION is set to 0, the function can be called, but this setting has no effect
 */
void LOGGER_SetFlushHook(LOGGER_FlushFunction hook, bool flushOnNewLine);


/**
 * @brief Enables log output
 * @return true when the operation was successful; false otherwise
 */
bool LOGGER_Enable(void);


/**
 * @brief Disables log output
 */
void LOGGER_Disable(void);


/**
 * @brief Returns information whether the log output is enabled
 * @return true when log is enabled, false otherwise.
 */
bool LOGGER_IsEnabled(void);


/**
 * @brief  Disables printing header message (channel name, time, and verbosity level) to output
 */
void LOGGER_DisableHeader(void);


/**
 * @brief Enables printing header message (channel name, time, and verbosity level) to output
 */
void LOGGER_EnableHeader(void);


typedef int LOGGER_level; ///< Default level type


/**
 * @brief Returns current runtime verbosity level
 * @note if CONFIG_LOGGER_RUNTIME_VERBOSITY is set to 0, the function can be called, however the result of the function is not meaningful (runtime level setting is not available).
 */
static inline LOGGER_level LOGGER_GetRuntimeLevel(void);


/**
 * @brief Sets runtime verbosity level for all channels
 * @param[in] level new runtime verbosity
 * @note This function will not print log messages that were not included at compile time
 * @note if CONFIG_LOGGER_RUNTIME_VERBOSITY is set to 0, the function can be called, but this setting has no effect (runtime level setting is not available)
 */
void LOGGER_SetRuntimeLevel(LOGGER_level level);


/**
 * @brief Prefixes each logger message with user-provided data.
 *
 * The prefix is applied before header. Turning off header WILL NOT prevent printing out prefix
 * The data MAY NOT be a valid. null-terminated string, and LOGGER guarantees that exactly length characters will be printed out
 * @param[in] data memory location with at least length size that stores the prefix. LOGGER stores only the pointer and size information, so user MUST guarantee the lifetime of the object
 * @param[in] length length of prefix
 * @note if CONFIG_LOGGER_CUSTOM_AFFIXES is set to 0, the function can be called, but this setting has no effect
 */
void LOGGER_SetPrefix(char const* data, size_t length);


/**
 * @brief Adds suffix to each logger message with user-provided data.
 *
 * The data MAY NOT be a valid. null-terminated string, and LOGGER guarantees that exactly length characters will be printed out
 * @param[in] data memory location with at least length size that stores the suffix. LOGGER stores only the pointer and size information, so user MUST guarantee the lifetime of the object
 * @param[in] length length of suffix
 * @note if CONFIG_LOGGER_CUSTOM_AFFIXES is set to 0, the function can be called, but this setting has no effect
 */
void LOGGER_SetSuffix(char const* data, size_t length);


/****************************************************************************************************************************************************************************/
/************* The content below must NOT be altered (unless you are a developer or the almighty guinea pig). Functions below are not meant to be used by user *************/
/****************************************************************************************************************************************************************************/

/***********************************************/
/************* Predefined Defaults *************/
/***********************************************/

#ifndef LOGGER_ENABLED
#    define LOGGER_ENABLED 1 ///< [GLOBAL] Main Component On/Off switch. If set to 0, even if every other conditions are met, logger will write nothing
#endif


#ifndef LOGGER_CHANNEL
#    define LOGGER_CHANNEL DEFAULT ///< Defines log channel
#endif


#ifndef DEFAULT_LOG_CHANNEL_LEVEL
#    define DEFAULT_LOG_CHANNEL_LEVEL LOGGER_LEVEL_NOTICE ///< Sets DEFAULT channel verbosity to GLOBAL one
#endif


#ifndef LOGGER_FILE
#    define LOGGER_FILE __FILE__ ///< Provides string with source file basename (if possible and supported by build system). If not, falls back to compiler's default __FILE__ definition
#endif

#define LOGGER_INTERNAL_REQUIRE_TRAILING_SEMICOLON (void)"This Macro Requires trailing semicolon"


/**********************************************/
/************* Preprocessor magic *************/
/**********************************************/

/**
 * @def LOGGER_CHECK_FORMAT(formatIndex, toCheckIndex)
 * @brief Provides compiler support for format checking
 * @param[in] formatIndex index of argument, which is the format string (starting from 1)
 * @param[in] toCheckIndex index of first argument to check against format string (starting from 1)
 * @note current implementation supports GNU based compilers only. User may define own format checking macro, compliant with this format
 */
#ifndef LOGGER_CHECK_FORMAT
#    if defined(__clang__) // Check clang first, as it tries to pretend to be all other compilers at once... (not cool clang people :/)
#        define LOGGER_CHECK_FORMAT(formatIndex, toCheckIndex) __attribute__((format(printf, formatIndex, toCheckIndex)))
#    elif defined(__GNUC__) || defined(__GNUG__)
#        if defined(__MINGW64__) || defined(__MINGW32__) // Workaround to surpress warning about %zu and %zi while using MINGW
#            define LOGGER_CHECK_FORMAT(formatIndex, toCheckIndex) __attribute__((format(gnu_printf, formatIndex, toCheckIndex)))
#        else
#            define LOGGER_CHECK_FORMAT(formatIndex, toCheckIndex) __attribute__((format(printf, formatIndex, toCheckIndex)))
#        endif
#    else // Rest, including MSVC - you're basically screwed.
#        define LOGGER_CHECK_FORMAT(formatIndex, toCheckIndex)
#    endif
#endif


/**
 * @brief Stringifies macro (first level of indirection)
 * @param[in] x macro to be stringifies
 */
#define LOGGER_STR(x) LOGGER_STR_I(x)


/**
 * @brief Stringifies macro (second level of indirection)
 * @param[in] x macro to be stringifies
 */
#define LOGGER_STR_I(x) #x


/// Evaluates variable argument macro
#define LOGGER_EVAL(...) __VA_ARGS__


/// Definition of empty macro
#define LOGGER_EMPTY()


/**
 * @brief Defers macro
 * @param[in] x macro to be defered
 */
#define LOGGER_DEFER(x) LOGGER_EMPTY() x


/**
 * @brief Concatenates two macros (first level of indirection)
 * @param[in] x first macro to be concatenated
 * @param[in] y second macro to be concatenated
 * @return xy
 */
#define LOGGER_CAT(x, y) LOGGER_CAT_I(x, y)


/**
 * @brief Concatenates two macros (second level of indirection)
 * @param[in] x first macro to be concatenated
 * @param[in] y second macro to be concatenated
 * @return xy
 */
#define LOGGER_CAT_I(x, y) x##y


/**
 * @brief Returns second argument
 * @param[in] a1 first argument
 * @param[in] a2 second argument (the one that will be returned)
 * @param[in] ... argument list of unspecified length
 * @return a2
 */
#define LOGGER_SECOND(a1, a2, ...) a2


/// Defines special probe value
#define LOGGER_PROBE() ~, 1


/**
 * @brief Checks whether passed arguments matches with LOGGER_PROBE macro
 * @param[in] ... argument list of unspecified length
 * @return 1 if passed arguments matches with LOGGER_PROBE
 */
#define LOGGER_IS_PROBE(...) LOGGER_EVAL(LOGGER_SECOND(__VA_ARGS__, 0))


/// Provides special value for LOGGER_NOT macro
#define LOGGER_NOT_0 LOGGER_PROBE()


/**
 * @brief performs logical negation on argument
 * @param[in] x value to be negated
 * @return !x
 */
#define LOGGER_NOT(x) LOGGER_IS_PROBE(LOGGER_CAT(LOGGER_NOT_, x), 0)


/**
 * @brief converts macro to logic value
 * @param[in] x macro to be converted
 * @return 0 if x == 0, 1 if x != 0 or undefined
 */
#define LOGGER_BOOL(x) LOGGER_NOT(LOGGER_NOT(x))


/**
 * @brief Compile time if/else statement (first layer of indirection)
 * example usage: IF_ELSE(x)(if_true)(else)
 * @param[in] x condition expression
 */
#define LOGGER_IF_ELSE(x) LOGGER_IF_ELSE_I(LOGGER_BOOL(x))


/**
 * @brief Compile time if/else statement (second layer of indirection)
 * example usage: IF_ELSE(x)(if_true)(else)
 * @param[in] x condition expression
 */
#define LOGGER_IF_ELSE_I(x) LOGGER_CAT(LOGGER_IF_, x)


/// Expands given statement when LOGGER_IF_ELSE condition is true
#define LOGGER_IF_1(...) __VA_ARGS__ LOGGER_IF_1_ELSE


/// Does not expand given statement when LOGGER_IF_ELSE condition is false
#define LOGGER_IF_0(...) LOGGER_IF_0_ELSE


/// Does not expand given ELSE statement when LOGGER_IF_ELSE condition is true
#define LOGGER_IF_1_ELSE(...)


/// Expands given ELSE statement when LOGGER_IF_ELSE condition is false
#define LOGGER_IF_0_ELSE(...) __VA_ARGS__


/**
 * @brief Checks whether given macro has empty value
 * @param[in] x macro to be checked
 * @return  0 only when macro has empty value, 1 of is non-zero or undefined
 */
#define LOGGER_IF_NOT_EMPTY(x) LOGGER_BOOL(LOGGER_CAT(LOGGER_CAT(LOGGER_IS_NOT_EMPTY_, x), _))


/// Special value for LOGGER_IF_NOT_EMPTY macro
#define LOGGER_IS_NOT_EMPTY__ 0


/// Special value for LOGGER_IF_NOT_EMPTY macro
#define LOGGER_IS_NOT_EMPTY_0_ 1


/**
 * @brief Checks whether given macro has non-zero value
 * @param[in] x macro to be checked
 * @return 0 only when macro has zero value, 1 of is non-zero or undefined
 */
#define LOGGER_IF_NZ(x) LOGGER_BOOL(LOGGER_CAT(LOGGER_CAT(LOGGER_IS_NZ_, x), _))


/// Special value for LOGGER_IF_NZ macro
#define LOGGER_IS_NZ__ 0


/// Special value for LOGGER_IF_NZ macro
#define LOGGER_IS_NZ_0_ 0


#if defined LOGGER_ENABLED && LOGGER_NOT(LOGGER_IF_NOT_EMPTY(LOGGER_ENABLED))
#    error LOGGER_ENABLED has no explicit value (should be defined either 1 or 0)
#endif


// Check whether LOGGER_CHANNEL was defined with value. If user did not define LOGGER_CHANNEL, it has default value (DEFAULT)
#if defined LOGGER_CHANNEL && LOGGER_NOT(LOGGER_IF_NOT_EMPTY(LOGGER_CHANNEL))
#    error LOGGER_CHANNEL has no explicit value
#endif


#define LOGGER_CHANNEL_LEVEL \
    LOGGER_CAT(LOGGER_CHANNEL, _LOG_CHANNEL_LEVEL) ///< Normally. this macro shall store integer from ***_LOG_CHANNEL_LEVEL macro.
                                                   ///< If user messed up and defined ***_LOG_CHANNEL_LEVEL empty, it would be empty (we have check for this below)
                                                   ///< If user messed up and did not defined ***_LOG_CHANNEL_LEVEL, it would have ***_LOG_CHANNEL_LEVEL value (we also have check for this below)


// Check if ***_LOG_CHANNEL_LEVEL macro is defined properly:
#if LOGGER_CHANNEL_LEVEL + 0 >= 0                             // Workaround: Neither LOGGER_IF_NOT_EMPTY nor LOGGER_IF_NZ handle negative values
#    if LOGGER_NOT(LOGGER_IF_NOT_EMPTY(LOGGER_CHANNEL_LEVEL)) // First ensure that LOGGER_CHANNEL_LEVEL has value. It won't be true if the user messed up and defined ***_LOG_CHANNEL_LEVEL empty
#        error "<see below>_LOG_CHANNEL_LEVEL has no explicit value"

#        if 1 == CONFIG_LOGGER_VERBOSE_ERRORS // Add verbose message
#            pragma message "LOGGER_CHANNEL evaluates to " LOGGER_STR(LOGGER_CHANNEL)
#        endif
#    endif

#    if !LOGGER_CHANNEL_LEVEL && LOGGER_IF_NZ(LOGGER_CHANNEL_LEVEL) // If ***_LOG_CHANNEL_LEVEL was not defined, the LOGGER_CHANNEL_LEVEL will be 0, but direct comparison to 0 will yield false
                                                                    // (because LOGGER_CHANNEL_LEVEL has no explicit value)
#        error <see below>_LOG_CHANNEL_LEVEL not defined for this translation unit

#        if 1 == CONFIG_LOGGER_VERBOSE_ERRORS // Add verbose message
#            pragma message "LOGGER_CHANNEL evaluates to " LOGGER_STR(LOGGER_CHANNEL)
#        endif
#    endif
#endif


// LOGGER_CHANNEL_LEVEL has **some** value. Perform bound checking [LOGGER_LEVEL_DISABLED, LOGGER_LEVEL_TRACE]
#if ((LOGGER_CHANNEL_LEVEL < LOGGER_LEVEL_DISABLED) || (LOGGER_CHANNEL_LEVEL > LOGGER_LEVEL_TRACE))
#    error <see below>_LOG_CHANNEL_LEVEL is out of bounds

#    if 1 == CONFIG_LOGGER_VERBOSE_ERRORS // Add verbose message
#        pragma message "LOGGER_CHANNEL evaluates to " LOGGER_STR(LOGGER_CHANNEL)
#    endif
#endif


#define LOGGER_HEADER LOGGER_STR(LOGGER_CHANNEL) ///< Contains stringified channel name


/*********************************************************************/
/************* RUNTIME_VERBOSITY - Configurable features *************/
/*********************************************************************/
/**
 * @def LOGGER_IS_VERBOSITY_ENABLED(level)
 * @brief Returns boolean value whether the given verbosity level may be printed with current runtime and compile-time verbosity setting
 * @param[in] level level to check
 */
#if 1 == CONFIG_LOGGER_RUNTIME_VERBOSITY
#    define LOGGER_IS_VERBOSITY_ENABLED(level) ((level > LOGGER_LEVEL_DISABLED) && (level <= LOGGER_CHANNEL_LEVEL) && (level <= LOGGER_GetRuntimeLevel()))


/**
 * @brief returns current logger runtime verbosity level
 */
LOGGER_level LOGGER_DoGetRuntimeLevel(void);

#else
#    define LOGGER_IS_VERBOSITY_ENABLED(level) ((level > LOGGER_LEVEL_DISABLED) && (level <= LOGGER_CHANNEL_LEVEL))

/**
 * @brief returns current channel compile-time level, as the function shall not be used in this mode
 *
 * @return LOGGER_level
 */
static inline LOGGER_level LOGGER_DoGetRuntimeLevel(void) {
    return LOGGER_CHANNEL_LEVEL;
}
#endif


static inline LOGGER_level LOGGER_GetRuntimeLevel(void) {
    return LOGGER_DoGetRuntimeLevel();
}

/************************************************************************/
/************* HEADER_WITH_LOCATION - Configurable features *************/
/************************************************************************/
#if 1 == CONFIG_LOGGER_HEADER_WITH_LOCATION

typedef struct {
    LOGGER_level level;
    char const*  channel;
    char const*  file;
    int          line;
} LOGGER_HeaderDescriptor;
#    ifdef __cplusplus
#        define LOGGER_HEADER_DESCR(lvl) (LOGGER_HeaderDescriptor{.level = lvl, .channel = LOGGER_HEADER "", .file = LOGGER_FILE "", .line = __LINE__})
#    else
#        define LOGGER_HEADER_DESCR(lvl) (LOGGER_HeaderDescriptor){.level = lvl, .channel = LOGGER_HEADER "", .file = LOGGER_FILE "", .line = __LINE__})
#    endif
#else

typedef struct {
    LOGGER_level level;
    char const*  channel;
} LOGGER_HeaderDescriptor;

#    ifdef __cplusplus
#        define LOGGER_HEADER_DESCR(lvl) (LOGGER_HeaderDescriptor{.level = lvl, .channel = LOGGER_HEADER ""})
#    else
#        define LOGGER_HEADER_DESCR(lvl) ((LOGGER_HeaderDescriptor){.level = lvl, .channel = LOGGER_HEADER ""})
#    endif


#endif




/***********************************************************************/
/************* THREAD_SAFETY_HOOKS - Configurable features *************/
/***********************************************************************/
#if 1 == CONFIG_LOGGER_THREAD_SAFETY_HOOKS
/**
 * @brief Attempts to achieve exclusive access to log output
 * @return true if access was achieved, false otherwise. If user did not set locking mechanisms, will allways return true
 */
bool LOGGER_Lock(void);


/// Yields exclusive access to log output, assuming that user provided locking mechanism
void LOGGER_Unlock(void);
#else
/**
 * @brief NO-OP
 */
static inline bool LOGGER_Lock(void) {
    return true;
}


/**
 * @brief NO-OP
 */
static inline void LOGGER_Unlock(void) {
}

#endif


/***************************************************************/
/************* FLUSH_HOOKS - Configurable features *************/
/***************************************************************/
#if 1 == CONFIG_LOGGER_FLUSH_HOOKS
/**
 * @brief Calls user flush function (if set)
 *
 */
void LOGGER_Flush(void);
#else
/**
 * @brief NO-OP
 */
static inline void LOGGER_Flush(void) {
}
#endif


/**********************************************************************/
/************* COMPILE_TIME_DEBUG - Configurable features *************/
/**********************************************************************/
#if 1 == LOGGER_COMPILE_TIME_DEBUG
/// Structure holding additional debugging information for this component
static struct {
    bool        loggerEnabled;      ///< Contains value of LOGGER_ENABLED
    char const* loggerChannel;      ///< Contains value of LOGGER_CHANNEL
    int         loggerChannelLevel; ///< Contains value of ***_CHANNEL_LEVEL
} LOGGER_CompileTimeDescriptor = {LOGGER_ENABLED, LOGGER_HEADER, LOGGER_CHANNEL_LEVEL};
#endif


/**
 * @brief Prints formatted string
 * @param[in] format cString containing format description
 * @param[in] ... format arguments
 * @warning does not perform sanity checks. Use only in conjunction with LOGGER_Lock LOGGER_SECTION_START
 */
void LOGGER_CHECK_FORMAT(1, 2) LOGGER_Print(char const* format, ...);


/**
 * @brief Prints log header: $TIMESTAMP $CHANNEL ($LEVEL) [FILE:LINE]:
 * @param[in] headerDescr header descriptor
 * @param[in] channelName cString containing channel name
 * @param[in] file cString containing file name
 * @param[in] line line number
 * @warning does not perform sanity checks. Use only in conjunction with LOGGER_Lock LOGGER_SECTION_START
 */
void LOGGER_PrintHeader(LOGGER_HeaderDescriptor descr);


/**
 * @brief Prints header with code location, and formated string, followed by new line character
 * @param[in] descr header descriptor
 * @param[in] format cString containing format description
 * @param[in] ... format arguments
 * @note this function actually performs sanity checks. To enable log compile-time optimization, use via LOGGER_PRINT_LINE macro
 */
void LOGGER_CHECK_FORMAT(2, 3) LOGGER_PrintLine(LOGGER_HeaderDescriptor descr, char const* format, ...);


/**
 * @brief Prints new line character
 * @warning does not perform sanity checks. Use only in conjunction with LOGGER_Lock @ref LOGGER_SECTION_START
 */
void LOGGER_PrintNL(void);


bool LOGGER_StartSection(void);


void LOGGER_EndSection(void);


// Ok. This is the point of no return - either log is enabled or disabled
#if (1 == LOGGER_ENABLED) && (LOGGER_LEVEL_DISABLED == LOGGER_CHANNEL_LEVEL)
#    undef LOGGER_ENABLED
#    define LOGGER_ENABLED 0
#endif


#if 1 == LOGGER_ENABLED
#    define LOGGER_PRINT_LINE(level, ...)                              \
        if (LOGGER_IS_VERBOSITY_ENABLED(level)) {                      \
            LOGGER_PrintLine(LOGGER_HEADER_DESCR(level), __VA_ARGS__); \
        }                                                              \
        LOGGER_INTERNAL_REQUIRE_TRAILING_SEMICOLON


#    define LOGGER_SECTION_START(level)                                    \
        if (LOGGER_IS_VERBOSITY_ENABLED(level) && LOGGER_StartSection()) { \
        LOGGER_PrintHeader(LOGGER_HEADER_DESCR(level))


#    define LOGGER_SECTION_CONTINUE(...) LOGGER_Print(__VA_ARGS__)


#    define LOGGER_SECTION_END() \
        LOGGER_EndSection();     \
        }                        \
        LOGGER_INTERNAL_REQUIRE_TRAILING_SEMICOLON

#    define LOGGER_SECTION_ENDL() \
        LOGGER_PrintNL();         \
        LOGGER_SECTION_END()


#else
#    define LOGGER_PRINT_LINE(level, ...)
#    define LOGGER_SECTION_START(level) \
        if (false) {                    \
        LOGGER_INTERNAL_REQUIRE_TRAILING_SEMICOLON

#    define LOGGER_SECTION_CONTINUE(...)
#    define LOGGER_SECTION_END() \
        }                        \
        LOGGER_INTERNAL_REQUIRE_TRAILING_SEMICOLON
#    define LOGGER_SECTION_ENDL() LOGGER_SECTION_END()
#endif


#ifdef __cplusplus
}
#endif


#endif
