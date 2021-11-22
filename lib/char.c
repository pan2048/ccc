int charIsAlpha(int c)
{
  if ((c >= 'a' && c <= 'z') ||
      (c >= 'A' && c <= 'Z'))
  {
    return (1);
  }
  return (0);
}

int charIsDigit(int c)
{
  if (c >= '0' && c <= '9')
  {
    return (1);
  }
  return (0);
}

int charIsXDigit(int c)
{
  if ((c >= '0' && c <= '9') ||
      (c >= 'a' && c <= 'f') ||
      (c >= 'A' && c <= 'F'))
  {
    return (1);
  }
  return (0);
}

int charToLower(int c)
{
  if (c >= 'A' && c <= 'Z')
  {
    int ret = c + 'a' - 'A';
    return (ret);
  }
  return (c);
}

int charPos(char *s, int c)
{
  int i;
  for (i = 0; s[i] != '\0'; i++)
  {
    if (s[i] == (char)c)
    {
      return (i);
    }
  }
  return (-1);
}
