#include "lexer/lexer.h"
#include "parser.h"
#include <unordered_map>
#include <stack>

void init_symbols_table(std::unordered_map<std::string, TOKEN>* SYMBOLS_TABLE);

int main(int argc, char* argv[]){
    std::unordered_map<std::string, TOKEN> SYMBOLS_TABLE;

    FILE *file;
    char ch;
    TOKEN token;
    TOKEN token_aux;
    std::string lexema;

    init_symbols_table(&SYMBOLS_TABLE);

    file = fopen(argv[1], "r");

    printf("\n");
    printf("\n");

    for (size_t i = 0; i < 77; i++)
    {
        for (size_t j = 0; j < 45; j++)
        {
            printf("%3d|", PARSER_TRANSITION_TABLE[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    printf("\n");


    while (!(feof(file))){
        token = SCANNER(file);
        
        if (token.token_class == TOKEN_CLASS[9]){
            if (auto result_tabela = SYMBOLS_TABLE.find(token.lexema); result_tabela == SYMBOLS_TABLE.end()){
                SYMBOLS_TABLE.insert({token.lexema, token});
                printf("Classe: %s, Lexema: %s, Tipo: %s\n", 
                                        token.token_class.c_str(),
                                        token.lexema.c_str(),
                                        token.type.c_str());
            }
            else{
                if (result_tabela->second.lexema == result_tabela->second.token_class and result_tabela->second.token_class == result_tabela->second.type){
                    printf("Classe: %s, Lexema: %s, Tipo: %s\n",
                                        result_tabela->second.token_class.c_str(),
                                        result_tabela->second.lexema.c_str(),
                                        result_tabela->second.type.c_str());
                }
            }
        }
        else{
            if(token.token_class != TOKEN_CLASS[8]){
                printf("Classe: %s, Lexema: %s, Tipo: %s\n", 
                                        token.token_class.c_str(),
                                        token.lexema.c_str(),
                                        token.type.c_str());
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