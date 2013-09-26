#include "stdafx.h"
#include "informative_assert.h"
#include <misc_src/logger.h>
#include <sstream>
#include <string>

void assertion_failed(const char* expression, const char* function, const char* file, int line) {
  LOG("Failure: [" << expression << "] [file: " << file << ":" << line << "]");
}

void assertion_failed(const char* expression, const char* msg, const char* function, const char* file, int line) {
  LOG("Failure: [" << expression << "] [" << msg << "] [file: " << file << ":" << line << "]");
}
