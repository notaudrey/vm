#pragma once

#include <string.h>
#include <time.h>

// ANSI color escapes
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"


#ifdef __DEBUG_BUILD
    // Debug print statements
    // [timestamp] [file:line:function] [TYPE] formatted_message
    // 
    // BE WARNED: This method of printing out the timestamp does a LOT of 
    // malloc() calls.
    #define _LOG(type, fmt, args...) do { \
        /* Oh this is so ugly :( There has to be a better way to do timestamps */ \
        time_t now; \
        time(&now); \
        char* time = ctime(&now); \
        /* Trim off the trailing newline from ctime() */ \
        time[strlen(time) - 1] = '\0'; \
        /* Print everything */ \
        fprintf(stderr, WHT "[" YEL "%s" WHT "] [" RED "%s" WHT ":" BLU "%d" \
            WHT ":" GRN "%s()" WHT "] " WHT "[" BLU "%s" WHT "] " fmt RESET, \
            time, __FILE__, __LINE__, __func__, type, ##args);\
        } while(0);
    #define DEBUG(fmt, args...) _LOG("DEBUG", fmt, ##args)
#else
    // Not debug mode? Don't print anything
    #define DEBUG(fmt, args...)
#endif

