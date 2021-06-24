#include <stdio.h>

int main( int argc, char* argv[] ){
    unsigned char v1 = -128;
    unsigned char v2 = 128;
    unsigned char v3 = 128;
    unsigned char v4 = 128;
    unsigned char v5 = 5<<5;
    printf("%d\n", (v1 >= v5));
    return 0;
}