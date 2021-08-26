#include <cstdio>
#include <cstdint>

extern "C" void print_checked(char* ptr) {
    if ((uint64_t)ptr > 0xffff) {
        printf("dlsym(???, %s);\n", ptr);
    }
}