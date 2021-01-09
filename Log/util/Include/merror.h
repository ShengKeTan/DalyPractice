#ifndef __ERROR_H__
#define __ERROR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cstdio>
#include <cerrno>
#include <cassert>

#ifdef _DEBUG
#define info_display(...) fprintf(stdout, __VA_ARGS__)

/* generic error */
#define error_display(...) do { \
    fprintf(stderr, __VA_ARGS__); \
    assert(0); \
} while (0)

/* fatal error */
#define exit_throw(...) do { \
    fprintf(stderr, "Error defined at %s, line : %i\n", __FILE__, __LINE__); \
    fprintf(stderr, "Errno : %d, msg : %s\n", errno, strerror(errno)); \
    fprintf(stderr, "%s\n", __VA_ARGS__); \
} while (0)

#else
#define info_display(...)

#define error_display(...)

#define exit_throw(...) do { \
    error_display("Error : %d, msg : %s\n", errno, strerror(errno)); \
    exit(errno); \
} while (0)

#endif // _DEBUG

#ifdef __cplusplus
}
#endif

#endif // __ERROR_H__