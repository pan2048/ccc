#include <string.h>
#include "../external/external.h"
#include "lib.h"

void *memCopy(char *dest, char *src, int len)
{
  char *d = dest;
  char *s = src;
  while (len--)
    *d++ = *s++;
  return ((void *)dest);
}

/* Find the last occurrence of C in S.  */
char *strRchr(char *s, int c)
{
  char *found, *p;
  c = (char)c;
  /* Since strchr is fast, we use it rather than the obvious loop.  */
  if (c == '\0')
    return (strChr(s, '\0'));
  found = NULL;
  while ((p = strChr(s, c)) != NULL)
  {
    found = p;
    s = p + 1;
  }
  return (found);
}

char *strChr(char *s, int c)
{
  c = (char)c;
  if (*s == c)
  {
    return (s);
  }
  while (*s++)
  {
    if (*s == c)
    {
      return (s);
    }
  }
  return ((char *)NULL);
}

int strCmp(char *p1, char *p2)
{
  char *s1 = p1;
  char *s2 = p2;
  char c1, c2;
  c1 = *s1;
  c2 = *s2;
  s1++;
  s2++;
  if (c1 == '\0')
    return (c1 - c2);
  while (c1 == c2)
  {
    c1 = *s1;
    c2 = *s2;
    s1++;
    s2++;
    if (c1 == '\0')
      return (c1 - c2);
  }
  return (c1 - c2);
}

char *strDup(char *s)
{
  int len = strlen(s) + 1;
  char *new = memAlloc(len);
  if (new == (char *)NULL)
    return ((char *)NULL);

  char *ret = (char *)memCopy(new, s, len);
  return (ret);
}
