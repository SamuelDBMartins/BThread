#include <stdio.h>
#include <setjmp.h>
#include <zconf.h>

static jmp_buf buf;

void dothings() {
    printf("Now I'm here\n");
    sleep(3);
    longjmp(buf, 42);
    printf("This is never printed\n");
}

int main() {
    if (!setjmp(buf)) { // the first time returns 0
        dothings();
    } else {
        printf("Now I'm there\n");
    }
    return 0;
}