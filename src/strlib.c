/*

File Name: strlib.c
Author(s): Slater Dennington, Ochihai Omuha, Salman Tajammal, Dan Meulendyk
Creation Date: Feb 12th, 2026
Last Modified Date: Feb 21st, 2026
Purpose: Contains all functions for string implementation 

*/

#include "../include/strlib.h"

/*

FUNCTION: strlen2
PURPOSE: Calculates the length of string
INPUT: *s - String to be calculated
OUTPUT: n - Number of char

*/

unsigned int strlen2(const char *s)
{
    unsigned int n = 0;
    while (s[n] != '\0')
        n++;
    return n;
}

/*

FUNCTION: streq
PURPOSE: Compares strings and sees if equal
INPUT: *a - String 1
       *b - String 2 
OUTPUT: 1 if true, 0 if false

*/


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

/*

FUNCTION: strcpy2
PURPOSE: Copies a string
INPUT: *dst - Destination string
       *src - Source string
OUTPUT: none
*/


void strcpy2(char *dst, const char *src)
{
    int i = 0;
    while (src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

/*

FUNCTION: strcntn
PURPOSE: Assesses if string contains a char
INPUT: *s - String to be assessed
       c - Char to be found
OUTPUT: 1 if true, 0 if false

*/


int strcntn(const char *s, char c)
{
  while(*s){
    if(*s == c)
      return 1;
    s++;
  }
  return 0;
}
