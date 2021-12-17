#include <string.h>

char* strtok_r(char *string, const char *control, char **lasts)
{
    unsigned char *str;
    const unsigned char *ctrl = (unsigned char *)control;

    unsigned char map[32];
    int n;

    for (n = 0; n < 32; n++)
        map[n] = 0;

    do {
        map[*ctrl >> 3] |= (1 << (*ctrl & 7));
    } while (*ctrl++);

    if (string) {
        str = (unsigned char *)string;
    } else {
        str = (unsigned char *)*lasts;
    }

    while ((map[*str >> 3] & (1 << (*str & 7))) && *str)
        str++;
    string = (char *)str;

    for ( ; *str; str++) {
        if (map[*str >> 3] & (1 << (*str & 7))) {
            *str++ = '\0';
            break;
        }
    }

    *lasts = (char *)str;

    if (string == (char *)str) {
        return 0;
    } else {
        return string;
    }
}
