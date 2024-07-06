#include "lexer/lexer.h"
#include "parser.h"
#include <unordered_map>
#include <stack>

void init_symbols_table(std::unordered_map<std::string, TOKEN>* SYMBOLS_TABLE);
int get_token_class_value(TOKEN token);

int main(int argc, char* argv[]){
    std::unordered_map<std::string, TOKEN> SYMBOLS_TABLE;
    std::stack<int> PARSER_STACK;

    int STATE = 0;
    int NEXT_STATE;
    int stack_top;
    int token_class_value;

    FILE *file;
    char ch;
    TOKEN token;
    TOKEN token_aux;
    std::string lexema;

    init_symbols_table(&SYMBOLS_TABLE);
    PARSER_STACK.push(0);

    if(argc < 2) {
        printf("\n\nChamada sem argumento!!\n\n");
        exit(1);
    }
    file = fopen(argv[1], "r");


    while (!(feof(file))){
        token = SCANNER(file);
        
        if (token.token_class == TOKEN_CLASS[9]){
            if (auto result_from_table = SYMBOLS_TABLE.find(token.lexema); result_from_table == SYMBOLS_TABLE.end()){
                SYMBOLS_TABLE.insert({token.lexema, token});
            }
            else{
                if (result_from_table->second.lexema == result_from_table->second.token_class 
                    and result_from_table->second.token_class == result_from_table->second.type){
                        token = result_from_table->second;
                }
            }
        }

        if(token.token_class != TOKEN_CLASS[8] && token.token_class != TOKEN_CLASS[12]){
            stack_top = PARSER_STACK.top();
            token_class_value = get_token_class_value(token);

            printf("\nstack_top: %d \ntoken_class_value: %d   |%s|\n", stack_top, token_class_value, token.token_class.c_str());
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

int get_token_class_value(TOKEN token){
    if(!token.token_class.compare("inicio")) {
        return 0;
    }
    else if (!token.token_class.compare("varinicio")) {
        return 1;
    }
    else if (!token.token_class.compare("varfim")) {
        return 2;
    }
    else if (!token.token_class.compare("PT_V")) {
        return 3;
    }
    else if (!token.token_class.compare("ID")) {
        return 4;
    }
    else if (!token.token_class.compare("VIR")) {
        return 5;
    }
    else if (!token.token_class.compare("inteiro")) {
        return 6;
    }
    else if (!token.token_class.compare("real")) {
        return 7;
    }
    else if (!token.token_class.compare("literal")) {
        return 8;
    }
    else if (!token.token_class.compare("leia")) {
        return 9;
    }
    else if (!token.token_class.compare("escreva")) {
        return 10;
    }
    else if (!token.token_class.compare("LIT")) {
        return 11;
    }
    else if (!token.token_class.compare("NUM")) {
        return 12;
    }
    else if (!token.token_class.compare("RCB")) {
        return 13;
    }
    else if (!token.token_class.compare("OPM")) {
        return 14;
    }
    else if (!token.token_class.compare("se")) {
        return 15;
    }
    else if (!token.token_class.compare("AB_P")) {
        return 16;
    }
    else if (!token.token_class.compare("FC_P")) {
        return 17;
    }
    else if (!token.token_class.compare("entao")) {
        return 18;
    }
    else if (!token.token_class.compare("OPR")) {
        return 19;
    }
    else if (!token.token_class.compare("fimse")) {
        return 20;
    }
    else if (!token.token_class.compare("repita")) {
        return 21;
    }
    else if (!token.token_class.compare("fimrepita")) {
        return 22;
    }
    else if (!token.token_class.compare("fim")) {
        return 23;
    }
    else if (!token.token_class.compare("EOF_CLASS")) {
        return 24;
    }
    else {
        return -1;
    }
}