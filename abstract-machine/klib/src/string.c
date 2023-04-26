#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  char* p = (void *)s;
  while (*p) p++;
  return p - s;
  //panic("Not implemented");
}

char *strcpy(char *dst, const char *src) {
  //panic("Not implemented");
  size_t n = strlen(src);
  for(unsigned int i = 0; i < n; i++){
    dst[i] = src[i];
  }
  dst[n] = 0;
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  for(unsigned int i = 0; i < n; i++) dst[i] = src[i];
  return dst;
}

char *strcat(char *dst, const char *src) {
  size_t offset = strlen(dst);
  size_t n = strlen(src);
  for (unsigned int i = 0; i < n; i++)
    dst[offset + i] = src[i];
  dst[offset + n] = 0;
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  char *p1 = (void*)s1;
  char *p2 = (void*)s2;
  while (*p1 && *p2 && (*p1 == *p2)) {
    p1++; p2++;
  }
  return *p1 - *p2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  char *p1 = (void*)s1;
  char *p2 = (void*)s2;
  while (*p1 && *p2 && (*p1 == *p2) && --n) {
    p1++; p2++;
  }
  return *p1 - *p2;
}

void *memset(void *s, int c, size_t n) {
  char *p = s;
  for (unsigned int i = 0; i < n; i++) {
    p[i] = (char)(c & 0xff);
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  char *pd = dst;
  const char *ps = src;
  for (unsigned int i = 0; i < n; i++) {
    pd[i] = ps[i];
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  return memmove(out, in, n);
}

int memcmp(const void *s1, const void *s2, size_t n) {
  char *p1 = (void*)s1;
  char *p2 = (void*)s2;
  while (*p1 == *p2 && --n) {
    p1++; p2++;
  }
  return *p1 - *p2;
}

#endif
