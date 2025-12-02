# LOGGER — Lightweight, Portable, Formatted Logging Component

[![Unit Tests](https://github.com/embetech-official/logger/actions/workflows/unit_tests.yml/badge.svg)](https://github.com/embetech-official/logger/actions/workflows/unit_tests.yml)
![GitHub License](https://img.shields.io/github/license/embetech-official/logger)
![GitHub Release](https://img.shields.io/github/v/release/embetech-official/logger)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/60b12b600a6842f8bf1e81a29c2ad4fe)](https://app.codacy.com/gh/embetech-official/logger/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

## Overview

Logger is a lightweight and portable logging component written in C99, with some preprocessor utilities (still C99-compliant).

Features:

- Supports `printf`-compliant format syntax
- Built-in format syntax checking (GNU compilers)
- Unlimited number of compile-time channels
- SYSLOG-compatible verbosity levels
- Runtime output stream selection
- Thread-safety hooks available

The library consists of two parts: compile-time channel/verbosity selection and runtime switches.

For each channel you may specify the maximum verbosity level that will be compiled — messages with higher verbosity are optimized out from the build, reducing memory requirements.

## Quick Start

```cmake
include(FetchContent)
FetchContent_Declare(
    logger
    GIT_REPOSITORY https://github.com/embetech-official/logger.git
    GIT_TAG main
)

FetchContent_MakeAvailable(logger)
```

## Configuration

### Translation Unit Configuration

Add the channel definition at the beginning of your source file:

```C
#include <embetech/logger.h>

// your code starts here ...
void example_function(void) {
    LOGGER_INFO("eat veggies");
    LOGGER_WARNING("... or else 3:> ");
}
```

This will produce the following log messages:

```shell
DEFAULT (I): eat veggies
DEFAULT (W): ... or else 3:>
```

You can assign your source file to a compile-time **Log Channel** to enable precise verbosity configuration. The **Log Channel** will be visible in the message header:

``` C
#define LOGGER_CHANNEL FOOBAR
#include <embetech/logger.h>

// your code starts here ...
void example_function(void) {
    LOGGER_INFO("eating veggies is fun");
    }

```

This will result in the following log message:

```shell
FOOBAR (I): eating veggies is fun
```

Logger can control which messages are compiled into your binary. You control this by setting each channel's verbosity. If you don't specify a verbosity for a channel, it remains disabled.

### Verbosity Configuration

Global configuration should be stored in `logger_config.h`. The example below presents all configuration options with their default values:

 ```C
#ifndef LOGGER_CONFIG_H_ // Include guard naming convention is not enforced, but endorsed :)
#define LOGGER_CONFIG_H_

#define LOGGER_ENABLED 1 ///< Main component On/Off switch. If set to 0, even if every other condition is met, the logger will write nothing.

/********* SPECIFY YOUR OWN CHANNELS BELOW *********/

#define COMPONENT1_LOG_CHANNEL_LEVEL LOGGER_LEVEL_INFO ///< Verbosity setting for channel COMPONENT1

#define COMPONENT2_LOG_CHANNEL_LEVEL LOGGER_LEVEL_WARNING ///< Verbosity setting for channel COMPONENT2

#define APP_LOG_CHANNEL_LEVEL LOGGER_LEVEL_EMERGENCY ///< Verbosity setting for channel APP

#define SANITY_CHECKS_LOG_CHANNEL_LEVEL LOGGER_LEVEL_DISABLED ///< Verbosity setting (a.k.a disabling) of channel SANITY_CHECKS :)


#endif // That's all folks
 ```

### Features Configuration

To stay flexible and minimalistic, the logger uses compile-time configuration options that affect global behavior. The list below reflects options available in the `CMakeLists.txt` file:

### LOGGER_TIMESTAMPS

If enabled, the logger header contains a timestamp acquired from a user-defined callback via `LOGGER_SetTimeSource`:

```C
LOGGER_SetTimeSource([](){return std::uint32_t(1);}); // Short C++ lambda
LOGGER_INFO("test message");
```

Printed as (assuming DEFAULT channel):

```shell
1 DEFAULT (I): test message
```

### LOGGER_HUMAN_READABLE_TIMESTAMP

Available only when `LOGGER_TIMESTAMPS` is ON.
Changes the timestamp format to `hh:mm:ss.ms`:

```shell
02:13:20.085 DEFAULT (I): test message
```

### LOGGER_CUSTOM_AFFIXES

Allows setting a custom prefix/suffix for every logger message using `LOGGER_SetPrefix`/`LOGGER_SetSuffix`.
Both are printed as binary data, so no '\0' termination is required.
Useful when working with custom terminal protocols.

```C
        LOGGER_SetPrefix("pre- ", 5U);
        LOGGER_SetSuffix(" -post", 6U);
        LOGGER_NOTICE("test message1");
```

Produces:

```shell
pre- DEFAULT (N): test message -post
```

### LOGGER_RUNTIME_VERBOSITY

Enables reducing verbosity of printed messages at runtime using `LOGGER_SetRuntimeLevel`. Compile-time level still applies, so the resulting set of messages is bounded by both:

 ```C
    LOGGER_NOTICE("test message1");
    LOGGER_SetRuntimeLevel(LOGGER_LEVEL_WARNING);
    LOGGER_NOTICE("test message2");
    LOGGER_SetRuntimeLevel(LOGGER_LEVEL_DEBUG);
    LOGGER_NOTICE("test message3");

```

Produces:

```shell
DEFAULT (N): test message1
DEFAULT (N): test message3
```

The second message is not printed; however, its code remains available to be enabled (assuming that `DEFAULT_LOG_CHANNEL_LEVEL` is at least `NOTICE`).

### LOGGER_HEADER_WITH_LOCATION

Expands the header with code location `[file:line]`:

```shell
DEFAULT (N) [x:\long_path\file.cpp:66]: test message
```

Since file paths might be long, a CMake utility can override compiler-generated file names with shorter versions:

``` cmake
# assuming the logger library is already found
include(logger_utils)
logger_normalize_printable_filenames()
```

This changes the above message to:

```shell
DEFAULT (N) [file.cpp:66]: test message
```

### LOGGER_THREAD_SAFETY_HOOKS

By design, the logger is as thread-safe as your output callback (often: not).
When this option is enabled, the logger uses user-provided lock/unlock functions to ensure thread safety.
Register lock/unlock callbacks:

``` C
static Mutex_t myMutex;

bool myLockFunction(void* context) {
    int const customTimeoutMS = 100; // If your mutex requires a timeout
    int const mutexSuccess = 69; // Assuming that Mutex_Take returns 69 on success
    Mutex_t* mutex = (Mutex_t*)context;
    return mutexSuccess == Mutex_Take(mutex, customTimeoutMS);
}

void myUnlockFunction(void* context) {
    Mutex_t* mutex = (Mutex_t*)context;
    Mutex_Give(mutex);
}
// ...
LOGGER_SetLockingMechanism(myLockFunction, myUnlockFunction, &myMutex);
```

The callback API is universal — you can often plug your OS's mutex functions.

The logger always tries to lock before printing a message. In scoped printing, the lock is acquired by `LOGGER_START` and released by `LOGGER_END`/`LOGGER_ENDL`.

### LOGGER_FLUSH_HOOKS

Enables binding a function to flush the output. The function may be called explicitly using `LOGGER_Flush()`, or automatically at the end of each message (i.e., every `LOGGER_INFO`/etc., or after each `LOGGER_END`/`LOGGER_ENDL`):

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

Would print:

```shell
marco?
marco??
marco???
polo!
marco!
polo!!
```

### LOGGER_VERBOSE_ERRORS

When a user makes a mistake, preprocessor errors can be hard to parse. If enabled, and your compiler is either Clang or GCC-like, each compile-time error is appended with information indicating which logger channel caused the issue.

## Usage

### Minimal Configuration

To make the logger print anything, you must provide the output function callback first:

``` C
void out(char c, void* context) {
    (void)context; // unused parameter
    putchar(c);
}

LOGGER_SetOutput(out, NULL);
```

You may provide a context pointer for your function; it is stored until the next invocation of `LOGGER_SetOutput`.
Next, enable the logger:

``` C
assert(LOGGER_Enable()); // The function returns whether the logger was enabled
```

## Logging Messages

There are two intended ways to produce log messages. Both are presented below:

``` C
void example_function(void) {
    LOGGER_TRACE("Trace message. Rarely enabled");
    LOGGER_DEBUG("Entering function in " __FILE__ ":%d", __LINE__);
    LOGGER_VERBOSE("Attempting to start to defuse the nuke");
    LOGGER_NOTICE("Nuke defusing started");
    LOGGER_WARNING("The Apple authorized service might not be the best place to fix the bomb");
    LOGGER_ERROR("Screwdriver broken");
    LOGGER_CRITICAL("Nuke warranty voided. Reason: liquid damage");
    LOGGER_ALERT("Nuke will blow in %d seconds", 3);
    LOGGER_EMERGENCY("Goodbye cruel world");
    LOGGER_DISABLED("There is no bright light at the end of the tunnel"); // Actually useful, when you want to disable some low level debugging messages in low latency code, and not comment it out (remember: commented-out code is a bad code)

    LOGGER_START(NOTICE);
    int trainingMsgNo = 42; // Scope reduced to LOGGER_START/LOGGER_ENDL
    LOGGER_CONTINUE("\n --- This was a training message no %d: \n", trainingMsgNo);
    LOGGER_CONTINUE("%s\n", getPrintableMsgNo(trainingMsgNo));
    LOGGER_CONTINUE("you may log as much information as you wish.\n");
    LOGGER_CONTINUE("log channel between LOGGER_START and LOGGER_END is locked");
    LOGGER_CONTINUE("(assuming you implemented a locking mechanism)");
    LOGGER_ENDL();
    
    LOGGER_NOTICE("bananas.")
}
```

This produces the following output:

```shell
DEFAULT (T): Trace message. Rarely enabled
DEFAULT (D): Entering function in ./example.c:499
DEFAULT (V): Attempting to start to defuse the nuke
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
DEFAULT (N): bananas
```

### Header Runtime Control

You can disable/enable printing the header using `LOGGER_EnableHeader` / `LOGGER_DisableHeader`.
