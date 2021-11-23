
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <sys/errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>

// Assuming 64bits platform
// 536_870_912 bytes - 67_108_864 uint64
#define BITS_SIZE (size_t)((uint64_t)((uint64_t)UINT32_MAX + (uint64_t)1) / (uint64_t)64)
#define bit_index(bn) (uint64_t)((uint64_t)(bn) / (uint64_t)64)
#define bit_mask(bn) (uint64_t)((uint64_t)1 << ((uint64_t)(bn) % (uint64_t)64))
#define bit_value(ba, bi, bm) ((ba)[(bi)] & (bm))
#define bit_set(ba, bi, bm) ((ba)[(bi)] |= (bm))

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define BUF_SIZE (4096 * sizeof(uint32_t))

void fatal(const char *fmt, ...)
{
    va_list valist;
    va_start(valist, fmt);
    vfprintf(stderr, fmt, valist);
    if (0 != errno)
    {
        fprintf(stderr, ": %s", strerror(errno));
    }
    fprintf(stderr, "\n");
    exit(255);
}

int main(int ac, char *av[])
{
    int fd, nmbrs, i, uniques = 0, non_singles = 0;
    size_t len;
    struct stat statbuf;
    uint32_t *buf;
    off_t ofs;
    uint64_t *bits;
    uint64_t *single_bits;

    if (2 != ac)
    {
        fprintf(stderr, "usage:\n\t%s <file-name>\n", av[0]);
        exit(1);
    }
    if (0 > (fd = open(av[1], O_RDONLY)))
    {
        fatal("Can't open file '%s'", av[1]);
    }
    if (0 > fstat(fd, &statbuf))
    {
        fatal("fstat('%s')", av[1]);
    }
    if (0 != (statbuf.st_size % sizeof(uint32_t)))
    {
        fatal("File '%s' has wrong size: %u", av[1], statbuf.st_size);
    }
    if (NULL == (bits = calloc(BITS_SIZE, sizeof(uint64_t))))
    {

        fatal("Cant allocate bits memory %d x %d", BITS_SIZE, sizeof(uint64_t));
    }
    if (NULL == (single_bits = calloc(BITS_SIZE, sizeof(uint64_t))))
    {

        fatal("Cant allocate singles bits memory %d x %d", BITS_SIZE, sizeof(uint64_t));
    }
    len = MIN(statbuf.st_size, BUF_SIZE);
    for (ofs = 0; ofs < statbuf.st_size; ofs += (off_t)len)
    {
        printf("mmap(%llu,%llu)\n", len, ofs);
        if (MAP_FAILED == (buf = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, ofs)))
        {
            fatal("Can't mmap input file");
        }
        nmbrs = len / sizeof(uint32_t);
        for (i = 0; i < nmbrs; ++i)
        {
            int bi = bit_index(buf[i]);
            uint64_t bm = bit_mask(buf[i]);
            if (bit_value(bits, bi, bm))
            {
                if (!bit_value(single_bits, bi, bm))
                {
                    bit_set(single_bits, bi, bm);
                    non_singles++;
                }
            }
            else
            {
                uniques++;
                bit_set(bits, bi, bm);
            }
        }
        if (0 > munmap(buf, len))
        {
            fatal("munmap failed");
        }
        len = MIN((statbuf.st_size - ofs), BUF_SIZE);
    }

    printf("Unique numbers: %d\nSingles: %d\n", uniques, uniques - non_singles);

    return 0;
}