int strncmp(const char *s1, const char *s2, uint32_t n);
void *memcpy(void *dest, const void *src, uint32_t n);
char *strncpy(char *dest, const char *src, uint32_t n);
int memcmp(const char *s1, const char *s2, uint32_t n);
int strlen(const char *s1);
void *memset(void *s, int c, uint32_t n);
void *memset32(void *s, int c, uint32_t n);
void test_memset(void *addr, int value, int size);
