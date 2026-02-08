#include "../include/strlib.h"

unsigned int strlen2(const char *s)
{
    unsigned int n = 0;
    while (s[n] != '\0')
        n++;
    return n;
}

int streq(const char *a, const char *b)
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i])
            return 0;
        i++;
    }
    return (a[i] == '\0' && b[i] == '\0');
}

void strcpy2(char *dst, const char *src)
{
    int i = 0;
    while (src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

