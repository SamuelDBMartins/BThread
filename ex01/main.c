#include <stdio.h>

int callee(int x, int y) {
    return 1;
}

void main() {
    int a,b;
    a = 13;
    b = 17;
    callee(a, b);
}
