#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LINESZ 256

int main(void){
    char ch;

    while((ch = fgetc(stdin)) != EOF){
        printf("%c", ch);
    }

    return 0;
}