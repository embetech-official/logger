# LOGGER - Lightweight, portable formatted logging component

[![C++ Unit Tests](https://github.com/embetech-official/logger/actions/workflows/cpp_unit_tests.yml/badge.svg)](https://github.com/embetech-official/logger/actions/workflows/cpp_unit_tests.yml)

# Overview
Logger is a lightweight and portable logging component written in C99, with some preprocessor magic (but C99-compliant (... yet magical :)) ).

Features:
- Supports printf-compliant format syntax
- Built in format syntax checking on (GNU compilers)
- Unlimited amount of compile-time channels
- SYSLOG compatibile verbosity levels
- Runtime output stream selection
- Thread-safe mechanisms ready


The library consists of two parts: compile-time channel and verbosity selection, as well as some runtime switches.

For each channel you may specify maximum verbosity level that will be compiled - messages with higher verbosity will be optimized-out from build, thus reducing memory requirements.


# Installation
## CMake
Logger is compatibile with almost every possible CMake include option:
|                         | usable             | remarks                                                   |
|-------------------------|--------------------|-----------------------------------------------------------|
| add_subdirectory        | :heavy_check_mark: | -                                                         |
| find_package            | :heavy_check_mark: | append download directory to CMAKE_PREFIX_PATH            |
| include                 | :x:                | theoretically it can be used, but prefer add_subdirectory |
| FetchContent            | :heavy_check_mark: | -                                                         |
| install -> find_package | :heavy_check_mark: | remember that installation will freeze available features |

example configuration using find_package:
``` cmake
set(LOGGER_CUSTOM_AFFIXES ON) # Available options are described later
find_package(logger REQUIRED)
target_link_libraries(example PUBLIC embetech::logger)
```
# Configuration
## Translation unit configuration
Simply add channel definition at the beginning of your source file:
``` C
#include <embetech/logger.h>

// your code starts here ...
void example_function(void) {
    LOGGER_INFO("eat veggies");
    LOGGER_WARNING("... or else 3:> ");
}
```
This will result in the following log message: 
```
DEFAULT (I): eat veggies
DEFAULT (W): ... or else 3:>
```

You can assing your source file to compile-time **Log Channel** to enable precise verbosity configuration. The **Log Channel** will be visible in message's header:
``` C
#define LOGGER_CHANNEL FOOBAR
#include <embetech/logger.h>

// your code starts here ...
void example_function(void) {
    LOGGER_INFO("eating veggies is fun");
    }
```
This will result in the following log message: 
```
FOOBAR (I): eating veggies is fun
```

Logger is capable of controling which messages are compiled into your binary file. You can control this by setting each channel verbosity. If you don't specify verbosity for a channel, it will remain disabled

## Verbosity configuration
Global configuration shall be stored in logger_config.h file The example below presents all configuration options with their default values:
 ``` C
#ifndef LOGGER_CONFIG_H_ // Include guard naming convention is not enforced, but endorsed :)
#define LOGGER_CONFIG_H_

#define LOGGER_ENABLED 1 ///< Main Component On/Off switch. If set to 0, even if every other conditions are met, logger will write nothing

/********* THIS IS THE PART WHERE YOU SPECIFY YOUR OWN CHANNELS AND SUBCHANNELS *********/

#define COMPONENT1_LOG_CHANNEL_LEVEL LOGGER_LEVEL_INFO ///< Verbosity setting for channel COMPONENT1

#define COMPONENT2_LOG_CHANNEL_LEVEL LOGGER_LEVEL_WARNING ///< Verbosity setting for channel COMPONENT2

#define APP_LOG_CHANNEL_LEVEL LOGGER_LEVEL_EMERGENCY ///< Verbosity setting for channel APP

#define SANITY_CHECKS_LOG_CHANNEL_LEVEL LOGGER_LEVEL_DISABLED ///< Verbosity setting (a.k.a disabling) of channel SANITY_CHECKS :)


#endif // That's all folks
 ```

## Features configuration
In order to stay flexible and minimalistic logger uses compile-time configuration options that will affect global behaviour. The list below reflects options available in CMakeLists.txt file:
### LOGGER_TIMESTAMPS
If turned on, the Logger header format will contain timestamp, acquired from the user defined callback via LOGGER_SetTimeSource function:
```C
LOGGER_SetTimeSource([](){return std::uint32_t(8000085);}); // Short C++ lambda to save the world
LOGGER_INFO("test message");
```
will be printed as (assuming DEFAULT channel):
```
8000085 DEFAULT (I): test message
```

### LOGGER_HUMAN_READABLE_TIMESTAMP
This option is available only when LOGGER_TIMESTAMPS is ON.
Changes the format of timestamp to hh:mm::ss.ms:
```
02:13:20.085 DEFAULT (I): test message
```
### LOGGER_CUSTOM_AFFIXES
Allows setting custom prefix/suffix for every Logger message, using LOGGER_SetPrefix/LOGGER_SetSuffix function.
Both will be printed as binary data, so no '\0' termination is required.
This feature might be useful when working with custom terminal protocols.

```C
        LOGGER_SetPrefix("pre- ", 5U);
        LOGGER_SetSuffix(" -post", 6U);
        LOGGER_NOTICE("test message1");
```
will produce
```
pre- DEFAULT (N): test message -post
```
### LOGGER_RUNTIME_VERBOSITY
Enables reducing verbosity of printed messages AT RUNTIME using  LOGGER_SetRuntimeLevel function. Of course, compile time level is still stronger so the resulting set of messages will be no greater than both:
```C
    LOGGER_NOTICE("test message1");
    LOGGER_SetRuntimeLevel(LOGGER_LEVEL_WARNING);
    LOGGER_NOTICE("test message2");
    LOGGER_SetRuntimeLevel(LOGGER_LEVEL_DEBUG);
    LOGGER_NOTICE("test message3");

```
will produce
```
DEFAULT (N): test message1
DEFAULT (N): test message3
```
the second message will not be printed, however its code is still available to be enabled (assuming that DEFAULT_LOG_CHANNEL_LEVEL is at least at NOTICE)

### LOGGER_HEADER_WITH_LOCATION
Expands the header with code location [file:line]:
```
DEFAULT (N) [x:\long_path\file.cpp:66]: test message
```
Since file paths might be pretty long, we added CMake util to override compiler-generated file names with shorter versions:
``` cmake
# assuming the logger library is already found
include(logger_utils)
logger_normalize_printable_filenames()
```
this would change the above message to
```
DEFAULT (N) [file.cpp:66]: test message
### LOGGER_THREAD_SAFETY_HOOKS
By design, logger is as thread-safe as your output callback (so probably no).
When this option is enabled. Logger will use user-provided lock/unlock functions to ensure thread safety.
To do so, you should register lock/unlock callbacks:
``` C
static Mutex_t myMutex;

bool myLockFunction(void* context) {
    int const customTimeoutMS = 100; // If your mutex requires timeout. you may also wrap it in terms of context
    int const mutexSuccess = 69; // Assuming that Mutex_Take return 69 on success
    Mutex_t* mutex = (Mutex_t*)context;
    return mutexSuccess == Mutex_Take(mutex, customTimeoutMS); // let's assume that if succeded, the function returns 69
}

void myUnlockFunction(void* context) {
    Mutex_t* mutex = (Mutex_t*)context;
    Mutex_Give(mutex);
}
// ...
LOGGER_SetLockingMechanism(myLockFunction, myUnlockFunction, &myMutex);

```
The API of callbacks is fairly universal... You might be able to simply plug your OS's mutex functions.

LOGGER will always try to lock before printing message. In scoped printing case, The lock will be acquired by LOGGER_START, and released by LOGGER_END/LOGGER_ENDL.

### LOGGER_FLUSH_HOOKS
Enables user to bind function, to flush output. The function may either be called explicitly, using LOGGER_Flush(), or automatically on the end of each message (So every LOGGER_INFO/etc. or after each LOGGER_END/LOGGER_ENDL):
```C
LOGGER_DisableHeader();
LOGGER_NOTICE("marco?");

LOGGER_SetFlushHook([](){puts("polo!");}, false); // Logger will flush when asked to
LOGGER_NOTICE("marco??");
LOGGER_NOTICE("marco???");
LOGGER_Flush();
LOGGER_SetFlushHook([](){puts("polo!!");}, true); // now logger will flush automatically
LOGGER_NOTICE("marco!");
```
would print
```
marco?
marco??
marco???
polo!
marco!
polo!!
```

### LOGGER_VERBOSE_ERRORS
When user messed up, it may be difficult to dig through all preprocessor magic. When this option is enabled, and your compiler is eiter clang or GCC-like, each compile-time error will be appended with message, which logger channel was the culprit



# Usage

## Minimal configuration
In order to make the logger print anything, you have to provide output function callback beforehand:
``` C
void out(char c, void* context) {
    (void)context; // unused parameter
    putchar(c);
}

LOGGER_SetOutput(out, NULL);
```
You may provide context for your function and it will be stored until next invocation of LOGGER_SetOutput.
The next step is to enable logger:
``` C
assert(LOGGER_Enable()); // Assert SHOULD be skipped, as this funtion is probably not that critical... Yet the function returns whether the logger was enabled
```

## Logging messages
We prepared two intended ways to produce log messages. Both will be presented in example below:
``` C
void example_function(void) {
    LOGGER_TRACE("Trace message. Rarely enabled");
    LOGGER_DEBUG("Entering function in " __FILE__ ":%d", __LINE__);
    LOGGER_VERBOSE("Attempting to start to defuse the nuke");
    LOGGER_INFO("Nuke defusal kit prepared");
    LOGGER_NOTICE("Nuke defusing started");
    LOGGER_WARNING("The Apple authorized service might not be the best place to fix the bomb");
    LOGGER_ERROR("Screwdriver broken");
    LOGGER_CRITICAL("Nuke warranty voided. Reason: liquid damage");
    LOGGER_ALERT("Nuke will blow in %d seconds", 3);
    LOGGER_EMERGENCY("Goodbye cruel world");
    LOGGER_DISABLED("There is no bright light at the end of the tunnel"); // Actually useful, when you want to disable some low level debugging messages in low latency code, and not comment it out (remember: commented-out code is a bad code)

    LOGGER_START(NOTICE);
    int trainingMsgNo = 42; // Notice that the scope of trainingMsgNo is reduced to LOGGER_START/LOGGER_ENDL
    LOGGER_CONTINUE("\n --- This was a training message no %d: \n", trainingMsgNo);
    LOGGER_CONTINUE("%s\n", getPrintableMsgNo(trainingMsgNo));
    LOGGER_CONTINUE("you may log as many information, as you wish.\n");
    LOGGER_CONTINUE("log channel between LOGGER_START and LOGGER_END is locked");
    LOGGER_CONTINUE("(assuming you implemented locking mechanism)");
    LOGGER_ENDL();
    
    LOGGER_NOTICE("bananas.")
}

```
This will produce the following output:
```
DEFAULT (T): Trace message. Rarely enabled
DEFAULT (D): Entering function in ./example.c:499
DEFAULT (V): Attempting to start to defuse the nuke
DEFAULT (I): Nuke defusal kit prepared
DEFAULT (N): Nuke defusing started
DEFAULT (W): The Apple authorized service might not be the best place to fix the bomb
DEFAULT (E): Screwdriver broken
DEFAULT (C): Nuke warranty voided. Reason: liquid damage
DEFAULT (A): Nuke will blow in 3 seconds
DEFAULT (M): Goodbye cruel world
DEFAULT (N): 
 --- This was a training message no 42: 
foo
you may log as many information, as you wish.
log channel between LOGGER_START and LOGGER_END is locked(assuming you implemented locking mechanism)
DEFAULT    (N): bananas
```

## Header runtime control
User may disable printing header, simply using LOGGER_EnableHeader/LOGGER_DisableHeader functions