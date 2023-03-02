#include <stdio.h>

const char* toStr() {
    return "Hello!"; 
}

int main() {
    // This is a line comment!
    if (1 == 1) {
        const char* s = toStr();
    }
    printf("Hello, World!\n");
    return 0;
}