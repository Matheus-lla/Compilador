#include <stdio.h>
#include "lexer/lexer.h"

int main(int argc, char* argv[]){
    FILE *file;
    char ch;
    TOKEN token;
    char *str;
    int i = 0; 

    file = fopen(argv[1], "r");

    while (!(feof(file))){
        token = SCANNER(file);
        printf("lexema: |%s|  --  class: |%s|\n", token.lexema.c_str(), token.token_class);
        i = 0;
        while(token.lexema[i] != '\0'){
            printf("%c|", token.lexema[i]);
            i++;
        }
        printf("\n%d %c\n", i, token.lexema[i]);
    }
    
    // token = SCANNER(file);
    // token = SCANNER(file);

    return 0;
}