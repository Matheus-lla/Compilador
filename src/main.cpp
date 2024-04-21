#include "lexer/lexer.h"
#include <unordered_map>

void init_symbols_table(std::unordered_map<std::string, TOKEN>* SYMBOLS_TABLE);

int main(int argc, char* argv[]){
    std::unordered_map<std::string, TOKEN> SYMBOLS_TABLE;

    FILE *file;
    char ch;
    TOKEN token;
    TOKEN token_aux;

    init_symbols_table(&SYMBOLS_TABLE);

    file = fopen(argv[1], "r");

    while (!(feof(file))){
        token = SCANNER(file);
        
        if (token.token_class == TOKEN_CLASS[9]){
            if (auto search = SYMBOLS_TABLE.find(token.lexema); search == SYMBOLS_TABLE.end()){
                SYMBOLS_TABLE.insert({token.lexema, token});
                printf("print novo id: |%s| - |%s| - |%s| - lin:|%d| - col:|%d|\n", 
                                        token.lexema.c_str(),
                                        token.token_class.c_str(),
                                        token.type.c_str(),
                                        token.line,
                                        token.col);
            }
            else{
                if (search->second.lexema == search->second.token_class and search->second.token_class == search->second.type){
                    printf("print reservada: |%s| - |%s| - |%s| - lin:|%d| - col:|%d|\n",
                                        search->second.lexema.c_str(),
                                        search->second.token_class.c_str(),
                                        search->second.type.c_str(),
                                        token.line,
                                        token.col);
                }
            }
            
        }
         
    }
    



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