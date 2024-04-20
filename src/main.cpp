#include "lexer/lexer.h"
#include <unordered_map>

int main(int argc, char* argv[]){
    std::unordered_map<std::string, TOKEN> SYMBOLS_TABLE = {
        {};
    };

    FILE *file;
    char ch;
    TOKEN token;
    char *str;

    file = fopen(argv[1], "r");

    while (!(feof(file))){
        token = SCANNER(file);
        printf("class: %4s - |%s|\n", token.token_class.c_str(), token.lexema.c_str());
    }
    
    return 0;
}