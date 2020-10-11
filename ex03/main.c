#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
static jmp_buf main_buf, dothings_buf;

void dothings() {
    int z = 1313;
    if(!setjmp(dothings_buf)) {
        printf("Now I'm here, z=%d\n", z);
        longjmp(main_buf, 42);
    } else {
        printf("Now I'm back here, z=%d\n", z);
        exit(0);
    }
}

void cushion() {
    char data_cushion[1000];
    data_cushion[999] = 1;
    dothings();
}

int main() {
    if (!setjmp(main_buf)) {
        cushion();
    } else {
        longjmp(dothings_buf, 17);
    }
    return 0;
}