//
// Created by SIMON on 6/23/2022.
//
#include <iostream>
#include <cstdlib>

#ifndef OMR_PLATFORM_LOG_H
#define OMR_PLATFORM_LOG_H

#ifdef __ANDROID__
#include <android/log.h>
#endif

using namespace std;

void platform_log(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
#ifdef __ANDROID__
    __android_log_vprint(ANDROID_LOG_VERBOSE, "ndk", fmt, args);
#else
    vprintf(fmt, args);
#endif
    va_end(args);
}

#endif //OMR_PLATFORM_LOG_H
