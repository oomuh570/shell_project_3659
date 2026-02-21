#ifndef STRLIB_H
#define STRLIB_H

// returns the length of string
unsigned int strlen2(const char *s);

// returns 1 if equal, 0 if not
int streq(const char *a, const char *b);

// copies source into destination
void strcpy2(char *dst, const char *src);

//Checks if a string contains a character
int strcntn(const char *s, char c);

#endif
