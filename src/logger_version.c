#include "embetech/logger.h"
#include <assert.h>

#ifndef LOGGER_VERSION
#error "LOGGER_VERSION is not defined"
#endif

static_assert(sizeof(LOGGER_VERSION) <= 64, "LOGGER_VERSION too long"); // NOLINT(readability-magic-numbers) - 64 bytes max

char const *LOGGER_GetVersionString(void) { return (LOGGER_VERSION); }
