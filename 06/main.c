#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/time.h>

const char *B64 =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static uint8_t
b64(uint8_t x)
{
    if (x < 26)
        return 'A' + x;

    if (x < 52)
        return 'a' + x;

    if (x < 62)
        return '0' + x;

    if (x == 62)
        return '+';

    return '/';
}

static void
fprint_u8a(FILE *stream, const uint8_t *a, size_t len)
{
    for (size_t i = 0; i < len; ++i)
        fprintf(stream, "%02x%c", a[i], i + 1 < len ? ' ' : '\n');
}

void
base64triple_1(uint8_t b[4], const uint8_t t[3])
{
    b[0] = B64[(/*         */ (t[0] >> 2)) & 63];
    b[1] = B64[((t[0] << 4) | (t[1] >> 4)) & 63];
    b[2] = B64[((t[1] << 2) | (t[2] >> 6)) & 63];
    b[3] = B64[((t[2] << 0) /*         */) & 63];
}

void
base64triple_2(uint8_t b[4], const uint8_t t[3])
{
    b[0] = B64[(/*         */ (t[0] >> 2)) & 255];
    b[1] = B64[((t[0] << 4) | (t[1] >> 4)) & 255];
    b[2] = B64[((t[1] << 2) | (t[2] >> 6)) & 255];
    b[3] = B64[((t[2] << 0) /*         */) & 255];
}

void
base64triple_3(uint8_t b[4], const uint8_t t[3])
{
    uint32_t u;

    u = (t[0] << 16) | (t[1] << 8) | t[2];

    b[0] = B64[(u >> (3 * 6)) & 63];
    b[1] = B64[(u >> (2 * 6)) & 63];
    b[2] = B64[(u >> (1 * 6)) & 63];
    b[3] = B64[(u >> (0 * 6)) & 63];
}

void
base64triple_4(uint8_t b[4], const uint8_t t[3])
{
    uint32_t u;

    u = (t[0] << 16) | (t[1] << 8) | t[2];

    b[0] = B64[(u >> (3 * 6)) & 255];
    b[1] = B64[(u >> (2 * 6)) & 255];
    b[2] = B64[(u >> (1 * 6)) & 255];
    b[3] = B64[(u >> (0 * 6)) & 255];
}

void
base64triple_5(uint8_t b[4], const uint8_t t[3])
{
    uint32_t u, v;

    u = (t[0] << 16) | (t[1] << 8) | t[2];

    v = 0;
    v |= B64[(u >> (3 * 6)) & 63];
    v |= B64[(u >> (2 * 6)) & 63];
    v |= B64[(u >> (1 * 6)) & 63];
    v |= B64[(u >> (0 * 6)) & 63];

    b[0] = (v >> (3 * 8)) & 255;
    b[1] = (v >> (2 * 8)) & 255;
    b[2] = (v >> (1 * 8)) & 255;
    b[3] = (v >> (0 * 8)) & 255;
}

void
base64triple_6(uint8_t b[4], const uint8_t t[3])
{
    uint32_t u, v;

    u = (t[0] << 16) | (t[1] << 8) | t[2];

    v = 0;
    v |= B64[(u >> (3 * 6)) & 63];
    v |= B64[(u >> (2 * 6)) & 63];
    v |= B64[(u >> (1 * 6)) & 63];
    v |= B64[(u >> (0 * 6)) & 63];

    *((uint32_t *)b) = v;
    /*
    b[0] = (v >> (3 * 8)) & 255;
    b[1] = (v >> (2 * 8)) & 255;
    b[2] = (v >> (1 * 8)) & 255;
    b[3] = (v >> (0 * 8)) & 255;
    */
}

void
base64triple_7(uint8_t b[4], const uint8_t t[3])
{
    uint32_t u;

    u = (t[0] << 16) | (t[1] << 8) | t[2];

    b[0] = b64((u >> (3 * 6)) & 63);
    b[1] = b64((u >> (2 * 6)) & 63);
    b[2] = b64((u >> (1 * 6)) & 63);
    b[3] = b64((u >> (0 * 6)) & 63);
}

void
base64triple_8(uint8_t b[4], const uint8_t t[3])
{
    uint32_t u, v;

    u = *((uint32_t *)t);
    v = htonl(u);
    /*v = __builtin_bswap32(u);*/

    b[0] = B64[(v >> (8 + 3 * 6)) & 63];
    b[1] = B64[(v >> (8 + 2 * 6)) & 63];
    b[2] = B64[(v >> (8 + 1 * 6)) & 63];
    b[3] = B64[(v >> (8 + 0 * 6)) & 63];
}

void
base64triple_9(uint8_t *restrict b, const uint8_t *restrict t)
{
    uint32_t u;

    u = (t[0] << 16) | (t[1] << 8) | t[2];

    b[0] = B64[(u >> (3 * 6)) & 63];
    b[1] = B64[(u >> (2 * 6)) & 63];
    b[2] = B64[(u >> (1 * 6)) & 63];
    b[3] = B64[(u >> (0 * 6)) & 63];
}

void
fill(uint8_t *dst, size_t size)
{
    for (size_t i = 0; i < size; ++i)
        dst[i] = rand();
}

void
time_function(void triple(uint8_t *, const uint8_t *), const char *name,
              uint8_t *dst, const uint8_t *src, size_t triples)
{
    struct timeval start, end;
    double duration, rate;

    gettimeofday(&start, NULL);

    for (size_t i = 0; i < triples; ++i)
    {
        triple(dst + i, src + i);
    }

    gettimeofday(&end, NULL);

    duration =
        (end.tv_sec - start.tv_sec) + 1e-6 * (end.tv_usec - start.tv_usec);

    rate = 3 * triples / duration / 1e6;

    fprintf(stderr, "%s\t%zu B %9.3f ms\t%9.3f MB/s\t", name, 3 * triples,
            1e3 * duration, rate);

    dst[16] = 0;
    fprintf(stderr, "%s\n", dst);
}

void
benchmark()
{
    const size_t triples = 64 * 1024 * 1024;
    uint8_t *dst, *src;

    if (NULL == (src = (uint8_t *)calloc(3 * triples, sizeof(uint8_t))))
        err(EXIT_FAILURE, "calloc");

    fprintf(stderr, "randomizing...\n");
    fill(src, 3 * triples);

    if (NULL == (dst = (uint8_t *)calloc(4 * triples, sizeof(uint8_t))))
        err(EXIT_FAILURE, "calloc");

    for (size_t i = 0; i < 3; ++i)
    {
        time_function(base64triple_1, "base64triple_1", dst, src, triples);
        time_function(base64triple_2, "base64triple_2", dst, src, triples);
        time_function(base64triple_3, "base64triple_3", dst, src, triples);
        time_function(base64triple_4, "base64triple_4", dst, src, triples);
        time_function(base64triple_5, "base64triple_5", dst, src, triples);
        time_function(base64triple_6, "base64triple_6", dst, src, triples);
        time_function(base64triple_7, "base64triple_7", dst, src, triples);
        time_function(base64triple_8, "base64triple_8", dst, src, triples);
        time_function(base64triple_9, "base64triple_9", dst, src, triples);
        fprintf(stderr, "\n");
    }
}

int
main()
{
    const size_t triples = 1024;
    size_t rbytes, rtriples;
    uint8_t pad;
    uint8_t *buf, *dst, *src, last[4];

    if (NULL == (buf = (uint8_t *)calloc(triples, 4 * sizeof(uint8_t))))
        err(EXIT_FAILURE, "calloc");

    dst = buf;
    src = buf + triples;

    for (;;)
    {
        rbytes = fread(src, 1, 3 * triples, stdin);
        rtriples = rbytes / 3;

        for (size_t i = 0; i < rtriples; ++i)
            base64triple_2(dst + 4 * i, src + 3 * i);

        fwrite(dst, 4, rtriples, stdout);

        if (rtriples == triples)
            continue;

        if (0 == rbytes % 3)
            break;

        src[rbytes + 0] = 0;
        src[rbytes + 1] = 0;

        pad = (3 - rbytes % 3) % 3;

        base64triple_2(last, src + 3 * rtriples);
        for (size_t i = 0; i <= pad; ++i)
            last[4 - i] = '=';

        fwrite(last, 1, 4, stdout);
        break;
    }

    return EXIT_SUCCESS;
}
