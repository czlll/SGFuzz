/*
 * CGC Compatibility Layer for SGFuzz
 * Replaces libcgc.h with standard library implementations
 */
#ifndef LIBCGC_H
#define LIBCGC_H

#include <stddef.h>
#include <string.h>
#include <setjmp.h>

/* Standard file descriptors */
#define STDIN  0
#define STDOUT 1
#define STDERR 2

/* Fuzzer input buffer - set by harness before each test */
extern const unsigned char *__fuzz_data;
extern size_t __fuzz_size;
extern size_t __fuzz_pos;
extern jmp_buf __fuzz_jmp_buf;
extern int __fuzz_should_exit;

/*
 * CGC receive() - Read from fuzzer input buffer instead of stdin
 */
static inline int receive(int fd, void *buf, size_t count, size_t *rx_bytes) {
    (void)fd;  /* Ignore fd, always read from fuzz buffer */

    if (__fuzz_pos >= __fuzz_size) {
        if (rx_bytes) *rx_bytes = 0;
        return 0;
    }

    size_t available = __fuzz_size - __fuzz_pos;
    size_t to_read = (count < available) ? count : available;

    memcpy(buf, __fuzz_data + __fuzz_pos, to_read);
    __fuzz_pos += to_read;

    if (rx_bytes) *rx_bytes = to_read;
    return 0;
}

/*
 * CGC transmit() - Silently discard output
 */
static inline int transmit(int fd, const void *buf, size_t count, size_t *tx_bytes) {
    (void)fd;
    (void)buf;
    if (tx_bytes) *tx_bytes = count;
    return 0;
}

/*
 * CGC _terminate() - Jump back to harness instead of exiting
 */
static inline void _terminate(int code) {
    (void)code;
    __fuzz_should_exit = 1;
    longjmp(__fuzz_jmp_buf, 1);
}

#endif /* LIBCGC_H */
