#pragma once

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
    #define DEBUG(type, fmt, args...) fprintf(stderr, WHT "[" RED "%s" WHT ":" \
            BLU "%d" WHT ":" GRN "%s()" WHT "] " WHT "[" BLU "%s" WHT "] " fmt \
            RESET, __FILE__, __LINE__, __func__, type, ## args);
#else
    // Not debug mode? Don't print anything
    #define DEBUG(type, fmt, args...)
#endif

