#include "lexer/lexer.h"
#include <unordered_map>

void init_symbols_table(std::unordered_map<std::string, TOKEN>* SYMBOLS_TABLE);

int main(int argc, char* argv[]){
    std::unordered_map<std::string, TOKEN> SYMBOLS_TABLE;

    FILE *file;
    char ch;
    TOKEN token;
    char *str;

    init_symbols_table(&SYMBOLS_TABLE);

    file = fopen(argv[1], "r");

    while (!(feof(file))){
        token = SCANNER(file);
        if (token.token_class == TOKEN_CLASS[10]){
            SYMBOLS_TABLE.find(token.lexema);
        }
         
    }
    
    printf("%s, %s, %s", SYMBOLS_TABLE.at("inicio").lexema.c_str(), SYMBOLS_TABLE.at("varinicio").token_class.c_str(), SYMBOLS_TABLE.at("fim").type.c_str());


    return 0;
}

void init_symbols_table(std::unordered_map<std::string, TOKEN>* SYMBOLS_TABLE){
    SYMBOLS_TABLE->insert({"inicio", make_palavra_reservada("inicio")});
    SYMBOLS_TABLE->insert({"varinicio", make_palavra_reservada("varinicio")});
    SYMBOLS_TABLE->insert({"varfim", make_palavra_reservada("varfim")});
    SYMBOLS_TABLE->insert({"escreva", make_palavra_reservada("escreva")});
    SYMBOLS_TABLE->insert({"leia", make_palavra_reservada("leia")});
    SYMBOLS_TABLE->insert({"se", make_palavra_reservada("se")});
    SYMBOLS_TABLE->insert({"entao", make_palavra_reservada("entao")});
    SYMBOLS_TABLE->insert({"fimse", make_palavra_reservada("fimse")});
    SYMBOLS_TABLE->insert({"repita", make_palavra_reservada("repita")});
    SYMBOLS_TABLE->insert({"fimrepita", make_palavra_reservada("fimrepita")});
    SYMBOLS_TABLE->insert({"fim", make_palavra_reservada("fim")});
    SYMBOLS_TABLE->insert({"inteiro", make_palavra_reservada("inteiro")});
    SYMBOLS_TABLE->insert({"literal", make_palavra_reservada("literal")});
    SYMBOLS_TABLE->insert({"real", make_palavra_reservada("real")});
}