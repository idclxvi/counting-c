#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdarg.h>

#define ZERO_FILE_NAME "zero.bin"
#define ZERO_FILE_SIZE 1024

#define SEQ_FILE_NAME "seq.bin"
#define SEQ_FILE_SIZE 4096

#define DOUBLE_FILE_NAME "double.bin"
#define DOUBLE_FILE_SIZE 4096

#define THIRD_FILE_NAME "third-unique.bin"
#define THIRD_FILE_SIZE 8192

/* must ge from max file-size */
#define BUF_SIZE 8192

void fatal(const char *fmt, ...)
{
    va_list valist;
    va_start(valist, fmt);
    vfprintf(stderr, fmt, valist);
    fprintf(stderr, "\n");
    exit(255);
}

int create_or_override_file(const char *fname)
{
    int fd = open(fname, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (0 > fd)
    {
        fatal("Cant create '%s'", fname);
    }
    return fd;
}

void write_u32_to_file(int fd, void *buf, size_t size)
{
    size *= sizeof(uint32_t);
    ssize_t n = write(fd, (const void *)buf, size);
    if ((0 > n) || ((size_t)n != size))
    {
        fatal("Failed write to file: %u -> %d", size, n);
    }
}

int main()
{

    int i, m, p, fd;
    uint32_t *buf;

    if (NULL == (buf = calloc(BUF_SIZE, sizeof(uint32_t))))
    {
        fatal("Can't allocate buffer for %d uint32_t", BUF_SIZE);
    }

    /* File with zeroes */
    fd = create_or_override_file(ZERO_FILE_NAME);
    write_u32_to_file(fd, buf, ZERO_FILE_SIZE);
    close(fd);

    /* Seq file */
    fd = create_or_override_file(SEQ_FILE_NAME);
    for (i = 0; i < SEQ_FILE_SIZE; ++i)
    {
        buf[i] = (uint32_t)i;
    }
    write_u32_to_file(fd, buf, SEQ_FILE_SIZE);
    close(fd);

    /* Double file */
    fd = create_or_override_file(DOUBLE_FILE_NAME);
    m = 0;
    for (i = 0; i < DOUBLE_FILE_SIZE; ++i)
    {
        buf[i] = (uint32_t)(m);
        m += (i & 1) ? 1 : 0;
    }
    write_u32_to_file(fd, buf, DOUBLE_FILE_SIZE);
    close(fd);

    /* 3rd unique number file */
    fd = create_or_override_file(THIRD_FILE_NAME);
    m = p = 0;
    for (i = 0; i < THIRD_FILE_SIZE; ++i)
    {
        buf[i] = (uint32_t)(m);
        switch (++p)
        {
        case 2:
        case 4:
        case 5:
            ++m;
            break;
        }
        p %= 5;
    }
    write_u32_to_file(fd, buf, THIRD_FILE_SIZE);
    close(fd);
}